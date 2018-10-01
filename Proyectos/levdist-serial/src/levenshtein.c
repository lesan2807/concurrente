#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <wchar.h>
#include <wctype.h>

#include "levenshtein.h"

// Choose the minimum of three numbers
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

size_t levenshtein_ascii(const char* source, const char* target)
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

size_t levenshtein_unicode(const wchar_t* source, const wchar_t* target)
{
    size_t x, y, source_length, target_length;
    source_length = wcslen(source);
    target_length = wcslen(target);

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

int lev_dist_calculate_files_ascii(lev_dist_files_t* distances, size_t comparisons)
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
            return fprintf(stderr, "levdist: error: could not open file: %s", distances[index].file_target), 2;
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

        distances[index].distance = levenshtein_ascii(source, target);
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
        if( source_file == NULL)
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
