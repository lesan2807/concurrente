#ifndef LEVDIST_H
#define LEVDIST_H

#include "arguments.h"
#include "queue.h"

typedef struct
{
	/// Information extracted from the command line arguments
	arguments_t arguments;
	/// Queue of files loaded from the directories
	queue_t* files;
} levdist_t;

/// Initializes a record with general information required by the controller
void levdist_init(levdist_t* this);

/// Start the execution of the command
int levdist_run(levdist_t* this, int argc, char* argv[]);

/// Finds all files in the directories given by arguments
int levdist_process_dirs(levdist_t* this, int argc, char* argv[]);

/// Add contents of the all directories in the arguments to the given list
int levdist_list_files_in_args(levdist_t* this, int argc, char* argv[]);


#endif // LEVDIST_H
