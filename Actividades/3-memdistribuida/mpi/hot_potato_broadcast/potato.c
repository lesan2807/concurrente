#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))



int main(int argc, char* argv[])
{
  MPI_Init(&argc, &argv);

  int my_rank = -1;
  int im_initial_process=0;
  int process_count = -1;
  int initial_process=0;
  int threads = 3;
  bool is_process_playing=1;

  double t1, t2;
  t1 = MPI_Wtime();

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &process_count);

  int potato=0;
  if(argc > 1){
    potato=strtol(argv[1],NULL,10);
   	initial_process=strtol(argv[2],NULL,10);
  }else{
    if(my_rank==0){
      scanf("%u", &potato);
      scanf("%u", &initial_process);
      for ( int source = 1; source < process_count; ++source ){
          MPI_Send(&potato, 1, MPI_INT,source, 0, MPI_COMM_WORLD);
          MPI_Send(&initial_process, 1, MPI_INT,source, 0, MPI_COMM_WORLD);
      }
    }
    else{
      MPI_Recv(&potato, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Recv(&initial_process, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
  }
	int params[4];
  params[0]=potato;
  params[1]=process_count;
  params[2]=0;
  params[3]=initial_process;

  int next_process=(my_rank+1)%process_count;
  int prev_process=(my_rank-1)%process_count;

  if(my_rank == 0){
	  prev_process=process_count-1;
  }

  int remaining_players=0;
  int is_game_over=0;

  int process_playing=initial_process;
  potato=params[0];
  remaining_players=params[1];
  is_game_over=params[2];
  process_playing=params[3];

  while( true ){
  	if(my_rank ==  process_playing && is_process_playing){
  		if(remaining_players == 1 && !is_game_over ){
  			is_game_over=1;
  			printf("process %d has won the game\n",my_rank);
  		}
  		if(is_process_playing){
  			if(potato%2 == 0){
  				potato/=2;
  			}else{
  				potato=(3*potato)+1;
  			}
  			if(my_rank%3 == 0)
  				++potato;
  			else
  				potato=0;
  			if(potato <= 1){
      		--remaining_players;
      		potato=process_count;
      		is_process_playing=0;
      		printf("Potato went Boom on process %d\n",my_rank);
  			}
  		}
  	}
    params[0]=potato;
    params[1]=remaining_players;
    params[2]=is_game_over;
    params[3]=next_process;
  	MPI_Bcast(&params,4,MPI_INT,process_playing,MPI_COMM_WORLD);
  	potato=params[0];
    remaining_players=params[1];
    is_game_over=params[2];
  	process_playing=params[3];
  	if(is_game_over)
  		break;
  }
  MPI_Finalize();
  return 0;
}
