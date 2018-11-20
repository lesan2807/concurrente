#include <mpi.h>
#include <omp.h>
#include <stdio.h> 

void hello(int rank, char* hostname)
{
	int thread_number = omp_get_thread_num();
	int thread_max = omp_get_num_threads();
	printf("\thello from worker thread %d of %d of process %d on %s\n", thread_number, thread_max, rank, hostname);
}

int main(int argc, char* argv[])
{
	int number_threads = omp_get_max_threads();
	MPI_Init(&argc, &argv);

	int my_rank = -1; 
	int process_count = -1;

	char hostname[MPI_MAX_PROCESSOR_NAME]; // forma de ir a la segura  
	int len_hostname = -1;  	

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Get_processor_name(hostname, &len_hostname); 
  
	printf("Hello from process %d of %d in %s\n\n", my_rank, process_count, hostname);
	#pragma omp parallel num_threads(number_threads)
	hello(my_rank, hostname); 
	
	MPI_Finalize(); 
	return 0; 
}
