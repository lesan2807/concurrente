#include <assert.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <wchar.h>
#include <wctype.h>

#include "levenshtein.h"

// Choose the minimum of three numbers
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

//size_t levenshtein_ascii(const char* source, const char* target)
//{
//    size_t x, y, source_length, target_length;
//    source_length = strlen(source);
//    target_length = strlen(target);

//    // size_t matrix[target_length+1][source_length+1];
//    long** matrix = (long**)calloc(target_length+1, sizeof(long *));
//    for(size_t index = 0; index < target_length+1; ++index)
//        matrix[index] = (long*)calloc(source_length+1, sizeof(long));

//    matrix[0][0] = 0;
//    for (x = 1; x <= target_length; ++x)
//    {
//        matrix[x][0] = matrix[x-1][0] + 1;
//    }
//    for (y = 1; y <= source_length; ++y)
//        matrix[0][y] = matrix[0][y-1] + 1;
//    for (x = 1; x <= target_length; ++x)
//        for (y = 1; y <= source_length; ++y)
//            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (source[y-1] == target[x-1] ? 0 : 1));

//    size_t distance = matrix[target_length][source_length];
//    for ( size_t row_index = 0; row_index < target_length+1; ++row_index )
//         free(matrix[row_index]);
//    free(matrix);
//    return distance;
//}

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

size_t levenshtein_ascii(unsigned char* source, unsigned char* target, size_t workers)
{
    size_t len_source = strlen((char*)source);
    size_t len_target = strlen((char*)target);
    size_t distance = 0;
    size_t columns = 1;
    size_t rows_d = len_source;
    assert(source);
    assert(target);
    if( len_source > len_target )
    {
        unsigned char* temp = source;
        source = target;
        target = temp;
        size_t temp_size = len_source;
        len_source = len_target;
        len_target = temp_size;
        columns = len_source;
        rows_d = len_target;
    }
    else
    {
        columns = len_target;
        rows_d = len_source;
    }
    size_t rows = 256;
    size_t** matrix_x = (size_t**)calloc(rows , sizeof(size_t*));
    for ( size_t row_index = 0; row_index < rows; ++row_index )
    {
        matrix_x[row_index] = (size_t*)calloc((columns+1) , sizeof(size_t));
    }

    size_t** matrix_d = (size_t**)calloc( 2 , sizeof(size_t*));
    for( size_t row_index = 0; row_index < 2; ++row_index )
    {
        matrix_d[row_index] = (size_t*)calloc((columns+1) , sizeof( size_t));
    }

#pragma omp parallel for num_threads(workers) default(none) shared(rows, matrix_x, target, source, columns)
    for(size_t fila = 0; fila < rows; ++fila)
    {
        for(size_t columna = 0; columna < columns; ++columna)
        {
            if( columna == 0)
            {
                matrix_x[fila][columna] = 0;
            }
            else if( target[columna-1] == (unsigned char)fila)
            {
                matrix_x[fila][columna] = columna;
            }
            else
                matrix_x[fila][columna] = matrix_x[fila][columna-1];
        }
    }

    for(size_t matrix_row = 0; matrix_row < rows_d; ++matrix_row)
    {
        size_t current = matrix_row % 2;
        size_t prev = (matrix_row + 1) % 2;
#pragma omp parallel for num_threads(workers) default(none) shared(columns, matrix_d, source, target, current, prev, matrix_x, matrix_row)
        for(size_t column = 0; column < columns; ++column)
        {
            if( matrix_row == 0)
            {
                matrix_d[current][column] = column;
                //printf("j ");

            }
            else if( column == 0)
            {
                matrix_d[current][column] = matrix_row;
                //printf("i ");
            }
            else if( target[column - 1] == source[matrix_row - 1])
            {
                //printf("== ");
                matrix_d[current][column] = matrix_d[prev][column - 1];
            }
            else if( matrix_x[(int)source[matrix_row - 1]][column] == 0)
            {
                //printf("x ");
                matrix_d[current][column] = 1 +
                        MIN3(matrix_d[prev][column],
                             matrix_d[prev][column-1],
                        matrix_row+column-1);
            }
            else
            {/*
                        if( column != initial_col_d)
                        {
                            matrix_d[current][column] = MIN3(
                                        matrix_d[prev][column],
                                        matrix_d[current][column - 1],
                                        matrix_d[prev][column - 1]);
                            //printf("op ");
                        }
                        else*/
                {

                    matrix_d[current][column] = 1 +
                            MIN3( matrix_d[prev][column], matrix_d[prev][column-1], matrix_d[prev][matrix_x[(int)source[matrix_row-1]][column] -1 ] + column - 1 - matrix_x[(int)source[matrix_row-1]][column]);
                    //printf("w ");
                }
            }
            //printf("(%zu)  ", matrix_d[current][column]);
        }
#pragma omp barrier
        //printf("\n");
    }
    distance = matrix_d[(rows_d - 1) % 2][columns-1];
    return distance;
}

