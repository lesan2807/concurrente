#ifndef DIR_H
#define DIR_H

#include "queue.h"

/**
@brief Add file names in the content of the directory @ path to the given list
@param queue A queue of filenames
@param path Path to the directory to be read
@param recursive true if files of subdirectories should be added to the queue
@return The exit status, where 0 stands for success, otherwise for an error.
 */
int dir_list_files_in_dir(queue_t* queue, const char* path, bool recursive);

#endif // DIR_H
