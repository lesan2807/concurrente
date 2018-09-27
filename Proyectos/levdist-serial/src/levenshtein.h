#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H


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
    int distance;
    /// Name of source file
    const char* file_source;
    /// Name of target file
    const char* file_target;
} lev_dist_files_t;

/**
  @brief Calculate levenshtein distance between two strings.
  \ref https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C

  @param source Source string.
  @param target Target string.
  @return Integer of the distance found between two strings.
*/
int levenshtein(const char* source, const char* target);


#endif // LEVENSHTEIN_H
