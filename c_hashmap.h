#ifndef C_HASHMAP_H
#define C_HASHMAP_H
#include "generic_vector.h"
#include "c_string.h"

typedef void* HASHMAP;
typedef void* FUNC_PAIR;

FUNC_PAIR func_pair_init_default(Status (*assignment)(ITEM* phLeft, ITEM hRight), void (*destroy)(ITEM* phItem), int (*hash_func)(ITEM hItem, int capacity), int (*compare_func)(ITEM hLeft, ITEM hRight));

HASHMAP hashmap_init_default(FUNC_PAIR keyFuncs, FUNC_PAIR valFuncs);

HASHMAP hashmap_init_set_cap(FUNC_PAIR keyFuncs, FUNC_PAIR valFuncs, int capacity);

Status hashmap_put(HASHMAP hMap, ITEM key, ITEM value);

ITEM hashmap_get(HASHMAP hMap, ITEM key);

void hashmap_destroy(HASHMAP* phMap);

void hashmap_debug(HASHMAP hMap);

#endif
