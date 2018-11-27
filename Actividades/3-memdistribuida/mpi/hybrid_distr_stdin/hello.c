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
	
	size_t a = 0;
	size_t b = 0; 
	if( argc >= 2)
	{
		a = strtoull(argv[1], NULL, 10); 
		b = strtoull(argv[2], NULL, 10);
	}
	else 
	{
		scanf("%zu %zu", &a, &b); 
	}
	
	char message[1024];


 
	size_t c = (b-a) / number_threads; 
	size_t r = (b-a) % number_threads; 

	int my_rank = -1; 
	int process_count = -1;

	char hostname[MPI_MAX_PROCESSOR_NAME]; // forma de ir a la segura  
	int len_hostname = -1;  	

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Get_processor_name(hostname, &len_hostname); 
	
	if( my_rank == 0) 
	{
		for( int recieve = 1; recieve < process_count; ++recieve)
		{
			sprintf(message, "%zu %zu", a, b);  
			MPI_Send(message, 1024, MPI_CHAR, recieve, 0, MPI_COMM_WORLD);
		}
	}
	else 
	{
		MPI_Recv(message, 1024, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}	

	size_t my_start = start(a, c, my_rank, r);
	size_t my_finish = finish(a, c, my_rank, r);
	
	printf("%s: range [%zu , %zu[ size %zu\n", hostname, my_start , my_finish, my_finish-my_start);
	#pragma omp parallel for num_threads(number_threads) default(none) shared (a, c, r, hostname, number_threads)
	for (long long index = 0; index < number_threads; ++index)
	{
		size_t inicio = start(a, c, omp_get_thread_num(), r);
		size_t final =  finish(a, c, omp_get_thread_num(), r);
		printf("\t %s:%d: range [%zu , %zu[ size %zu\n", hostname, omp_get_thread_num(), inicio, final, final-inicio); 
	}
	
	MPI_Finalize(); 
	return 0; 
}
