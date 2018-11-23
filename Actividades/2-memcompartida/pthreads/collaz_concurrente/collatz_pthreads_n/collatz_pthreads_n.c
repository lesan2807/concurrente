#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct
{
	size_t id;
	size_t next;
	size_t prev;
	size_t size_numbers;
	size_t* current_step;
	const size_t* max_steps;
	size_t* numbers;
	pthread_barrier_t* barrier;

}thread_info_t;

int all_one(size_t* numbers, size_t size)
{
	int all_are_one = 1;
	for(size_t index = 0; index < size; ++index)
	{
		if(numbers[index] != 1)
			all_are_one = 0;
	}
	return all_are_one;
}

void* calculate(void* data)
{
	thread_info_t* my_info = (thread_info_t*)data;
	while( *(my_info->current_step) < *(my_info->max_steps) && !all_one(my_info->numbers, my_info->size_numbers))
	{
		size_t calcul = 1;
		if(my_info->numbers[my_info->id] % 2 == 0)
		 	calcul = my_info->numbers[my_info->id] / 2;
		else if( my_info->numbers[my_info->id] != 1)
			calcul = my_info->numbers[my_info->id] * my_info->numbers[my_info->prev] + my_info->numbers[my_info->next];

		pthread_barrier_wait(my_info->barrier); 
		my_info->numbers[my_info->id] = calcul;
		pthread_barrier_wait(my_info->barrier);
		
		if (my_info->id == 0)
			++*(my_info->current_step);
		else
			calcul = 1;
	}
	return NULL;
}


int main(int argc, char* argv[])
{
	size_t worker_count = sysconf(_SC_NPROCESSORS_ONLN);
	if(argc	> 1 )
        worker_count = strtoull(argv[1], NULL, 10);
	size_t number_count = strtoull(argv[2], NULL, 10); 
	size_t max = 10;

	size_t current = 0;
	size_t* numbers_array = calloc( number_count,  sizeof(size_t));
	for ( size_t index = 0; index < number_count; ++index )
		scanf("%zu", &numbers_array[index]);

	pthread_barrier_t barriers; 
	pthread_barrier_init(&barriers, NULL, (unsigned)worker_count);


	pthread_t threads[worker_count];
	thread_info_t* info = calloc(worker_count, sizeof(thread_info_t));
	for(size_t index = 0; index < worker_count; ++index)
	{
		info[index].id = index;
		info[index].prev = (index + number_count - 1) % number_count;
		info[index].next = (index + 1) % number_count;
		info[index].size_numbers = number_count;
		info[index].current_step = &current;
		info[index].max_steps = &max;
		info[index].numbers = numbers_array;
		info[index].barrier = &barriers;
		pthread_create(&threads[index], NULL, calculate, (void*)&info[index]);
	}

	for(size_t index =	0; index <	worker_count; ++index)
	{
		pthread_join(threads[index], NULL);
	}

	if ( !all_one(numbers_array, number_count) )
		printf("No converge in %zu steps", max);
	else
		printf("Converged in %zu steps", current);
	free(numbers_array);
	free(info);

	return 0;
}
