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
double trapezoidal_rule(double a, double b, double n)
{
    double delta_x = (b-a)/n;
    double trapezoid_sum = f(a) + f(a+(n*delta_x));
    for(size_t index = 0; index <= n; ++index)
    {
        trapezoid_sum += 2*f(a+(index*delta_x));
    }
    return (delta_x/2)*trapezoid_sum;
}

int main(int argc, char* argv[])
{
    (void)argc;
    size_t a = strtoull(argv[1], NULL, 10);
    size_t b = strtoull(argv[2], NULL, 10);
    size_t n = strtoull(argv[3], NULL, 10);

    printf("%lf\n", trapezoidal_rule(a, b, n));

    return 0;
}
