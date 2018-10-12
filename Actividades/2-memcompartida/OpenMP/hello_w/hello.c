#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void hello(void)
{
	int thread_number = omp_get_thread_num();
	int thread_max = omp_get_num_threads();
	printf("hello from worker thread %d of %d\n", thread_number, thread_max);
}

int main(int argc, char* argv[])
{
	int number_threads = omp_get_max_threads();
	if( argc > 1 )
		number_threads = atoi(argv[1]);
	#pragma omp parallel num_threads(number_threads)
	hello();

	printf("Hello from main thread\n");
	return 0;
}
