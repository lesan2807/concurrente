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

	int prev = (my_rank - 1 + process_count) % process_count; 
	int next = (my_rank + 1) % process_count; 

	if(my_rank == 0)
		MPI_Send(&potato, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, MPI_COMM_WORLD); 
		
	while( potato > 0)
	{
		MPI_Recv(&potato, 1, MPI_UNSIGNED_LONG_LONG, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		--potato;
		if( potato == 0)
			printf("Rip %d\n", my_rank); 
		MPI_Send(&potato, 1, MPI_UNSIGNED_LONG_LONG, next, 0, MPI_COMM_WORLD);
	}
	
	MPI_Finalize(); 

	return 0;
}
