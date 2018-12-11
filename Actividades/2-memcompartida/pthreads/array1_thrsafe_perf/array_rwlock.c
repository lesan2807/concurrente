#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "array_rwlock.h"


array_rwlock_t array_rwlock_create(size_t capacity)
{
	assert(capacity);
	array_rwlock_t temp = malloc(sizeof(array_rwlock_struct)); 
	temp->array_capacity = capacity;
	temp->array_count = 0;
	temp->array_elements = malloc( capacity * sizeof(void*) );
	pthread_rwlock_init(&temp->lock, NULL);
	return temp; 
}

void array_rwlock_destroy(array_rwlock_t array)
{
	pthread_rwlock_destroy(&array->lock);
	free(array->array_elements);
	free(array);
}

int array_rwlock_increase_capacity(array_rwlock_t array)
{
	size_t new_capacity = 10 * array->array_capacity;
	void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;
	
	array->array_capacity = new_capacity;
	array->array_elements = new_elements;

	return 0; // Success
}

int array_rwlock_decrease_capacity(array_rwlock_t array)
{
	size_t new_capacity = array->array_capacity / 10;
	if ( new_capacity < 10 )
		return 0;
	void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;
	
	array->array_capacity = new_capacity;
	array->array_elements = new_elements;;
	return 0; // Success
}

size_t array_rwlock_get_count(const array_rwlock_t array)
{
	pthread_rwlock_rdlock(&array->lock);
	size_t count = 0;
	count = array->array_count;
	pthread_rwlock_unlock(&array->lock);
	return count; 
}

void* array_rwlock_get_element(array_rwlock_t array, size_t index)
{
	pthread_rwlock_rdlock(&array->lock);
	assert( index < array_rwlock_get_count(array) );
	void* element = array->array_elements[index];
	pthread_rwlock_unlock(&array->lock);
	return element;
}

int array_rwlock_append(array_rwlock_t array, void* element)
{
	pthread_rwlock_wrlock(&array->lock);
	if ( array->array_count == array->array_capacity )
		if ( ! array_rwlock_increase_capacity(array) )
		{
			pthread_rwlock_unlock(&array->lock);
			return -1;
		}

	array->array_elements[array->array_count++] = element;
	pthread_rwlock_unlock(&array->lock);
	return 0; // Success
}

size_t array_rwlock_find_first(const array_rwlock_t array, const void* element, size_t start_pos)
{
	pthread_rwlock_rdlock(&array->lock);
	for ( size_t index = start_pos; index < array->array_count; ++index )
		if ( array->array_elements[index] == element )
		{
			pthread_rwlock_unlock(&array->lock);
			return index;
		}
	pthread_rwlock_unlock(&array->lock);

	return array_not_found_rwlock;
}

int array_rwlock_remove_first(array_rwlock_t array, const void* element, size_t start_pos)
{
	size_t index = array_rwlock_find_first(array, element, start_pos);
	if ( index == array_not_found_rwlock )
		return -1;

	pthread_rwlock_wrlock(&array->lock);
	for ( --array->array_count; index < array->array_count; ++index )
		array->array_elements[index] = array->array_elements[index + 1];
	if ( array->array_count == array->array_capacity / 10 )
		array_rwlock_decrease_capacity(array);
	pthread_rwlock_unlock(&array->lock);
	return 0; // Removed
}
