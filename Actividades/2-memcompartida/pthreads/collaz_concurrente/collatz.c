#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


static size_t worker_count = 3;
static size_t number_count = 3;
static size_t* numbers = NULL;
static size_t current_step = 0;
static size_t max_steps = 10;
static pthread_t* workers = NULL;
static pthread_barrier_t barrier;

void* calculate(void* data)
{
	const size_t my_id = (size_t)data;
	const size_t next = (my_id + 1) % worker_count;
	const size_t prev = (size_t)((long long)my_id - 1) % worker_count;

	while ( current_step < max_steps && numbers[my_id] > 1 )
	{
		if ( numbers[my_id] % 2 == 0 )
			numbers[my_id] /= 2;
		else
			numbers[my_id] = numbers[prev] * numbers[my_id] + numbers[next];

		pthread_barrier_wait(&barrier);

		if ( my_id == 0 )
			++current_step;
	}
	return NULL;
}

int main()
{
	//scanf("%zu", &number_count);
	numbers = malloc( number_count * sizeof(size_t) );
	for ( size_t index = 0; index < number_count; ++index )
		scanf("%zu", &numbers[index]);

	pthread_barrier_init(&barrier, NULL, (unsigned)worker_count);

	workers = malloc(worker_count * sizeof(pthread_t));
	for ( size_t index = 0; index < worker_count; ++index )
		pthread_create(&workers[index], NULL, calculate, (void*)index);

	for ( size_t index = 0; index < worker_count; ++index )
		pthread_join(workers[index], NULL);

	pthread_barrier_destroy(&barrier);

	if ( current_step > max_steps )
		printf("No converge in %zu steps", max_steps);
	else
		printf("Converged in %zu steps", current_step);

	return 0;
}