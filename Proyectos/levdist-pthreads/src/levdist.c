#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "concurrency.h"
#include "dir.h"
#include "levdist.h"
#include "levenshtein.h"

// Private functions:

/// Shows how to travese the list removing elements
void levdist_print_and_destroy_files(levdist_t* this);

/// Shows how to traverse the list without removing elements
void levdist_print_files(levdist_t* this);

/// Makes sure that files are different
int levdist_check_different_files(queue_t* queue);

/// Prints all the distances with the files.
void levdist_print_distances(levdist_t* this, size_t comparisons);

/// Orders the array of distances
void levdist_order_lev_dist_t(lev_dist_files_t* distances, size_t comparisons);

/// Orders one register by files
void levdist_order_file_source_target(lev_dist_files_t *one);

/// Call-back function for ordering distances
int less_than_distance(const void* first, const void* second);

/// Call-back function for ordering source files
int less_than_files_source(const void* first, const void* second);

/// Call-back function for ordering target files
int less_than_files_target(const void* first, const void* second);

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
		return fprintf(stderr, "levdist: error: no directories or files given\n"), 1;

	// Arguments seem fine, process the directories or files
	return levdist_process_dirs(this, argc, argv);
}

int levdist_process_dirs(levdist_t* this, int argc, char* argv[])
{
    // If -u is an argument, set locale
    if( this->arguments.unicode )
    {
        setlocale(LC_ALL, "");
    }
	// Start counting the time
	walltime_t start_total;
	walltime_t start_comparisons;
	walltime_start(&start_total);

	// Load all files into a list
	this->files = queue_create();
	levdist_list_files_in_args(this, argc, argv);

    // How many comparisons need to be done. (Gauss formula)
    size_t comparisons = queue_count(this->files)*(queue_count(this->files)-1)/2;

    // If only one file or the same file:
    if( comparisons == 0 || levdist_check_different_files(this->files))
    {
        queue_destroy(this->files, true);
        return fprintf(stderr, "levdist: error: at least two files are required to compare\n"), 2;
    }
    // Create the array
    this->distances = malloc(comparisons*sizeof(lev_dist_files_t));

    // Fill the array of records for each file
    distances_init(this);

	walltime_start(&start_comparisons);
    // Calculate levenshtein distance for all files.
    if( this->arguments.unicode )
        lev_dist_calculate_files_unicode(this->distances, comparisons);
    else
        lev_dist_calculate_files_ascii(this->distances, comparisons);

    // Order array
    levdist_order_lev_dist_t(this->distances, comparisons);
    qsort(this->distances, comparisons, sizeof(lev_dist_files_t), less_than_files_target);
    qsort(this->distances, comparisons, sizeof(lev_dist_files_t), less_than_files_source);
    qsort(this->distances, comparisons, sizeof(lev_dist_files_t), less_than_distance);


    // Print filenames found when -Q is provided as an argument.
    if( !this->arguments.silent)
    {
        // fprintf(stderr, "comparisons: %zu\n", comparisons);
        // levdist_print_files(this);
        levdist_print_distances(this, comparisons);
    }

    //array_destroy
    free(this->distances);

	// Report elapsed time when -q is not written on arguments.
	if( !this->arguments.quiet )
		printf("Total time %.9lfs, comparing time %.9lfs, with %d workers\n", walltime_elapsed(&start_total), walltime_elapsed(&start_comparisons), this->arguments.workers);

    queue_destroy(this->files, true);
	return 0;
}

int levdist_list_files_in_args(levdist_t* this, int argc, char* argv[])
{
	struct stat file_info;
	// Traverse all arguments
	for ( int current = 1; current < argc; ++current )
	{
		// Skip command-line options
		const char* arg = argv[current];
		if ( *arg == '-' )
			continue;
		stat( arg, &file_info);
		if( S_ISDIR(file_info.st_mode) )
		{
			dir_list_files_in_dir(this->files, arg, this->arguments.recursive);
		}
		else
		{
			queue_append(this->files, strdup(arg));
		}
	}
	return 0;
}

void distances_init(levdist_t* this)
{
    lev_dist_files_t file_info;
    size_t index = 0;
    for(queue_iterator_t itr_source = queue_begin(this->files); itr_source != queue_end(this->files); itr_source = queue_next(itr_source))
    {
        file_info.file_source = (const char*)queue_data(itr_source);
        for(queue_iterator_t itr_target = queue_next(itr_source); itr_target != queue_end(this->files); itr_target = queue_next(itr_target))
        {
            file_info.file_target = (const char*)queue_data(itr_target);
            this->distances[index] = file_info;
            ++index;
        }
    }
}

void levdist_print_and_destroy_files(levdist_t* this)
{
	long count = 0;
	while ( ! queue_is_empty(this->files) )
	{
		char* filename = (char*)queue_pop(this->files);
		printf("%ld: %s\n", ++count, filename);
		free(filename);
	}
}

void levdist_print_files(levdist_t* this)
{
	long count = 0;
	for ( queue_iterator_t itr = queue_begin(this->files); itr != queue_end(this->files); itr = queue_next(itr) )
	{
		const char* filename = (const char*)queue_data(itr);
		printf("%ld: %s\n", ++count, filename);
	}
}

void levdist_print_distances(levdist_t* this, size_t comparisons)
{
    for(size_t index = 0; index < comparisons; ++index)
    {
        printf("%zu\t%s\t%s\n", this->distances[index].distance, this->distances[index].file_source, this->distances[index].file_target);
    }
}

int levdist_check_different_files(queue_t* queue)
{
    int different = 0;
    for(queue_iterator_t itr_source = queue_begin(queue); itr_source != queue_end(queue); itr_source = queue_next(itr_source))
    {
        const char* file_source = (const char*)queue_data(itr_source);
        for(queue_iterator_t itr_target = queue_next(itr_source); itr_target != queue_end(queue); itr_target = queue_next(itr_target))
        {
            const char* file_target = (const char*)queue_data(itr_target);
            if( strcmp(file_source, file_target) == 0)
                different = 1;
        }
    }
    return different;
}

void levdist_order_lev_dist_t(lev_dist_files_t* distances, size_t comparisons)
{
    for(size_t index = 0; index < comparisons; ++index)
        levdist_order_file_source_target(&distances[index]);
}

void levdist_order_file_source_target(lev_dist_files_t* one)
{
    //int size = strlen(one.file_source) > strlen(one.file_target) ? strlen(one.file_source) : strlen(one.file_target);
    if( strcmp( one->file_source, one->file_target) > 0)
    {
        const char* temp = one->file_source;
        one->file_source = one->file_target;
        one->file_target = temp;
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
