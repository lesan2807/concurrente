#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "levenshtein.h"

// Choose the minimum of three numbers
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(const char* source, const char* target)
{
    unsigned int x, y, source_length, target_length;
    source_length = strlen(source);
    target_length = strlen(target);
    unsigned int matrix[target_length+1][source_length+1];
    matrix[0][0] = 0;
    for (x = 1; x <= target_length; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= source_length; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= target_length; x++)
        for (y = 1; y <= source_length; y++)
            matrix[x][y] = MIN3(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (source[y-1] == target[x-1] ? 0 : 1));

    return(matrix[target_length][source_length]);
}
