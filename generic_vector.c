#include <stdio.h>
#include <stdlib.h>
#include "generic_vector.h"

struct c_vector{
    int size;
    int capacity;
    ITEM* data;
    Status(*item_assignment)(ITEM* phLeft, ITEM hRight);
    void(*item_destroy)(ITEM* phItem);
};

typedef struct c_vector Vector;

Status vector_resize(VECTOR hVector){
    Vector *pVector = (Vector*)hVector;
    ITEM* temp = (ITEM*)malloc(sizeof(ITEM) * pVector->capacity * 2);
    if(temp == NULL)
        return FAILURE;
    int i;
    for(i = 0; i < pVector->size; i++)
        temp[i] = pVector->data[i];
    pVector->capacity *= 2;
    for(; i < pVector->capacity; i++)
        temp[i] = NULL;
    free(pVector->data);
    pVector->data = temp;
    return SUCCESS;
}

VECTOR vector_init_default(Status(*assignment)(ITEM* phLeft, ITEM hRight), void(*destroy)(ITEM* phItem)){
    Vector *pVector = (Vector*)malloc(sizeof(Vector));
    int i;
    if(pVector != NULL){
        pVector->size = 0;
        pVector->capacity = 1;
        pVector->item_assignment = assignment;
        pVector->item_destroy = destroy;
        pVector->data = (ITEM*)malloc(sizeof(ITEM) * pVector->capacity);
        if(pVector->data == NULL){
            free(pVector);
            return NULL;
        }
        for(i = 0; i < pVector->capacity; i++)
            pVector->data[i] = NULL;
    }
    return pVector;
}

Status vector_push_back(VECTOR hVector, ITEM n){
    Vector *pVector = (Vector*)hVector;
    if(pVector == NULL)
        printf("NULL!\n");
    if(pVector->size >= pVector->capacity)
        if(!vector_resize(hVector))
            return FAILURE;
    if(pVector->item_assignment(&(pVector->data[pVector->size]), n)){
        pVector->size++;
        return SUCCESS;
    }
    return FAILURE;
}

Status vector_pop_back(VECTOR hVector){
    Vector *pVector = (Vector*)hVector;
    if(pVector->size < 1)
        return FAILURE;
    pVector->item_destroy(&(pVector->data[pVector->size - 1]));
    pVector->size--;
    return SUCCESS;
}

ITEM vector_at(VECTOR hVector, int index){
    Vector *pVector = (Vector*)hVector;
    if(index < 0 || pVector->size < index)
        return NULL; 
    return pVector->data[index];
}

VECTOR vector_deep_copy(VECTOR hVector){
    Vector *pVector = (Vector*)hVector;
    VECTOR rVector = vector_init_default(pVector->item_assignment, pVector->item_destroy);
    if(rVector == NULL)
        return NULL;
    int i;
    for(i = 0; i < vector_get_size(hVector); i++){
        if(!vector_push_back(rVector, vector_at(hVector, i))){
            vector_destroy(&rVector);
            return NULL;
        }
    }
    return rVector;
}

int vector_get_capacity(VECTOR hVector){
    return ((Vector*)hVector)->capacity;
}

int vector_get_size(VECTOR hVector){
    return ((Vector*)hVector)->size;
}

Boolean vector_empty(VECTOR hVector){
    return (Boolean)(((Vector*)hVector)->size < 1);
}

void vector_destroy(VECTOR* phVector){
    int i;
    Vector *pVector = (Vector*)*phVector;
    for(i = 0; i < pVector->size; i++)
        if(pVector->data[i] != NULL)
            pVector->item_destroy(&(pVector->data[i]));
    free(pVector->data);
    free(pVector);
    *phVector = NULL;
}



