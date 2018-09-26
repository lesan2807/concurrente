#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdbool.h>

/**
 * Record containing the result of analyzing arguments
 */
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
	/// True if output does not print elapsed time
	bool quiet;
	/// True if does not generate output at all
	bool silent;
} arguments_t;

/**
 * Initialize the given record with default values
 *
 * @param arguments Pointer to the record to be initialized
 * @return Pointer to the same arguments record
 */
arguments_t* arguments_init(arguments_t* arguments);

/**
 * Analyze the arguments given by user and returns a copy of a record
 * containing the results of the analysis.
 *
 * @param argc Argument count provided from the `main()` function.
 * @param argv Argument vector provided from the `main()` function.
 * @return A copy of a record containing the results of the analysis.
 */
arguments_t arguments_analyze(int argc, char* argv[]);

/**
 * @brief Print help about valid arguments in standard output
 * @return Success exit code (0)
 */
int arguments_print_usage();

/**
 * @brief Print version information about this software in standard output
 * @return Success exit code (0)
 */
int arguments_print_version();

#endif // ARGUMENTS_H
