#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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

void* calculate_levenshtein(void* data)
{
    thread_info_levdist_t* info = (thread_info_levdist_t*)data;
    for(size_t fila = info->initial_row_x; fila < info->final_row_x; ++fila)
    {
        for(size_t columna = 0; columna < *(info->columnas)+1; ++columna)
        {
            if( columna == 0)
            {
                info->matrix_x[fila][columna] = 0;
            }
            else if( info->target[columna-1] == (unsigned char)fila)
            {
                info->matrix_x[fila][columna] = columna;
            }
            else
                info->matrix_x[fila][columna] = info->matrix_x[fila][columna-1];
        }
    }

    for(size_t matrix_row = 0; matrix_row < *info->distance_row; ++matrix_row)
        {
            size_t current = matrix_row % 2;
            size_t prev = (matrix_row - 1) % 2;
            for(size_t column = info->initial_col_d; column < info->final_col_d; ++column)
            {
                if( matrix_row == 0)
                {
                    info->matrix_d[current][column] = column;
                   // printf("r1 ");

                }
                else if( column == 0)
                {
                    info->matrix_d[current][column] = matrix_row;
                    //printf("r2 ");
                }
                else if( info->target[matrix_row - 1] == info->source[matrix_row - 1])
                {
                    //printf("r3 ");
                    info->matrix_d[current][column] = info->matrix_d[prev][column - 1];
                }
                else if( info->matrix_x[info->source[matrix_row - 1]][column] == 0)
                {
                    //printf("r4 ");
                    info->matrix_d[current][column] = 1 +
                            MIN3(info->matrix_d[prev][column - 1],
                            info->matrix_d[prev][column],
                            matrix_row+column-1);
                }
                else
                {

                    if( column != info->initial_col_d)
                    {
                        info->matrix_d[current][column] = MIN3(
                                    info->matrix_d[prev][column],
                                    info->matrix_d[current][column - 1],
                                    info->matrix_d[prev][column - 1]);
                        ++(info->matrix_d[current][column]);
                        //printf("op ");
                    }
                    else
                    {

                        info->matrix_d[current][column] =
                                MIN3(
                                    info->matrix_d[prev][column],
                                    info->matrix_d[prev][column-1],
                                    info->matrix_d[prev][info->matrix_x[info->source[matrix_row-1]][column] - 1]
                                    + column - 1 - info->matrix_x[info->source[matrix_row-1]][column]);
                        ++info->matrix_d[current][column];
                        //printf("ow ");
                    }
                }
                // printf("(%u)  ", info->matrix_d[current][column]);
            }
            pthread_barrier_wait(info->barrier);
        }

    return NULL;
}

