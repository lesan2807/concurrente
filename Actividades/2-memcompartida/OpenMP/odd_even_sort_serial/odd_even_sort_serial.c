#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// https://cboard.cprogramming.com/c-programming/132024-casting-rand-generate-double-float-etc.html
double randDouble(double lower, double upper)
{
    int range;           /* the width of given interval */
    double randomValue;  /* the random number generated */
    
    range = upper - lower;

    randomValue = (double) rand();       /* get random double*/
    randomValue = randomValue / RAND_MAX;   /* convert to value between 0 and 1 */
    randomValue = randomValue * range;      /* fit into desired range */
    randomValue = randomValue + lower;      /* adjust to fit desired interval */
    
    return randomValue;
}

void swap(double *a, double *b)
{
   double t = *b;
   *b = *a;
   *a = t;
}

void serial_odd_even_sort(size_t n, double arr[n])
{
	for ( size_t phase = 0; phase < n; ++phase )
	{
		if ( phase % 2 == 0 )
		{
			for ( size_t i = 1; i < n; i += 2 )
				if ( arr[i - 1] > arr[i] )
					swap( &arr[i - 1], &arr[i] );
		}
		else
		{
			for ( size_t i = 1; i < n - 1; i += 2 )
				if ( arr[i] > arr[i + 1] )
					swap( &arr[i], &arr[i + 1]);
		}
	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	size_t n = 10; 
	if(argc > 1)
		n = strtoull(argv[1], NULL, 10);
	double arr[n]; 
	for(size_t index = 0; index < n; ++index)
	{
		arr[index] = randDouble(1.0, 10.0);
	}
	serial_odd_even_sort(n, arr);
	for(size_t index = 0; index < n; ++index)
	{
		printf("%lf ", arr[index]);
	}
	printf("\n");
	return 0;
}


