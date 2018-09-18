#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "arguments.h"
#include "concurrency.h"

arguments_t* arguments_init(arguments_t* arguments)
{
	assert(arguments);

	arguments->error = 0;
	arguments->help_asked = false;
	arguments->version_asked = false;
	arguments->dir_count = 0;
	arguments->recursive = false;
	arguments->workers = concurrency_cpu_count();

	return arguments;
}

int arguments_print_version()
{
	printf("levdist v1.1 [2018-sep-14] AUTHOR1 <EMAIL1>, AUTHOR2 <EMAIL2>\n");
	printf("This is free software with no warranties. Use it at your own risk.\n");

	return 0;
}

int arguments_print_usage()
{
	printf("Usage: levdist [-hr][-w W] DIR1 [DIR2 ... DIR_N]\n");
	printf("Finds most similar files using Levenshtein distance\n");
	printf("\n");
	printf("Options:\n");
	printf("  -h,--help       Prints this help\n");
	printf("  -r,--recursive  Analyze files in subdirectories\n");
	printf("  -w W            Use W workers (threads)\n");

	return 0;
}

arguments_t arguments_analyze(int argc, char* argv[])
{
	arguments_t arguments;
	arguments_init(&arguments);

	for ( int current = 1; current < argc; ++current )
	{
		const char* arg = argv[current];

		// Check for an option
		if ( *arg == '-' )
		{
			// Check for a long --argument
			if ( arg[1] == '-' )
			{
				if ( strcmp(arg, "--help") == 0 )
					arguments.help_asked = true;
				if ( strcmp(arg, "--version") == 0 )
					arguments.version_asked = true;
			}
			else
			{
				// Short argument provided, e.g: -h
				switch ( arg[1] )
				{
					case 'h': arguments.help_asked = true; break;
					case 'r': arguments.recursive = true; break;

					case 'w':
						if ( current < argc - 1 )
						{
							char* str = argv[++current];
							int workers = 0;
							if ( sscanf(str, "%d", &workers ) == 1 && workers > 0 )
								arguments.workers = workers;
							else
							{
								fprintf(stderr, "levdist: error: invalid number of workers: %s\n", str);
								arguments.error = 2;
							}

							// Avoid considering this argument as a directory later
							*str = '-';
						}
						else
						{
							fprintf(stderr, "levdist: error: missing number of workers\n");
							arguments.error = 3;
						}
						break;

					// Unknown option
					default:
						fprintf(stderr, "levdist: error: unknown argument: %s\n", arg);
						arguments.error = 1;
						break;
				}
			}

		}
		else
		{
			// It is not an option, assume a directory
			++arguments.dir_count;
		}
	}

	return arguments;
}
