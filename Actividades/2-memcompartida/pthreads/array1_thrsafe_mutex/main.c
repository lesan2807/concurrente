#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "array.h"

#define MIN_CNT 10
#define MAX_CNT 20

typedef struct
{
   size_t id;
   size_t num_threads;
   size_t initial;
   size_t final;
   array_t* array_one;
   array_t* array_two;
}thread_info_t;

size_t initial_range(size_t n, size_t num_threads, size_t thread_id)
{
    size_t initial = 0;
    size_t division = n/num_threads;
    initial = thread_id*division;
    return initial;
}

size_t final_range(size_t n, size_t num_threads, size_t thread_id)
{
    size_t final = 0;
    size_t division = n/num_threads;
    final = (thread_id+1)*division;
    return final;
}

void print_array(const char* name, array_t* array)
{
    printf("%s =", name);
    for ( size_t index = 0; index < array_get_count(array); ++index )
        printf(" %zu", (size_t)array->array_elements[index]);
    putchar('\n');
    fflush(stdout);
}

void* functions(void* data)
{
    thread_info_t* info = (thread_info_t*)data;
    srand( (unsigned int)((unsigned long)time(NULL) + clock()) );
    for ( size_t index = info->initial; index < info->final; ++index )
    {
        array_append( info->array_one, (void*)(10 + (size_t)rand() % 90) );
        array_append( info->array_two, (void*)(100 + (size_t)rand() % 900) );
    }
    return NULL;
}

int main()
{
    size_t number_threads = sysconf( _SC_NPROCESSORS_ONLN);
    pthread_t threads[number_threads];
    thread_info_t* info = calloc(number_threads, sizeof(thread_info_t));

    array_t* array1 = (array_t*)malloc (MAX_CNT * sizeof(array_t) );// = array_create(array1, MAX_CNT);
    array_t* array2 = (array_t*)malloc (MAX_CNT * sizeof(array_t) );// = array_create(array2, MAX_CNT);
    array1 = array_create(array1, MAX_CNT);
    array2 = array_create(array2, MAX_CNT);

    for(size_t index = 0; index < number_threads; ++index)
    {
        info[index].array_one = array1;
        info[index].array_two = array2;
        info[index].id = index;
        info[index].final = final_range(MAX_CNT, number_threads, index);
        info[index].initial = initial_range(MAX_CNT, number_threads, index);
        info[index].num_threads = number_threads;
        pthread_create(&threads[index], NULL, functions, (void*)&info[index]);
    }

    for(size_t index = 0; index < number_threads; ++index)
        pthread_join(threads[index], NULL);

    print_array("array1", array1);
    print_array("array2", array2);

    array_destroy(array1);
    array_destroy(array2);

    free(array1);
    free(array2);
    free(info);

    return 0;
}
