#include <mpi.h>
#include <omp.h>
#include <stdio.h> 
#include <stdlib.h>	

#define MIN(a,b) (((a)<=(b))?(a):(b))

size_t start(size_t a, size_t c, size_t i, size_t r) 
{
	return a + i*c + MIN(i,r); 
}

size_t finish(size_t a, size_t c, size_t i, size_t r) 
{
	return a + ((i+1)*c) + MIN((i+1),r); 
}

int main(int argc, char* argv[])
{
	int number_threads = omp_get_max_threads();

	MPI_Init(&argc, &argv);
	
	size_t a = strtoull(argv[1], NULL, 10); 
	size_t b = strtoull(argv[2], NULL, 10); 

	int my_rank = -1; 
	int process_count = -1;

	char hostname[MPI_MAX_PROCESSOR_NAME]; // forma de ir a la segura  
	int len_hostname = -1;  	

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Get_processor_name(hostname, &len_hostname); 
	
	size_t c = (b-a) / process_count;
	size_t r = (b-a) % process_count;
	
	size_t my_start = start(a, c, my_rank, r);
	size_t my_finish = finish(a, c, my_rank, r);
	
	size_t c1 = (my_finish-my_start) / number_threads;
	size_t r1 = (my_finish-my_start) % number_threads;
	
	printf("%s:%d range [%zu , %zu[ size %zu\n", hostname, my_rank, my_start , my_finish, my_finish-my_start);
	#pragma omp parallel num_threads(number_threads) default(none) shared(my_rank, hostname, c1, r1, a, my_start)
	{ 
		size_t inicio = start(my_start, c1, omp_get_thread_num(), r1);
		size_t final = finish(my_start, c1, omp_get_thread_num(), r1);  
		printf("\t%s:%d.%d: range [%zu , %zu[ size %zu\n", hostname, my_rank, omp_get_thread_num(), inicio, final, final-inicio);
	}

	MPI_Finalize(); 
	return 0; 
}
