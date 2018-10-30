#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H

#include <stddef.h>
#include <pthread.h>
#include "queue.h"

/** @file levenshtein.h

Implements the levenshtein distance algorithm for strings and for files.
It contains a structure for storing the levenshtein distance found between two files.

*/

/**
An opaque record that contains attributes for calculating the levenshtein distance between source file and target file.
*/
/// A record that stores the Levensthein distance between two files
typedef struct{
    /// Levenshtein distance
    size_t distance;
    /// Name of source file
    const char* file_source;
    /// Name of target file
    const char* file_target;
} lev_dist_files_t;

/**
An opaque record that contains attributes for calculating the matrix X of the parallel algorithm.
*/
typedef struct
{
    unsigned int initial_row;
    unsigned int final_row;
    unsigned int** matrix_x;
    unsigned char* target;
    unsigned char* alphabet;
    size_t* columnas;
}thread_info_matrix_t;

/**
An opaque record that contains attributes for calculating the matrix D of the parallel algorithm.
*/
typedef struct
{
    size_t id;
    size_t initial_col;
    size_t final_col;
    unsigned int** matrix_x;
    unsigned int** matrix_d;
    size_t* distance_row;
    size_t* columnas;
    unsigned char* source;
    unsigned char* target;
    unsigned char* alphabet;
    pthread_barrier_t* barrier;
}thread_info_levdist_t;

/**
  @brief Calculate levenshtein distance between two strings only ASCII.
  https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C

  @param source Source string.
  @param target Target string.
  @return Unsigned long long of the distance found between two strings.
*/
size_t levenshtein_ascii(unsigned char* source, unsigned char* target, size_t number_threads, size_t source_size, size_t target_size);

/**
  @brief Calculates levenshtein distance for all the files.
  Uses the method to calculate the levenstein distance between two strings only ACII.

  @param distances Array of records that have the distance, the name of the source file and the name of the target file.
  @param comparisons Number of comparisons needed, also size of array.
  @return Integer: for error handling.
 */
int lev_dist_calculate_files_ascii(lev_dist_files_t* distances, size_t comparisons, size_t number_threads);

/**
  @brief Opens file for comparison
  @param path is the path to open
  */

unsigned char* levenshtein_load_file(const char *path, size_t *size);

/**
  @brief Calculates levenshtein distance for all the files.
  Uses the method to calculate the levenstein distance between two strings with Unicode.

  @param distances Array of records that have the distance, the name of the source file and the name of the target file.
  @param comparisons Number of comparisons needed, also size of array.
  @return Integer: for error handling.
 */
int lev_dist_calculate_files_unicode(lev_dist_files_t* distances, size_t comparisons);


#endif // LEVENSHTEIN_H

