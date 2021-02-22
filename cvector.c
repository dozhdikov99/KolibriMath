#include "cvector.h"

int cvector_size(cvector* v)
{
	int size_count = CVECTOR_UNDEFINED;
	if (v) { size_count = v->size; }
	return size_count;
}

int cvector_empty(cvector* v)
{
	return (cvector_size(v) == 0);
}

int cvector_set_capacity(cvector* v, int capacity)
{
	int status = CVECTOR_UNDEFINED;
	if (v)
	{
		void** items = realloc(v->items, sizeof(void*) * capacity);
		if (items)
		{
			v->items = items;
			v->capacity = capacity;
			status = CVECTOR_SUCCESS;
		}
	}
	return status;
}

int cvector_push_back(cvector* v, void* item)
{
	int status = CVECTOR_UNDEFINED;
	if (v)
	{
		if (v->capacity == v->size)
		{
			status = cvector_set_capacity(v, v->capacity * 2);
			if (status != CVECTOR_UNDEFINED) { v->items[v->size++] = item; }
		}
		else
		{
			v->items[v->size++] = item;
			status = CVECTOR_SUCCESS;
		}
	}
	return status;
}

int cvector_set(cvector* v, int index, void* item)
{
	int status = CVECTOR_UNDEFINED;
	if (v)
	{
		if ((index >= 0) && (index < v->size))
		{
			v->items[index] = item;
			status = CVECTOR_SUCCESS;
		}
	}
	return status;
}

void* cvector_get(cvector* v, int index)
{
	void* read_data = NULL;
	if (v) { if ((index >= 0) && (index < v->size)) { read_data = v->items[index]; } }
	return read_data;
}

int vector_delete(cvector* v, int index)
{
	int status = CVECTOR_UNDEFINED, i = 0;
	if (v)
	{
		if ((index < 0) || (index >= v->size)) { return status; }
		v->items[index] = NULL;
		for (i = index; i < v->size - 1; ++i) // shift left by 1 element
		{
			v->items[i] = v->items[i + 1];
			v->items[i + 1] = NULL;
		}
		v->size--;
		if ((v->size > 0) && ((v->size) == (v->capacity / 4))) { cvector_set_capacity(v, v->capacity / 2); }
		status = CVECTOR_SUCCESS;
	}
	return status;
}

int cvector_free(cvector* v)
{
	int status = CVECTOR_UNDEFINED;
	if (v)
	{
		free(v->items);
		v->items = NULL;
		status = CVECTOR_SUCCESS;
	}
	return status;
}

void cvector_init(cvector* v)
{
	//initialize the capacity and allocate the memory
	v->capacity = CVECTOR_INIT_CAPACITY;
	v->size = 0;
	v->items = malloc(sizeof(void*) * v->capacity);
}