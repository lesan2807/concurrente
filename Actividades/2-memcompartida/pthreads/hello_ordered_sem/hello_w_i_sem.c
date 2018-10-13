#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef	struct
{
    size_t number;
    size_t count;
    sem_t* semaphores;

}thread_info_t;

void* hello(void* data)
{
	thread_info_t* my_info = (thread_info_t*)data;
	sem_wait(&my_info->semaphores[my_info->count]);
	printf("Hello world from thread	%zu	of %zu\n",	my_info->count,	my_info->number-1);
	sem_post(&my_info->semaphores[(my_info->count+1) % my_info->number]);
	return NULL;
}

int	main(int argc, char* argv[])
{
	size_t number_threads =	sysconf(_SC_NPROCESSORS_ONLN);
	if(argc	> 1 )
        number_threads = strtoull(argv[1], NULL, 10);
	pthread_t threads[number_threads];
	thread_info_t* info	= calloc(number_threads, sizeof(thread_info_t));
	sem_t semaphore[number_threads];
	sem_init(&semaphore[0], 0, 1);
	for(size_t index = 1; index	< number_threads; ++index)
	{
		sem_init(&semaphore[index],	0,	0);
	}
	for(size_t index = 0; index	<number_threads;	++index)
	{
		info[index].number = number_threads;
		info[index].count = index;
		info[index].semaphores = semaphore;
		pthread_create(&threads[index],	NULL, hello, (void*)&info[index]);
	}

	printf("Hello from	main thread\n");

	for(size_t index =	0;	index <	number_threads;	++index)
	{
		pthread_join(threads[index], NULL);
	}

	free(info);
	return	0;
}
