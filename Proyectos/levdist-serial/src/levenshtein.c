#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "levenshtein.h"




// Choose the minimum of three numbers
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

size_t levenshtein(const char* source, const char* target)
{

    size_t x, y, source_length, target_length;
    source_length = strlen(source);
    target_length = strlen(target);

    // size_t matrix[target_length+1][source_length+1];
    long** matrix = (long**)calloc(target_length+1, sizeof(long *));
    for(size_t index = 0; index < target_length+1; ++index)
        matrix[index] = (long*)calloc(source_length+1, sizeof(long));

    matrix[0][0] = 0;
    for (x = 1; x <= target_length; ++x)
    {
        matrix[x][0] = matrix[x-1][0] + 1;
    }
    for (y = 1; y <= source_length; ++y)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= target_length; ++x)
        for (y = 1; y <= source_length; ++y)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (source[y-1] == target[x-1] ? 0 : 1));

    size_t distance = matrix[target_length][source_length];
    for ( size_t row_index = 0; row_index < target_length+1; ++row_index )
         free(matrix[row_index]);
    free(matrix);
    return distance;
}

int lev_dist_calculate_files(lev_dist_files_t* distances, size_t comparisons)
{
    for(size_t index = 0; index < comparisons; ++index)
    {
        FILE* source_file;
        FILE* target_file;
        char* source;
        char* target;
        struct stat file_info_source;
        struct stat file_info_target;

        // open files
        source_file = fopen( distances[index].file_source, "rb" );
        if( source_file == NULL)
            return fprintf(stderr, "levdist: error: could not open file: %s", distances[index].file_source), 1;
        target_file = fopen( distances[index].file_target, "rb" );
        if( source_file == NULL)
            return fprintf(stderr, "levdist: error: could not open file: %s", distances[index].file_target), 2;;
        stat( distances[index].file_source, &file_info_source);
        stat( distances[index].file_target, &file_info_target);

        // allocate memory to contain the whole file;
        source = (char*) malloc(sizeof(char)*file_info_source.st_size+1);
        target = (char*) malloc(sizeof(char)*file_info_target.st_size+1);
        if(source == NULL || target == NULL)
            return fprintf(stderr, "levdist: error: could not allocate memory"), 3;

        fread( source, 1, file_info_source.st_size, source_file);
        fread( target, 1, file_info_target.st_size, target_file);
        source[file_info_source.st_size] = '\0';
        target[file_info_target.st_size] = '\0';

        distances[index].distance = levenshtein(source, target);
        free(source);
        free(target);
        fclose(source_file);
        fclose(target_file);
    }
    return 0;
}

void distances_init(lev_dist_files_t* files, queue_t* queue)
{
    lev_dist_files_t file_info;
    size_t index = 0;
    for(queue_iterator_t itr_source = queue_begin(queue); itr_source != queue_end(queue); itr_source = queue_next(itr_source))
    {
        file_info.file_source = (const char*)queue_data(itr_source);
        for(queue_iterator_t itr_target = queue_next(itr_source); itr_target != queue_end(queue); itr_target = queue_next(itr_target))
        {
            file_info.file_target = (const char*)queue_data(itr_target);
            files[index] = file_info;
            ++index;
        }
    }
}

int less_than_distance(const void* first, const void* second)
{
    lev_dist_files_t one = *(lev_dist_files_t*)first;
    lev_dist_files_t two = *(lev_dist_files_t*)second;
    if ( one.distance <  two.distance ) return -1;
    if ( one.distance ==  two.distance ) return 0;
    if ( one.distance >  two.distance ) return 1;
    return 0;
}



void order_files(lev_dist_files_t* one)
{
    //int size = strlen(one.file_source) > strlen(one.file_target) ? strlen(one.file_source) : strlen(one.file_target);
    if( strcmp( one->file_source, one->file_target) > 0)
    {
        const char* temp = one->file_source;
        one->file_source = one->file_target;
        one->file_target = temp;
    }
}

int less_than_files_source(const void *first, const void *second)
{
    lev_dist_files_t one = *(lev_dist_files_t*)first;
    lev_dist_files_t two = *(lev_dist_files_t*)second;
    if ( strcmp(one.file_source, two.file_source) < 0 ) return -1;
    if ( strcmp(one.file_source, two.file_source) == 0 ) return 0;
    if ( strcmp(one.file_source, two.file_source) > 0 ) return 1;
    return 0;
}

int less_than_files_target(const void *first, const void *second)
{
    lev_dist_files_t one = *(lev_dist_files_t*)first;
    lev_dist_files_t two = *(lev_dist_files_t*)second;
    if ( strcmp(one.file_target, two.file_target) < 0 ) return -1;
    if ( strcmp(one.file_target, two.file_target) == 0 ) return 0;
    if ( strcmp(one.file_target, two.file_target) > 0 ) return 1;
    return 0;
}
