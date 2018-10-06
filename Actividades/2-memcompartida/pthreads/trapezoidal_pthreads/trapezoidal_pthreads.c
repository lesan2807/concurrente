#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
  size_t my_number_threads;
  size_t my_current;
  double my_a;
  double my_b;
  double my_n;
  double my_sum;
  size_t my_init_range;
  size_t my_final_range;
}thread_info_t;

double f(double x)
{
    // return cos(x)*2*x + x/8;
    return x;
}

void* trapezoidal_rule(void* data)
{
    thread_info_t* my_info = (thread_info_t*)data;
    double delta_x = (my_info->my_b - my_info->my_a)/my_info->my_n;

    for(size_t index = my_info->my_init_range; index < my_info->my_final_range; ++index)
    {
        my_info->my_sum += 2*f(my_info->my_a + index*delta_x);
    }
    return NULL;
}

size_t initial_range(size_t n, size_t num_threads, size_t thread_id)
{
    size_t initial = 0;
    size_t division = n/num_threads;
    initial = thread_id*division;
    return initial;
}

size_t final_range(size_t n, size_t num_threads, size_t thread_id)
{
    size_t final = 0;
    size_t division = n/num_threads;
    final = (thread_id+1)*division;
    return final;
}

int main(int argc, char* argv[])
{
    double a = strtod(argv[1], NULL);
    double b = strtod(argv[2], NULL);
    double n = strtod(argv[3], NULL);

    double delta_x = (b-a)/n;
    size_t number_threads = sysconf( _SC_NPROCESSORS_ONLN);
    if( argc > 4)
        number_threads = strtoull(argv[4], NULL, 10);

    pthread_t threads[number_threads];
    thread_info_t* info = calloc(number_threads, sizeof(thread_info_t));
    double sum = 0.0;
    sum +=  f(a) + f(a+n*delta_x);

    for(size_t index = 0; index < number_threads; ++index)
    {
        info[index].my_number_threads = number_threads;
        info[index].my_current = index;
        info[index].my_a = a;
        info[index].my_b = b;
        info[index].my_n = n;
        info[index].my_init_range = initial_range(n, number_threads, index);
        info[index].my_final_range = final_range(n, number_threads, index);
        info[index].my_sum = 0.0;
        pthread_create(&threads[index], NULL, trapezoidal_rule, (void*)&info[index]);

    }

    for(size_t index = 0; index < number_threads; ++index)
        pthread_join(threads[index], NULL);


    for(size_t index = 0; index < number_threads; ++index)
        sum += info[index].my_sum;

    printf("%.2lf = area\n", delta_x/2*sum);

    free(info);

    return 0;
}
