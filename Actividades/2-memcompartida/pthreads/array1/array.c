#include <assert.h>
#include <stdlib.h>

#include "array.h"


array_t array_create(size_t capacity)
{
	assert(capacity);
	array_t temp = (array_t)malloc(sizeof(array_t)); 
	temp->array_capacity = capacity;
	temp->array_count = 0;
	temp->array_elements = (void**)malloc( capacity * sizeof(void*) );
	return temp; 
}

void array_destroy(array_t array)
{
	free(array->array_elements);
	free(array);
}

int array_increase_capacity(array_t array)
{
	size_t new_capacity = 10 * array->array_capacity;
	void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array->array_capacity = new_capacity;
	array->array_elements = new_elements;

	return 0; // Success
}

int array_decrease_capacity(array_t array)
{
	size_t new_capacity = array->array_capacity / 10;
	if ( new_capacity < 10 )
		return 0;

	void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
	if ( new_elements == NULL )
		return -1;

	array->array_capacity = new_capacity;
	array->array_elements = new_elements;

	return 0; // Success
}

size_t array_get_count(const array_t array)
{
	(void)array;
	return array->array_count;
}

void* array_get_element(array_t array, size_t index)
{
	assert( index < array_get_count(array) );
	return array->array_elements[index];
}

int array_append(array_t array, void* element)
{
	if ( array->array_count == array->array_capacity )
		if ( ! array_increase_capacity(array) )
			return -1;

	array->array_elements[array->array_count++] = element;
	return 0; // Success
}

size_t array_find_first(const array_t array, const void* element, size_t start_pos)
{
	for ( size_t index = start_pos; index < array->array_count; ++index )
		if ( array->array_elements[index] == element )
			return index;

	return array_not_found;
}

int array_remove_first(array_t array, const void* element, size_t start_pos)
{
	size_t index = array_find_first(array, element, start_pos);
	if ( index == array_not_found )
		return -1;

	for ( --array->array_count; index < array->array_count; ++index )
		array->array_elements[index] = array->array_elements[index + 1];
	if ( array->array_count == array->array_capacity / 10 )
		array_decrease_capacity(array);

	return 0; // Removed
}
