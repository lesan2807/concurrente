#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "array.h"

#define MIN_CNT 10
#define MAX_CNT 20

void print_array(const char* name, const array_t* array)
{
    printf("%s =", name);
    for ( size_t index = 0; index < array_get_count(array); ++index )
        printf(" %zu", (size_t)array->array_elements[index]);
    putchar('\n');
    fflush(stdout);
}

int main()
{
    array_t* array1 = (array_t*)malloc (MAX_CNT * sizeof(array_t) );// = array_create(array1, MAX_CNT);
    array_t* array2 = (array_t*)malloc (MAX_CNT * sizeof(array_t) );// = array_create(array2, MAX_CNT);
    array1 = array_create(array1, MAX_CNT);
    array2 = array_create(array2, MAX_CNT);

    srand( (unsigned int)((unsigned long)time(NULL) + clock()) );
    for ( size_t index = 0, count = MIN_CNT + rand() % MAX_CNT; index < count; ++index )
    {
        array_append( array1, (void*)(10 + (size_t)rand() % 90) );
        array_append( array2, (void*)(100 + (size_t)rand() % 900) );
    }

    print_array("array1", array1);
    print_array("array2", array2);

    array_destroy(array1);
    array_destroy(array2);

    free(array1);
    free(array2);

    return 0;
}
