int main(int argc, char const *argv[])
{
	MPI_Init(&argc, &argv); 
	int my_rank = -1; 
	int process_count = -1; 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &process_count); 

	double vocal_prob = argv[1]; 
	double consonant_prob = argv[2];; 
	int rounds = 1; 
	char* message = NULL; 
	if (argc > 2)
		rounds = atoi(argv[3]); 

	if(my_rank == 0)
	{
		message = read_message(); 
		for(int send = 1; send < process_count; ++send)
		{
			MPI_Send(message, strlen(message), MPI_CHAR, send, 0, MPI_COMM_WORLD); 
		}
	}
	else
	{
		MPI_Recv(message, strlen(message), MPI_CHAR, 0, 0 MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
	}

	int next = (my_rank + 1) % process_count; 
	int prev = (my_rank - 1 + process_count) % process_count; 	
	int round_so_far = 0; 
	if(my_rank == 0)
	{
		MPI_Send(&round_so_far, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
		modify_message(vocal_prob, consonant_prob, message); 
		MPI_Send(message, stlen(message), MPI_CHAR, 1, 0, MPI_COMM_WORLD); 
	}

	while( round_so_far < rounds)
	{
		MPI_Recv(message, strlen(message), MPI_CHAR, PRVE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
		MPI_Recv(&round_so_far, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if(my_rank == process_count -1)
			++round_so_far; 
		modify_message(vocal_prob, consonant_prob, message); 
		MPI_Send(message, stlen(message),MPI_CHAR, next, 0, MPI_COMM_WORLD); 
		MPI_Send(&round_so_far, 1, MPI_INT, next, 0, MPI_COMM_WORLD); 
	}

	free(message); 

	MPI_Finalize(); 

	return 0;
}

char* read_message()
{
	char* message_to_read = (char*)calloc(10000, sizeof(char)); 
	int c = -1; 
	int index = 0; 
	while(c != EOF)
	{
		c = fgetc(stdin); 
		message_to_read[index] = c; 
		++index; 
	}
	return message_to_read; 
}

void modify_message(double vocal_prob, double consonant_prob, char* message)
{
	srand(time(NULL)); 
	int vocal = vocal_prob*10; 
	int consonant = consonant_prob*10; 
	int no_change = 10 - vocal -consonant; 
	size_t rand_pos = (size_t)rand() % strlen(message); 
	int rand_change = rand() % 10 + 1; 
	if( rand_change >= no_change + 1 && rand_change <= vocal+no_change) // change vocal 
	{
		char vocals[] = "aeiouAEIOU"; 
		int rand_vocal = rand()% strlen(vocals); 
		if( strchr(vocals, message[rand_pos]) && isalpha(message[rand_pos]) )
			message[rand_pos] = vocals[rand_vocal]; 
	}
	else if( rand_change >= no_change+vocals+1 && rand_change <= no_change+vocal+consonant) // change consonant
	{
		char consontants[] = "bcdfghjklmnpqrstvwxyzBCDFGHIKLMNPQRSTVWXYZ"; 
		int rand_consontant = rand()% strlen(consontants); 
		if( strch(consontants, message[rand_pos]) && isalpha(message[rand_pos]) )
			message[rand_pos] = consontants[rand_consontant]; 
	}
}