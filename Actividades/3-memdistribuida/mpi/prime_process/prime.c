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

size_t count_primes(size_t my_start, size_t my_finish)
{
	size_t prime_count = 0;
	size_t current;
	for ( current = my_start; current <= my_finish; ++current )
		if ( is_prime(current) )
			++prime_count;
	return prime_count;  
}


int main(int argc, char* argv[])
{
	double start_time = 0.0; 
	double end_time = 0.0; 

	MPI_Init(&argc, &argv);
	start_time = MPI_Wtime(); 
	
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

    number_primes = count_primes(my_start, my_finish); 

    if( my_rank == 0)
    {	
    	size_t total_primes = number_primes; 
    	for( int recieve = 1; recieve < process_count; ++recieve)
		{
			MPI_Recv(&number_primes, 1, MPI_UNSIGNED_LONG_LONG, recieve, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
			total_primes += number_primes;  
		}
		end_time = MPI_Wtime(); 
		printf("%zu primes found in range [%zu, %zu[ in %lfs with %d processes\n", total_primes, a, b, end_time-start_time, process_count);

    }
    else
    {
    	MPI_Send(&number_primes, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    } 
	
	MPI_Finalize(); 
	return 0; 
}
