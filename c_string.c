#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "c_string.h"

struct c_string{
    int size;
    int capacity;
    char* data;
};

typedef struct c_string Str;

Status string_specific_resize(C_STRING hMy_string, int new_size){
    Str *pStr = (Str*)hMy_string;
    char* t = (char*)malloc(sizeof(char) * new_size);
    if(t == NULL)
        return FAILURE;
    int i;
    for(i = 0; i < pStr->size; i++)
        t[i] = pStr->data[i];
    free(pStr->data);
    pStr->data = t;
    pStr->capacity = new_size;
    return SUCCESS;
}

Status string_resize(C_STRING hMy_string){
    return string_specific_resize(hMy_string, ((Str*)hMy_string)->capacity * 2);
}

C_STRING string_init_default(void){
    Str *pStr = (Str*)malloc(sizeof(Str));
    if(pStr != NULL){
        pStr->size = 0;
        pStr->capacity = 7;
        pStr->data = (char*)malloc(sizeof(char) * pStr->capacity);
        if(pStr->data == NULL){
            free(pStr);
            pStr = NULL;
        }
    }
    return (C_STRING)pStr;
}

C_STRING string_init_c_string(const char* c_string){
    Str *pStr = (Str*)malloc(sizeof(Str));
    if(pStr != NULL){
        int string_len;
        for(string_len = 0; c_string[string_len]; string_len++);
        pStr->size = string_len;
        pStr->capacity = pStr->size + 1;
        pStr->data = (char*)malloc(sizeof(char) * pStr->capacity);
        if(pStr->data == NULL){
            free(pStr);
            pStr = NULL;
        }else{
            int i;
            for(i = 0; i < pStr->capacity; i++)
                pStr->data[i] = c_string[i];
        }
    }
    return (C_STRING)pStr;
}

int string_get_capacity(C_STRING hMy_string){
    Str *pStr = (Str*)hMy_string;
    return pStr->capacity;
}

int string_get_size(C_STRING hMy_string){
    Str *pStr = (Str*)hMy_string;
    return pStr->size; 
}

int string_compare(C_STRING hLeft_string, C_STRING hRight_string){
    char *pDataL = string_c_str(hLeft_string);
    char *pDataR = string_c_str(hRight_string);
    for(;;){
        if(*pDataL == 0 && *pDataR == 0)
            return 0;
        if(*pDataL - *pDataR != 0)
            return *pDataL - *pDataR;
        pDataL++;
        pDataR++;
    }
}

char* get_string(FILE* fp){
    char *r = malloc(sizeof(char) * 2);
    char c = fgetc(fp);
    int i = 0;
    if(c == EOF){
        free(r);
        return NULL;
    }
    while(c != EOF && !isspace(c)){
        r[i++] = c;
        c = fgetc(fp);
        char *t = realloc(r, sizeof(char) * (2 + i));
        if(t == NULL){
            free(r); return NULL;
        }
        r = t;
    }
    if(c == ' ')
        ungetc(c, fp);
    r[i] = '\0';
    return r;
}

int get_string_len(char* str){
    int i;
    for(i = 0; str[i]; i++);
    return i;
}

Status string_extraction(C_STRING hMy_string, FILE* fp){
    Str *pStr = (Str*)hMy_string;
    char* str = get_string(fp);
    if(str == NULL){
        free(str);
        return FAILURE;
    }
    int len = get_string_len(str);
    if(len >= pStr->capacity){
        if(!string_resize(hMy_string)){
            free(str);
            return FAILURE;
        }
    }
    int i;
    for(i = 0; i < pStr->size; i++)
        pStr->data[i] = '\0'; //clear out string first
    for(i = 0; i < len; i++)
        pStr->data[i] = str[i];
    pStr->size = len;
    free(str);
    return SUCCESS;
}

Status string_insertion(C_STRING hMy_string, FILE* fp){
    Str *pStr = (Str*)hMy_string;
    int i;
    for(i = 0; i < pStr->size; i++)
        if(!fprintf(fp, "%c", pStr->data[i]))
            return FAILURE;
    return SUCCESS;
}

Status string_push_back(C_STRING hMy_string, char item){
    Str *pStr = (Str*)hMy_string;
    if(pStr->size >= pStr->capacity) //size + 1 for new 'item' and null termintor
        if(!string_resize(hMy_string))
            return FAILURE;
    pStr->data[pStr->size++] = item;
    return SUCCESS;
}

Status string_pop_back(C_STRING hMy_string){
    Str *pStr = (Str*)hMy_string;
    if(pStr->size > 0){
        pStr->data[--pStr->size] = '\0'; //clear data previously there
        return SUCCESS;
    }
    return FAILURE;
}

char* string_at(C_STRING hMy_string, int index){
    Str *pStr = (Str*)hMy_string;
    if(index < 0 || index >= pStr->size)
        return NULL;
    return &pStr->data[index];
}

char* string_c_str(C_STRING hMy_string){
    Str *pStr = (Str*)hMy_string;
    if(pStr->size + 1 >= pStr->capacity)
        if(!string_resize(hMy_string))
            return NULL;
    pStr->data[pStr->size] = '\0';
    return pStr->data;
}

Status string_concat(C_STRING hResult, C_STRING hAppend){
    Str *pRes = (Str*)hResult;
    Str *pApp = (Str*)hAppend;
    int size = pRes->size + pApp->size;
    char* r = (char*)malloc(sizeof(char) * (size + 1));
    if(r == NULL)
        return FAILURE;
    int i;
    for(i = 0; i < pRes->size; i++){
        r[i] = pRes->data[i];
    }
    for(;i - pRes->size < pApp->size; i++){
        r[i] = pApp->data[i - pRes->size];
    }
    free(pRes->data);
    pRes->data = r;
    pRes->size = size;
    pRes->capacity = size + 1;
    return SUCCESS;
}

Boolean string_empty(C_STRING hMy_string){
    Str *pStr = (Str*)hMy_string;
    return (Boolean)(pStr->size == 0);
}

Str* string_deep_copy(Str* s){
    Str *pStr = (Str*)malloc(sizeof(Str));
    if(pStr == NULL)
        return NULL;
    pStr->size = s->size;
    pStr->capacity = s->capacity;
    pStr->data = (char*)malloc(sizeof(char) * pStr->capacity);
    if(pStr->data == NULL){
        free(pStr);
        return NULL;
    }else{
        int i;
        for(i = 0; i < s->size; i++)
            pStr->data[i] = s->data[i];
    }
    return pStr;
}

Status string_assignment(ITEM* pLeft, ITEM Right){
    Str* pStrR = (Str*)Right;
    if(*pLeft != NULL){
        if(string_specific_resize(*pLeft, pStrR->capacity)){
            Str* pStrL = (Str*)*pLeft;
            int i;
            for(i = 0; i < pStrR->size; i++)
                pStrL->data[i] = pStrR->data[i];
        }else
            return FAILURE;
    }else{
        Str* str = string_deep_copy(pStrR);
        if(pLeft == NULL)
            printf("NULL");
        if(str != NULL)
            *pLeft = (ITEM)str;
        else
            return FAILURE;
    }
    return SUCCESS;
}

void string_destroy(ITEM *phMy_string){
    Str *pStr = (Str*)*phMy_string;
    free(pStr->data);
    free(pStr);
    *phMy_string = NULL;
}