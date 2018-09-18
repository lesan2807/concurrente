#include <stdio.h>
#include <stdlib.h>

#include "concurrency.h"
#include "dir.h"
#include "levdist.h"

void levdist_init(levdist_t* this)
{
	arguments_init(&this->arguments);
	this->files = NULL;
}

int levdist_run(levdist_t* this, int argc, char* argv[])
{
	// Analyze the arguments given by the user
	this->arguments = arguments_analyze(argc, argv);

	// If arguments are incorrect, stop
	if ( this->arguments.error )
		return this->arguments.error;

	// If user asked for help or software version, print it and stop
	if ( this->arguments.help_asked )
		return arguments_print_usage();
	if ( this->arguments.version_asked )
		return arguments_print_version();

	// If user did not provided directories, stop
	if ( this->arguments.dir_count <= 0 )
		return fprintf(stderr, "levdist: error: no directories given\n"), 1;

	// Arguments seems fine, process the directories
	return levdist_process_dirs(this, argc, argv);
}

int levdist_process_dirs(levdist_t* this, int argc, char* argv[])
{
	// Start counting the time
	walltime_t start;
	walltime_start(&start);

	// Load all files into a list
	this->files = queue_create();
	levdist_list_files_in_args(this, argc, argv);

	// Print filenames found
	long count = 0;
	while ( ! queue_is_empty(this->files) )
	{
		char* filename = (char*)queue_pop(this->files);
		printf("%ld: %s\n", ++count, filename);
		free(filename);
	}
	queue_destroy(this->files, true);

	// Report elapsed time
	printf("Elapsed %.9lfs with %d workers\n", walltime_elapsed(&start), this->arguments.workers);

	return 0;
}

int levdist_list_files_in_args(levdist_t* this, int argc, char* argv[])
{
	// Traverse all arguments
	for ( int current = 1; current < argc; ++current )
	{
		// Skip command-line options
		const char* arg = argv[current];
		if ( *arg == '-' )
			continue;

		dir_list_files_in_dir(this->files, arg, this->arguments.recursive);
	}

	return 0;
}
