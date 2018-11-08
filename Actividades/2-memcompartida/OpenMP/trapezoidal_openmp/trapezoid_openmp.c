#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double f(double x)
{
    //return cos(x)*2*x + x/8;
    return x; 
}

// https://en.wikipedia.org/wiki/Trapezoidal_rule
// https://www.geeksforgeeks.org/trapezoidal-rule-for-approximate-value-of-definite-integral/
double trapezoidal_rule(double a, double b, size_t n, int number_threads)
{
    double x; 
    size_t index; 
    double delta_x = (b-a)/n;
    double trapezoid_sum = f(a) + f(a+(n*delta_x));
    #pragma omp parallel for num_threads(number_threads) default(none) shared(a, n, delta_x, trapezoid_sum) private(index, x)
    for(index = 0; index <= n; ++index)
    {
        x = a + index*delta_x;
        #pragma omp critical
        trapezoid_sum += 2*f(x);
    }   
    return (delta_x/2)*trapezoid_sum;
}



int main(int argc, char* argv[])
{
    size_t a = strtoull(argv[1], NULL, 10);
    size_t b = strtoull(argv[2], NULL, 10);
    size_t n = strtoull(argv[3], NULL, 10);
    int number_threads = omp_get_max_threads(); 
    if(argc > 4)
        number_threads = atoi(argv[4]); 

    printf("%lf\n", trapezoidal_rule(a, b, n, number_threads));

    return 0;
}
