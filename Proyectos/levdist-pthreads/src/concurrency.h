#ifndef CONCURRENCY_H
#define CONCURRENCY_H

/**
@file concurrency.h

Provides common subroutines to implement concurrency, such as measure wall times
or detecting the amount of available CPUs in the system where the program is running.
*/

#include <time.h>

/**
 * Stores a point-in-time with nanosecond resolution
 */
typedef struct timespec walltime_t;

/**
@brief Stores current time in the given record.
@details This function is used to store the current wall time with
nanosecond resolution into a variable. The @a walltime_elapsed() can be used
to calculate the elapsed time after a time-consuming task is done.
@param start Pointer to the variable where the point-in-time will be stored.
@see walltime_elapsed()
*/
void walltime_start(walltime_t* start);

/**
@brief Returns time elapsed from @a start time to current time in seconds.
@param start Pointer to a point-in-time stored by the @a walltime_start() function.
@return The elapsed time in seconds with nanoseconds resolution (9 decimals).
@details This function is used to calculate the elapsed time in seconds from
a point-in-time stored in the variable pointed by @a start and the current
time. This calculation is done in cooperation with the @a walltime_start()
function. For example, to measure the elapsed time doing a task:

~~~~~~~~~~~~~~~{.c}
int main()
{
	// Start counting the time
	walltime_t start;
	walltime_start(&start);

	// Do some time-consuming task

	// Report elapsed time doing the task
	printf("Elapsed %.9lfs\n", walltime_elapsed(&start));
	return 0;
}
~~~~~~~~~~~~~~~
 */
double walltime_elapsed(const walltime_t* start);

/**
@brief Returns the number of CPUs available in the system.
@return The number of online CPUs, that is, it will not include CPUs that the
operating system may have shutted down.
 */
int concurrency_cpu_count();

#endif // CONCURRENCY_H
