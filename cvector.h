#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdio.h>
#include <stdlib.h>
#define CVECTOR_INIT_CAPACITY 16
#define CVECTOR_UNDEFINED  -1
#define CVECTOR_SUCCESS 0
#define CVECTOR_INIT(vec) cvector vec;\
	cvector_init(&vec)

typedef struct cvector
{
	void** items; // dynamic array of void*
	int capacity; // capacity of vector
	int size;    // how many elements in vector now
} cvector;

// cvector functions
int cvector_size(cvector* v);
int cvector_set_capacity(cvector* v, int capacity);
int cvector_push_back(cvector* v, void* item);
int cvector_set(cvector* v, int index, void* item);
void* cvector_get(cvector* v, int index);
int vector_delete(cvector* v, int index);
int cvector_free(cvector* v);
void cvector_init(cvector* v);


#endif
