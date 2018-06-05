#include <stdio.h>
#include <stdlib.h>
#include "c_hashmap.h"

typedef struct funcs{
    Status(*item_assignment)(ITEM* phLeft, ITEM hRight);
    void(*item_destroy)(ITEM* phItem);
    int(*hash)(ITEM hItem, int capacity);
    int(*compare)(ITEM hLeft, ITEM hRight);
} Funcs;

typedef struct bucket{
    int size;
    ITEM* keys;
    ITEM* values;
    Funcs* key_funcs;
    Funcs* val_funcs;
} Bucket;

typedef struct hashmap{
    int size;
    int capacity;
    Bucket** buckets;
    Funcs* key_funcs;
    Funcs* val_funcs;
} Hashmap;

FUNC_PAIR func_pair_init_default(Status (*assignment)(ITEM* phLeft, ITEM hRight), void (*destroy)(ITEM* phItem), int (*hash_func)(ITEM hItem, int capacity), int (*compare_func)(ITEM hLeft, ITEM hRight)){
    Funcs *pFunc = (Funcs*)malloc(sizeof(Funcs));
    if(pFunc != NULL){
        pFunc->item_assignment = assignment;
        pFunc->item_destroy = destroy;
        pFunc->hash = hash_func;
        pFunc->compare = compare_func;
    }
    return (FUNC_PAIR)pFunc;
}

Status func_assign(Funcs* pFunc, ITEM* phLeft, ITEM hRight){
    return pFunc->item_assignment(phLeft, hRight);
}

void func_destroy(Funcs* pFunc, ITEM* phItem){
    pFunc->item_destroy(phItem);
}

int func_hash(Funcs* pFunc, ITEM item, int capacity){
    return pFunc->hash(item, capacity);
}

int func_compare(Funcs* pFunc, ITEM hLeft, ITEM hRight){
    return pFunc->compare(hLeft, hRight);
}

Status resize_items(Bucket* pBucket){
    int i;
    ITEM *tKeys = (ITEM*)malloc(sizeof(ITEM) * (pBucket->size + 1));
    if(tKeys == NULL)
        return FAILURE;
    for(i = 0; i < pBucket->size; i++){
        func_assign(pBucket->key_funcs, &tKeys[i], pBucket->keys[i]);
        func_destroy(pBucket->key_funcs, &(pBucket->keys[i]));
    }
    pBucket->keys[i] = NULL;
    ITEM *tVals = (ITEM*)malloc(sizeof(ITEM) * (pBucket->size + 1));
    if(tVals == NULL){
        for(i = 0; i < pBucket->size; i++)
        func_destroy(pBucket->key_funcs, &(pBucket->keys[i]));
        free(tKeys);
        return FAILURE;
    }
    for(i = 0; i < pBucket->size; i++){
        func_assign(pBucket->val_funcs, &tVals[i], pBucket->values[i]);
        func_destroy(pBucket->val_funcs, &(pBucket->values[i]));
    }
    pBucket->values[i] = NULL;
    free(pBucket->keys);
    free(pBucket->values);
    pBucket->keys = tKeys;
    pBucket->values = tVals;
    return SUCCESS;
}

Bucket* bucket_init_pair(ITEM key, ITEM value, FUNC_PAIR key_funcs, FUNC_PAIR val_funcs){
    Bucket *pBucket = (Bucket*)malloc(sizeof(Bucket));
    if(pBucket != NULL){
        pBucket->size = 1;
        pBucket->key_funcs = (Funcs*)key_funcs;
        pBucket->val_funcs = (Funcs*)val_funcs;
        pBucket->keys = (ITEM*)malloc(sizeof(ITEM));
        if(pBucket->keys == NULL){
            free(pBucket);
            return NULL;
        }
        pBucket->keys[0] = NULL;
        func_assign(pBucket->key_funcs, &pBucket->keys[0], key);
        pBucket->values = (ITEM*)malloc(sizeof(ITEM));
        if(pBucket->values == NULL){
            free(pBucket->keys);
            free(pBucket);
            return NULL;
        }
        pBucket->values[0] = NULL;
        func_assign(pBucket->val_funcs, &pBucket->values[0], value);
    }
    return pBucket;
}

Status bucket_add_pair(Bucket* pBucket, ITEM key, ITEM value){
    resize_items(pBucket);
    if(!func_assign(pBucket->key_funcs, &(pBucket->keys[pBucket->size]), key))
        return FAILURE;
    if(!func_assign(pBucket->val_funcs, &(pBucket->values[pBucket->size]), value))
        return FAILURE;
    pBucket->size++;
    return SUCCESS;
}

HASHMAP hashmap_init_default(FUNC_PAIR key_funcs, FUNC_PAIR val_funcs){
    return hashmap_init_set_cap(key_funcs, val_funcs, 100);
}

