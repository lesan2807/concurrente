#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>

/// Record containing the result of analyzing arguments
typedef struct
{
	/// True if arguments do not have errors
	bool error;
	/// True if user asked for help
	bool help_asked;
	/// True if user asked for software version
	bool version_asked;
	/// Number of directories given by user
	int dir_count;
	/// True if files should be found recursively
	bool recursive;
	/// Number of workers (threads) to use
	int workers;
} arguments_t;

/// Initialize the given record with default values
arguments_t* arguments_init(arguments_t* arguments);

/// Analyze the arguments given by user and returns a record with the result
arguments_t arguments_analyze(int argc, char* argv[]);

/// Print help about valid arguments
int arguments_print_usage();

/// Print version information about this software
int arguments_print_version();

#endif // ARGUMENTS_H
