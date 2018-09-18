#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <time.h>

/// Stores a point-in-time with nanosecond resolution
typedef struct timespec walltime_t;

/// Stores current time in the given record
void walltime_start(walltime_t* start);

/// Returns time elapsed from @a start time to current time in seconds
double walltime_elapsed(const walltime_t* start);


/// Returns the number of CPU available in the system
int concurrency_cpu_count();

#endif // CONCURRENCY_H
