#ifndef ARRAY_MUTEX_H
#define ARRAY_MUTEX_H

#include <stddef.h>
#include <pthread.h>

typedef struct
{
	size_t array_count; 
	size_t array_capacity;
	void** array_elements;  
	pthread_mutex_t lock;
}array_mutex_struct; 

typedef array_mutex_struct* array_mutex_t; 

static const size_t array_not_found = (size_t)-1;

array_mutex_t array_mutex_create(size_t capacity);
void array_mutex_destroy(array_mutex_t array);
int array_mutex_append(array_mutex_t array, void* element);
size_t array_mutex_get_count(const array_mutex_t array);
void* array_mutex_get_element(array_mutex_t array, size_t index);
size_t array_mutex_find_first(const array_mutex_t array, const void* element, size_t start_pos);
int array_mutex_remove_first(array_mutex_t array, const void* element, size_t start_pos);

#endif // ARRAY_MUTEX_H