HASHMAP hashmap_init_set_cap(FUNC_PAIR key_funcs, FUNC_PAIR val_funcs, int capacity){
    Hashmap *pMap = (Hashmap*)malloc(sizeof(Hashmap));
    if(pMap != NULL){
        pMap->size = 0;
        pMap->capacity = capacity;
        pMap->key_funcs = (Funcs*)key_funcs;
        pMap->val_funcs = (Funcs*)val_funcs;
        pMap->buckets = (Bucket**)malloc(sizeof(Bucket*) * capacity);
        if(pMap->buckets != NULL){
            int i;
            for(i = 0; i < capacity; i++)
                pMap->buckets[i] = NULL;
        }else{
            free(pMap);
            return NULL;
        }
    }
    return (HASHMAP)pMap;
}

Status hashmap_put(HASHMAP hMap, ITEM key, ITEM value){
    Hashmap *pMap = (Hashmap*)hMap;
    int index = func_hash(pMap->key_funcs, key, pMap->capacity);
    if(pMap->buckets[index] == NULL){
        pMap->buckets[index] = bucket_init_pair(key, value, pMap->key_funcs, pMap->val_funcs);
        pMap->size++;
    }else{
        bucket_add_pair(pMap->buckets[index], key, value);
    }
    return SUCCESS;
}

ITEM hashmap_get(HASHMAP hMap, ITEM key){
    Hashmap *pMap = (Hashmap*)hMap;
    int index = func_hash(pMap->key_funcs, key, pMap->capacity);
    if(index < 0 || index >= pMap->capacity || pMap->buckets[index] == NULL)
        return NULL;
    Bucket *pBucket = pMap->buckets[index];
    int i;
    for(i = 0; i < pBucket->size; i++)
        if(func_compare(pMap->key_funcs, key, pBucket->keys[i]) == 0)
            return pBucket->values[i];
    return NULL;
}

Status hashmap_remove(HASHMAP hMap, ITEM key){
    Hashmap *pMap = (Hashmap*)hMap;
    int index = func_hash(pMap->key_funcs, key, pMap->capacity);
    if(index < 0 || index >= pMap->capacity || pMap->buckets[index] == NULL)
        return FAILURE;
    Bucket *pBucket = pMap->buckets[index];
    int i;
    for(i = 0; i < pBucket->size; i++){
        if(func_compare(pMap->key_funcs, key, pBucket->keys[i]) == 0){
            pBucket->size--;
            func_destroy(pBucket->key_funcs, &pBucket->keys[i]);
            func_destroy(pBucket->val_funcs, &pBucket->values[i]);
            break;
        }
    }
    return NULL;
}

ITEM hashmap_pop(HASHMAP hMap, ITEM key){
    Hashmap *pMap = (Hashmap*)hMap;
    int index = func_hash(pMap->key_funcs, key, pMap->capacity);
    if(index < 0 || index >= pMap->capacity || pMap->buckets[index] == NULL)
        return FAILURE;
    Bucket *pBucket = pMap->buckets[index];
    int i;
    for(i = 0; i < pBucket->size; i++){
        if(func_compare(pMap->key_funcs, key, pBucket->keys[i]) == 0){
            pBucket->size--;
            func_destroy(pBucket->key_funcs, &pBucket->keys[i]);
            return pBucket->values[i];
        }
    }
    return NULL;
}

HASHMAP hashmap_deep_copy(HASHMAP hMap, int capacity){
    Hashmap *pMap = (Hashmap*)hMap;
    if(capacity < pMap->capacity)
        return NULL;
    HASHMAP hNew = hashmap_init_set_cap(pMap->key_funcs, pMap->val_funcs, capacity);
    if(hNew == NULL)
        return NULL;
    int i, j;
    for(i = 0; i < pMap->capacity; i++){
        if(pMap->buckets[i] != NULL){
            for(j = 0; j < pMap->buckets[i]->size; j++){
                if(!hashmap_put(hNew, pMap->buckets[i]->keys[j], pMap->buckets[i]->values[j])){
                    hashmap_destroy(&hNew);
                    return NULL;
                }
            }
        }
    }
    return hNew;
}

void hashmap_destroy(HASHMAP* phMap){
    Hashmap *pMap = (Hashmap*)*phMap;
    int i, j;
    for(i = 0; i < pMap->capacity; i++){
        if(pMap->buckets[i] != NULL){
            for(j = 0; j < pMap->buckets[i]->size; j++){
                func_destroy(pMap->key_funcs, &(pMap->buckets[i]->keys[j]));
                func_destroy(pMap->val_funcs, &(pMap->buckets[i]->values[j]));
            }
            free(pMap->buckets[i]->keys);
            free(pMap->buckets[i]->values);
            free(pMap->buckets[i]);
        }
    }
    free(pMap->buckets);
    free(pMap);
    *phMap = NULL;
}

void hashmap_debug(HASHMAP hMap){
    Hashmap *pMap = (Hashmap*)hMap;
    printf("Elements: %d\n", pMap->size);
    int i;
    for(i = 0; i < pMap->capacity; i++){
        printf(pMap->buckets[i] == NULL ? "[0]" : "[1]");
        printf((i + 1) % 10 == 0 ? "\n" : "");
    }
    printf("\n");
}
