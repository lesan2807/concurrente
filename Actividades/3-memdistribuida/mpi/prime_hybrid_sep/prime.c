#include <mpi.h>
#include <omp.h>
#include <stdio.h> 
#include <stdlib.h>	
#include <math.h>
#include <stdbool.h>


#define MIN(a,b) (((a)<=(b))?(a):(b))

size_t start(size_t a, size_t c, size_t i, size_t r)
{
        return a + i*c + MIN(i,r);
}

size_t finish(size_t a, size_t c, size_t i, size_t r)
{
        return a + ((i+1)*c) + MIN((i+1),r);
}

bool is_prime(size_t number)
{
	if ( number < 2 ) return false;
	if ( number == 2 ) return true;
	if ( number % 2 == 0 ) return false;

	for ( size_t i = 3, last = (size_t)(double)sqrt(number); i <= last; i += 2 )
		if ( number % i == 0 )
			return false;

	return true;
}

size_t count_primes(int number_threads, size_t my_start, size_t my_finish)
{
	
	size_t prime_count = 0;
	size_t current;
	#pragma omp parallel for num_threads(number_threads) default(none) shared(my_start, my_finish) private(current) reduction(+:prime_count)
	for ( current = my_start; current <= my_finish; ++current )
		if ( is_prime(current) )
			++prime_count;
	return prime_count;  
}


int main(int argc, char* argv[])
{
	int number_threads = omp_get_max_threads();

	MPI_Init(&argc, &argv);
	
	size_t a = 0;
	size_t b = 0; 
		
	int my_rank = -1; 
	int process_count = -1;

	char hostname[MPI_MAX_PROCESSOR_NAME]; // forma de ir a la segura  
	int len_hostname = -1;  	

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Get_processor_name(hostname, &len_hostname); 
	
	if( argc >= 2)
	{
		a = strtoull(argv[1], NULL, 10); 
		b = strtoull(argv[2], NULL, 10);
	}
	else if( my_rank == 0)
	{
		scanf("%zu %zu", &a, &b);
		for( int recieve = 1; recieve < process_count; ++recieve)
		{
			MPI_Send(&a, 1, MPI_UNSIGNED_LONG_LONG, recieve, 0, MPI_COMM_WORLD);
			MPI_Send(&b, 1, MPI_UNSIGNED_LONG_LONG, recieve, 0, MPI_COMM_WORLD);
		} 
	}
	else 
	{
		MPI_Recv(&a, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		MPI_Recv(&b, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
	}	
        size_t c = (b-a) / process_count;
        size_t r = (b-a) % process_count;

        size_t my_start = start(a, c, my_rank, r);
        size_t my_finish = finish(a, c, my_rank, r);

	size_t number_primes = 0;
	double starttime = 0.0; 
	double endtime = 0.0; 

	starttime = MPI_Wtime(); 	
        number_primes = count_primes(number_threads, my_start, my_finish); 
	endtime = MPI_Wtime(); 
	printf("Process %d on %s found %zu primes in range [%zu, %zu[ in %lf with %d threads\n", my_rank, hostname, number_primes, my_start, my_finish, endtime-starttime, number_threads);  
	
	MPI_Finalize(); 
	return 0; 
}
