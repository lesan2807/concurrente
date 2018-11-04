#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "array.h"

array_t* array_create(array_t* array, size_t capacity)
{
	assert(capacity);
    array->array_capacity = capacity;
    array->array_count = 0;
    array->array_elements = (void**)malloc( capacity * sizeof(void*) );
    pthread_mutex_init(&array->lock, NULL);
    return array;
}

void array_destroy(array_t* array)
{
    free(array->array_elements);
    pthread_mutex_destroy(&array->lock);
}

int array_increase_capacity(array_t* array)
{
    pthread_mutex_lock(&array->lock);
    size_t new_capacity = 10 * array->array_capacity;
    void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
    if ( new_elements == NULL )
    {
        pthread_mutex_unlock(&array->lock);
        return -1;
    }

    array->array_capacity = new_capacity;
    array->array_elements = new_elements;
    pthread_mutex_unlock(&array->lock);
    return 0; // Success
}

int array_decrease_capacity(array_t* array)
{
    pthread_mutex_lock(&array->lock);
    size_t new_capacity = array->array_capacity / 10;
    if ( new_capacity < 10 )
    {
        pthread_mutex_unlock(&array->lock);
        return 0;
    }

    void** new_elements = (void**)realloc( array->array_elements, new_capacity * sizeof(void*) );
    if ( new_elements == NULL )
    {
        pthread_mutex_unlock(&array->lock);
        return -1;
    }

    array->array_capacity = new_capacity;
    array->array_elements = new_elements;
    pthread_mutex_unlock(&array->lock);
    return 0; // Success
}

size_t array_get_count(array_t* array)
{
    pthread_mutex_lock(&array->lock);
    size_t count = array->array_count;
    pthread_mutex_unlock(&array->lock);
    return count;
}

void* array_get_element(array_t* array, size_t index)
{
    assert( index < array_get_count(array) );
    pthread_mutex_lock(&array->lock);
    void* element = array->array_elements[index];
    pthread_mutex_unlock(&array->lock);
    return element;
}

int array_append(array_t* array, void* element)
{
    pthread_mutex_lock(&array->lock);
    if ( array->array_count == array->array_capacity )
        if ( ! array_increase_capacity(array) )
        {
            pthread_mutex_unlock(&array->lock);
            return -1;
        }

    array->array_elements[array->array_count] = element;
    ++array->array_count;
    pthread_mutex_unlock(&array->lock);
    return 0; // Success
}

size_t array_find_first( array_t* array, const void* element, size_t start_pos)
{
    pthread_mutex_lock(&array->lock);
    for ( size_t index = start_pos; index < array->array_count; ++index )
        if ( array->array_elements[index] == element )
        {
            pthread_mutex_unlock(&array->lock);
            return index;
        }
    pthread_mutex_unlock(&array->lock);
    return array_not_found;
}

int array_remove_first(array_t* array, const void* element, size_t start_pos)
{
    pthread_mutex_lock(&array->lock);
    size_t index = array_find_first(array, element, start_pos);
    if ( index == array_not_found )
    {
        pthread_mutex_unlock(&array->lock);
        return -1;
    }

    for ( --array->array_count; index < array->array_count; ++index )
        array->array_elements[index] = array->array_elements[index + 1];
    if ( array->array_count == array->array_capacity / 10 )
        array_decrease_capacity(array);
    pthread_mutex_unlock(&array->lock);
    return 0; // Removed
}
