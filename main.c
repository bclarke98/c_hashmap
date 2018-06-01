/*
    Author:         Brendan Clarke
    Purpose:        Create a hashmap in raw C that allows any data type to be used as keys/values
    Last Modified:  6/1/18
    
    main.c is a test file for the functionality of the hashmap opaque object.
    It is not required for use of the hashmap opaque object.
*/

#include <stdlib.h>
#include <stdio.h>
#include "c_string.h"
#include "c_hashmap.h"

#define NUM 10

int string_hash(ITEM item, int capacity){
    C_STRING str = (C_STRING)item;
    int i;
    int n = 0;
    for(i = 0; i < string_get_size(str); i++)
        n += (int)(*string_at(str, i));
    return n % capacity;
}

int main(int argc, char* argv[]) {
    FUNC_PAIR funcs = func_pair_init_default(string_assignment, string_destroy, string_hash, string_compare);
    HASHMAP hMap = hashmap_init_default(funcs, funcs);
    
    int i, j;
    C_STRING *keys = (C_STRING*)malloc(sizeof(C_STRING) * NUM);
    C_STRING *vals = (C_STRING*)malloc(sizeof(C_STRING) * NUM);
    printf("Displaying key/value pairs...\n");
    for(i = 0; i < NUM; i++){
        keys[i] = string_init_default();
        vals[i] = string_init_default();
        for(j = 0; j < 4; j++){
            string_push_back(keys[i], 'a' + i);
            string_push_back(vals[i], 'z' - i);
        }
        printf("k: [%s] | v: [%s] | h: [%d]\n", string_c_str(keys[i]), string_c_str(vals[i]), string_hash(keys[i], 100));
        hashmap_put(hMap, keys[i], vals[i]);
    }
    
    C_STRING result = (C_STRING)hashmap_get(hMap, keys[0]);
    printf("Displaying result of hashmap_get with first key...\n");
    printf("%s\n", string_c_str(result));
    
    HASHMAP hNew = hashmap_deep_copy(hMap, 1000);
    printf("Displaying debug output from resized deep copy of original hashmap...\n");
    hashmap_debug(hNew);
    
        
    for(i = 0; i < NUM; i++){
        string_destroy(&keys[i]);
        string_destroy(&vals[i]);
    } 
    free(keys);
    free(vals);
    hashmap_destroy(&hMap);
    hashmap_destroy(&hNew);
    free(funcs);
    return 0;
}