size_t levenshtein_ascii(unsigned char* source, unsigned char* target, size_t number_threads, size_t source_size, size_t target_size)
{
    unsigned char* pattern = source;
    unsigned char* text = target;
    size_t columns = 1;
    assert(source);
    assert(target);
    if( source_size > target_size )
    {
        unsigned char* temp = pattern;
        pattern = text;
        text = temp;
        size_t temp_size = source_size;
        target_size = source_size;
        source_size = temp_size;
        columns = source_size;
    }
    else
    {
        columns = target_size;
    }
    //printf("pattern: %zu text:%zu\n", source_size, target_size);
    size_t rows = 256;
    size_t** matrix = (size_t**)calloc(rows , 1+sizeof(size_t*));
    if(matrix == NULL)
        return printf("oups"), 1;
    for ( size_t row_index = 0; row_index < rows; ++row_index )
    {
        matrix[row_index] = (size_t*)calloc((columns+1) , sizeof(size_t));
        if(matrix[row_index] == NULL)
            return printf("oups"), 1;
    }

    size_t** matrix_lev = (size_t**)calloc( 2 , 1+sizeof(size_t*));
    if( matrix_lev == NULL)
        return printf("oups"), 1;
    for( size_t row_index = 0; row_index < 2; ++row_index )
    {
        matrix_lev[row_index] = (size_t*)calloc((columns+1) , sizeof( size_t));
        if(matrix_lev[row_index] == NULL)
            return printf("oups"), 1;
    }

    size_t actual_row = source_size;

    pthread_t threads_levdist[number_threads];
    thread_info_levdist_t* info_levdist = calloc(number_threads, sizeof(thread_info_levdist_t));
    pthread_barrier_t barriers;
    pthread_barrier_init(&barriers, NULL, (unsigned)number_threads);
    for(size_t index = 0; index < number_threads; ++index)
    {
        info_levdist[index].id = index;
        info_levdist[index].initial_row_x = initial_range(256, number_threads, index);
        info_levdist[index].final_row_x = final_range(256, number_threads, index);
        info_levdist[index].initial_col_d = initial_range(columns+1, number_threads, index);
        info_levdist[index].final_col_d = final_range(columns+1, number_threads, index);
        info_levdist[index].matrix_x = matrix;
        info_levdist[index].matrix_d = matrix_lev;
        info_levdist[index].distance_row = &actual_row;
        info_levdist[index].columnas = &columns;
        info_levdist[index].source = pattern;
        info_levdist[index].target = text;
        info_levdist[index].barrier = &barriers;
        pthread_create(&threads_levdist[index], NULL, calculate_levenshtein, (void*)&info_levdist[index]);
    }

    for(size_t index = 0; index < number_threads; ++index)
        pthread_join(threads_levdist[index], NULL);

    //debug_print_matrix(matrix_lev, 2, strlen((unsigned char*)target));

    free(info_levdist);

    for ( size_t row_index = 0; row_index < rows; ++row_index )
        free(matrix[row_index]);
    free(matrix);

    size_t distance = matrix_lev[(actual_row-1) % 2][columns];

    for ( size_t row_index = 0; row_index < 2; ++row_index )
        free(matrix_lev[row_index]);
    free(matrix_lev);
    //printf("distance:%u\n", distance-1);
    return distance-1;
}

size_t lev_dist_calculate_files_ascii(lev_dist_files_t* distances, size_t comparisons, size_t number_threads)
{
    for(size_t index = 0; index < comparisons; ++index)
        {
            FILE* source_file;
            FILE* target_file;
            unsigned char* source = NULL;
            unsigned char* target = NULL;
            struct stat file_info_source;
            struct stat file_info_target;
            // open files
            source_file = fopen( distances[index].file_source, "rb" );
            if( source_file == NULL)
                return fprintf(stderr, "levdist: error: could not open file: %s", distances[index].file_source), 1;
            target_file = fopen( distances[index].file_target, "rb" );
            if( target_file == NULL)
                return fprintf(stderr, "levdist: error: could not open file: %s", distances[index].file_target), 2;
            stat( distances[index].file_source, &file_info_source);
            stat( distances[index].file_target, &file_info_target);

            // allocate memory to contain the whole file;
            source = (unsigned char*) calloc(sizeof(unsigned char), (size_t)file_info_source.st_size+1);
            target = (unsigned char*) calloc(sizeof(unsigned char), (size_t)file_info_target.st_size+1);
            if(source == NULL || target == NULL)
                return fprintf(stderr, "levdist: error: could not allocate memory"), 3;

            fread( source, 1, (size_t)file_info_source.st_size, source_file);
            fread( target, 1, (size_t)file_info_target.st_size, target_file);
            source[file_info_source.st_size] = '\0';
            target[file_info_target.st_size] = '\0';
            // printf("1:%zu 2:%zu \n\n" ,strlen((char*)source), strlen((char*)target));
            distances[index].distance = 0;
            if( (size_t)file_info_source.st_size != 0 && (size_t)file_info_target.st_size != 0)
                distances[index].distance = levenshtein_ascii(source, target, number_threads, (size_t)file_info_source.st_size, (size_t)file_info_target.st_size);
            free(source);
            free(target);
            fclose(source_file);
            fclose(target_file);
        }
    return 0;
}



