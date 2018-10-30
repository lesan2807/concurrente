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

void debug_print_matrix(unsigned int** matrix, int rows, int cols)
{
    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols ;++col)
        {
            fprintf(stderr, "[%d]", matrix[row][col]);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "-------------------\n");
}

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

void* calculate_levdist(void* data)
{
    thread_info_levdist_t* info = (thread_info_levdist_t*)data;
    for(size_t matrix_row = 0; matrix_row < *info->distance_row; ++matrix_row)
    {
        for(size_t column = info->initial_col; column < info->final_col; ++column)
        {
            if( matrix_row == 0)
                info->matrix_d[matrix_row % 2][column] = column;
            else if( column == 0)
                info->matrix_d[matrix_row % 2][column] = matrix_row;
            else if( info->target[matrix_row - 1] == info->source[matrix_row - 1])
                info->matrix_d[matrix_row % 2][column] = info->matrix_d[(matrix_row - 1) % 2][column - 1];
            else if( info->matrix_x[info->source[matrix_row - 1]][column] == 0)
                info->matrix_d[matrix_row % 2][column] = 1 +
                        MIN3(info->matrix_d[(matrix_row -1 ) % 2][column - 1],
                        info->matrix_d[(matrix_row -1 ) % 2][column],
                        matrix_row+column-1);
            else
            {
                if( column != info->initial_col)
                    info->matrix_d[matrix_row % 2][column] = MIN3(info->matrix_d[(matrix_row -1 )% 2][column] + 1,
                            info->matrix_d[matrix_row % 2][column - 1] + 1,
                            info->matrix_d[(matrix_row - 1) % 2][column - 1] + (info->source[column - 1] == info->target[(matrix_row-1) % 2] ? 0 : 1));
                else
                    info->matrix_d[matrix_row % 2][column] = 1 +
                            MIN3(info->matrix_d[(matrix_row - 1) % 2][column],
                            info->matrix_d[(matrix_row - 1) % 2][column-1],
                            info->matrix_d[(matrix_row - 1) % 2][info->matrix_x[info->source[matrix_row-1]][column]]
                            + column - 1 - info->matrix_x[info->source[matrix_row-1]][column]);
            }
        }
        pthread_barrier_wait(info->barrier);
    }


    return NULL;
}

size_t levenshtein_ascii(unsigned char* source, unsigned char* target, size_t number_threads)
{
    pthread_t threads_matrix[number_threads];
    thread_info_matrix_t* info_matrix = calloc(number_threads, sizeof(thread_info_matrix_t));
    unsigned char ascii[256];
        for(unsigned int index = 0; index < 256; ++index)
            ascii[index] = index;
    size_t columns = 1;
    assert(source);
    assert(target);
    if( strlen((const char*)source) > strlen((const char*)target) )
    {
        columns = strlen((const char*)source);
        unsigned char* temp = source;
        source = target;
        target = temp;
    }
    else
    {
        columns = strlen((const char*)target);
    }
    size_t rows = 256;
    unsigned int** matrix = (unsigned int**)malloc(rows * sizeof(unsigned int*));
    if(matrix == NULL)
        return printf("oups"), 1;
    for ( size_t row_index = 0; row_index < rows; ++row_index )
    {
        matrix[row_index] = (unsigned int*)malloc((columns+1) * sizeof(unsigned int));
        if(matrix[row_index] == NULL)
            return printf("oups"), 1;
    }

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

    unsigned int** matrix_lev = (unsigned int**)malloc( 2 * sizeof(unsigned int*));
    if( matrix_lev == NULL)
        return printf("oups"), 1;
    for( size_t row_index = 0; row_index < 2; ++row_index )
    {
        matrix_lev[row_index] = (unsigned int*)malloc((columns+1) * sizeof( unsigned int));
        if(matrix_lev[row_index] == NULL)
            return printf("oups"), 1;
    }

    size_t actual_row = strlen((char*)source);

    pthread_t threads_levdist[number_threads];
    thread_info_levdist_t* info_levdist = calloc(number_threads, sizeof(thread_info_levdist_t));
    pthread_barrier_t barriers;
    pthread_barrier_init(&barriers, NULL, (unsigned)number_threads);
    for(size_t index = 0; index < number_threads; ++index)
    {
        info_levdist[index].id = index;
        info_levdist[index].initial_col = initial_range(columns+1, number_threads, index);
        info_levdist[index].final_col = final_range(columns+1, number_threads, index);
        info_levdist[index].matrix_x = matrix;
        info_levdist[index].matrix_d = matrix_lev;
        info_levdist[index].distance_row = &actual_row;
        info_levdist[index].columnas = &columns;
        info_levdist[index].source = source;
        info_levdist[index].target = target;
        info_levdist[index].alphabet = ascii;
        info_levdist[index].barrier = &barriers;
        pthread_create(&threads_levdist[index], NULL, calculate_levdist, (void*)&info_levdist[index]);
    }

    for(size_t index = 0; index < number_threads; ++index)
        pthread_join(threads_levdist[index], NULL);

    //debug_print_matrix(matrix_lev, 2, strlen((unsigned char*)target));

    free(info_matrix);
    free(info_levdist);

    for ( size_t row_index = 0; row_index < rows; ++row_index )
        free(matrix[row_index]);
    free(matrix);
    return matrix_lev[(actual_row - 1)  % 2][columns-1];
}

unsigned char* levenshtein_load_file(const char *path)
{
    assert(path);
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
    assert(buffer);
    return buffer;
}

int lev_dist_calculate_files_ascii(lev_dist_files_t* distances, size_t comparisons, size_t number_threads)
{
    unsigned char* source = NULL;
    unsigned char* target = NULL;
    for(size_t index = 0; index < comparisons; ++index)
    {
        source = levenshtein_load_file(distances[index].file_source);
        target = levenshtein_load_file(distances[index].file_target);;
        distances[index].distance = levenshtein_ascii(source, target, number_threads);
        assert(source);
        assert(target);
        free(source);
        free(target);
    }
    return 0;
}


