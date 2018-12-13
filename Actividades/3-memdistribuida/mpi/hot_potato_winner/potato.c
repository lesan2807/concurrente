#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
	MPI_Init( &argc, &argv );
  	
	int my_rank = -1; 
	int process_count = -1;
	long long potato = 0; 

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	if(argc > 1 )
	{
		potato = strtoull(argv[1], NULL, 10);       
		if((long long)potato < 0)
			return printf("%s\n", "Potato is negative, must be positive"), 2; 
	}
	else 
		return printf("%s\n", "No potato given"), 1;
		
	long long my_potato = potato;

	int prev = (my_rank - 1 + process_count) % process_count; 
	int next = (my_rank + 1) % process_count; 
	
	int total = process_count; 
	
	int alive = 1; 

	if(my_rank == 0)
	{
		MPI_Send(&my_potato, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, MPI_COMM_WORLD); 
		MPI_Send(&total, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
	}
		
	while( potato > 0 && total > 1)
	{
		MPI_Recv(&my_potato, 1, MPI_UNSIGNED_LONG_LONG, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		MPI_Recv(&total, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		if(alive != 0)
		{
			--my_potato;
		}
		if( my_potato == 0)
		{
			--total;
			my_potato = potato; 
			alive = 0;
		}
		MPI_Send(&my_potato, 1, MPI_UNSIGNED_LONG_LONG, next, 0, MPI_COMM_WORLD);
		MPI_Send(&total, 1, MPI_INT, next, 0, MPI_COMM_WORLD); 
		if(total == 1 && alive != 0)
			printf("I won! %d\n", my_rank);
	}
	
	MPI_Finalize(); 

	return 0;
}
