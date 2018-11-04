#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

typedef struct
{
 void** array_elements;
 size_t array_capacity;
 size_t array_count;
}array_t;

static const size_t array_not_found = (size_t)-1;

array_t *array_create(array_t* array, size_t capacity);
void array_destroy(array_t* array);
int array_append(array_t* array, void* element);
size_t array_get_count(const array_t* array);
void* array_get_element(array_t* array, size_t index);
size_t array_find_first(const array_t* array, const void* element, size_t start_pos);
int array_remove_first(array_t* array, const void* element, size_t start_pos);

#endif // ARRAY_H
