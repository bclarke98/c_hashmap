#ifndef C_VECTOR_H
#define C_VECTOR_H

#include "status.h"
#include "generic.h"

typedef void* VECTOR;

VECTOR vector_init_default(Status (*assignment)(ITEM* phLeft, ITEM hRight), void (*destroy)(ITEM* phItem));

Status vector_push_back(VECTOR hVector, ITEM n);

Status vector_pop_back(VECTOR hVector);

ITEM vector_at(VECTOR hVector, int index);

VECTOR vector_deep_copy(VECTOR hVector);

int vector_get_capacity(VECTOR hVector);

int vector_get_size(VECTOR hVector);

Boolean vector_empty(VECTOR hVector);

void vector_destroy(VECTOR* phVector);

#endif
