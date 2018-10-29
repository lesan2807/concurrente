#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <wchar.h>
#include <wctype.h>

#include "levenshtein.h"

// Choose the minimum of three numbers
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

size_t initial_range(size_t rows, size_t num_threads, size_t thread_id)
{
    size_t initial = 0;
    size_t division = rows/num_threads;
    initial = thread_id*division;
    return initial;
}

size_t final_range(size_t rows, size_t num_threads, size_t thread_id)
{
    size_t final = 0;
    size_t division = rows/num_threads;
    final = (thread_id+1)*division;
    if(thread_id == num_threads-1)
        final = rows;
    return final;
}

typedef struct
{
    unsigned int initial_row;
    unsigned int final_row;
    unsigned int** matrix_x;
    unsigned char* target;
    int* alphabet;
    size_t* columnas;
}thread_info_matrix_t;

void* fill_matrix(void* data)
{
    thread_info_matrix_t* info = (thread_info_matrix_t*)data;
    for(unsigned int fila = info->initial_row; fila < info->final_row; ++fila)
    {
        info->matrix_x[fila][0] = 0;
        for(size_t columna = 1; columna < *info->columnas+1; ++columna)
        {

            if( info->target[columna-1] == info->alphabet[fila])
            {
                info->matrix_x[fila][columna] = columna;
            }
            else
                info->matrix_x[fila][columna] = info->matrix_x[fila][columna-1];
        }
    }
    return NULL;
}

size_t levenshtein_ascii(unsigned char* source, unsigned char* target, size_t number_threads)
{
    pthread_t threads_matrix[number_threads];
    thread_info_matrix_t* info_matrix = calloc(number_threads, sizeof(thread_info_matrix_t));
    int ascii[256];
        for(int index = 0; index < 256; ++index)
            ascii[index] = index;
    size_t columns = 1;
    if( strlen((char*)source) > strlen((char*)target) )
    {
        columns = strlen((char*)source);
        unsigned char* temp = source;
        source = target;
        target = temp;
    }
    else
    {
        columns = strlen((char*)target);
    }
    size_t rows = 256;
    unsigned int** matrix = (unsigned int**)malloc(rows * sizeof(unsigned int*));
    for ( size_t row_index = 0; row_index < rows; ++row_index )
        matrix[row_index] = (unsigned int*)malloc((columns+1) * sizeof(unsigned int));

    for (size_t index = 0; index < number_threads; ++index)
    {
        info_matrix[index].initial_row = initial_range(256, number_threads, index);
        info_matrix[index].final_row = final_range(256, number_threads, index);
        info_matrix[index].matrix_x = matrix;
        info_matrix[index].target = target;
        info_matrix[index].alphabet = ascii;
        info_matrix[index].columnas = &columns;
        pthread_create(&threads_matrix[index], NULL, fill_matrix, (void*)&info_matrix[index]);
    }

    for(size_t index = 0; index < number_threads; ++index)
        pthread_join(threads_matrix[index], NULL);

    return 0;
}

unsigned char* levenshtein_load_file(const char *path)
{
    unsigned char* buffer = NULL;
    size_t length = 0;
    FILE* file = fopen(path, "rb");
    if(file)
    {
        fseek( file, 0, SEEK_END);
        length = ftell(file);
        fseek( file, 0, SEEK_SET);
        buffer = (unsigned char*)malloc(length+1);
        if(buffer)
        {
            fread(buffer, 1, length, file);
            buffer[length] = '\0';
        }
        fclose(file);
    }
    return buffer;
}

int lev_dist_calculate_files_ascii(lev_dist_files_t* distances, size_t comparisons, size_t number_threads)
{
    for(size_t index = 0; index < comparisons; ++index)
    {
        unsigned char* source = levenshtein_load_file(distances[index].file_source);
        unsigned char* target = levenshtein_load_file(distances[index].file_source);;
        distances[index].distance = levenshtein_ascii(source, target, number_threads);
        free(source);
        free(target);
    }
    return 0;
}