//size_t levenshtein_unicode(const wchar_t* source, const wchar_t* target)
//{
//    size_t x, y, source_length, target_length;
//    source_length = wcslen(source);
//    target_length = wcslen(target);

//    // size_t matrix[target_length+1][source_length+1];
//    long** matrix = (long**)calloc(target_length+1, sizeof(long *));
//    for(size_t index = 0; index < target_length+1; ++index)
//        matrix[index] = (long*)calloc(source_length+1, sizeof(long));

//    matrix[0][0] = 0;
//    for (x = 1; x <= target_length; ++x)
//    {
//        matrix[x][0] = matrix[x-1][0] + 1;
//    }
//    for (y = 1; y <= source_length; ++y)
//        matrix[0][y] = matrix[0][y-1] + 1;
//    for (x = 1; x <= target_length; ++x)
//        for (y = 1; y <= source_length; ++y)
//            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (source[y-1] == target[x-1] ? 0 : 1));

//    size_t distance = matrix[target_length][source_length];
//    for ( size_t row_index = 0; row_index < target_length+1; ++row_index )
//         free(matrix[row_index]);
//    free(matrix);
//    return distance;
//}

size_t levenshtein_unicode(const wchar_t* source, const wchar_t* target)
{
    size_t source_length, target_length, x, y, lastdiag, olddiag;
    source_length = wcslen(source);
    target_length = wcslen(target);
    size_t* column = malloc((source_length+1)*sizeof(size_t));
    for (y = 1; y <= source_length; ++y)
        column[y] = y;
    for (x = 1; x <= target_length; ++x) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= source_length; ++y) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (source[y-1] == target[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    size_t distance = column[source_length];
    free(column);
    return distance;
}


int lev_dist_calculate_files_ascii(lev_dist_files_t* distances, size_t comparisons, size_t workers)
{
    for(size_t index = 0; index < comparisons; ++index)
    {
        FILE* source_file;
        FILE* target_file;
        unsigned char* source;
        unsigned char* target;
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
        source = (unsigned char*) malloc(sizeof(unsigned char)*(size_t)file_info_source.st_size+1);
        target = (unsigned char*) malloc(sizeof(unsigned char)*(size_t)file_info_target.st_size+1);
        if(source == NULL || target == NULL)
            return fprintf(stderr, "levdist: error: could not allocate memory"), 3;

        fread( source, 1, file_info_source.st_size, source_file);
        fread( target, 1, file_info_target.st_size, target_file);
        source[file_info_source.st_size] = '\0';
        target[file_info_target.st_size] = '\0';

        distances[index].distance = 0;
        if( (size_t)file_info_source.st_size != 0 && (size_t)file_info_target.st_size != 0)
            distances[index].distance = levenshtein_ascii(source, target, workers);
        free(source);
        free(target);
        fclose(source_file);
        fclose(target_file);
    }
    return 0;
}

int lev_dist_calculate_files_unicode(lev_dist_files_t *distances, size_t comparisons)
{
    for(size_t index = 0; index < comparisons; ++index)
    {
        FILE* source_file;
        FILE* target_file;
        wchar_t* source;
        wchar_t* target;
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
        source = (wchar_t*) malloc(sizeof(wchar_t)*file_info_source.st_size+1);
        target = (wchar_t*) malloc(sizeof(wchar_t)*file_info_target.st_size+1);
        if(source == NULL || target == NULL)
            return fprintf(stderr, "levdist: error: could not allocate memory"), 3;

        fread( source, 1, file_info_source.st_size, source_file);
        fread( target, 1, file_info_target.st_size, target_file);
        source[file_info_source.st_size] = '\0';
        target[file_info_target.st_size] = '\0';

        distances[index].distance = levenshtein_unicode(source, target);
        free(source);
        free(target);
        fclose(source_file);
        fclose(target_file);
    }
    return 0;
}
