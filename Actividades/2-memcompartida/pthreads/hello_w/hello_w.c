#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* hello(void* data)
{
    size_t thread_number = (size_t)data;
    printf("Hello world from thread %zu\n", thread_number);
    return NULL;
}

int main(int argc, char* argv[])
{
  size_t number_threads = sysconf( _SC_NPROCESSORS_ONLN);
  if(argc > 1 )
    number_threads = strtoull(argv[1], NULL, 10);
  pthread_t threads[number_threads];

      for(size_t index = 0; index < number_threads; ++index)
        pthread_create(&threads[index], NULL, hello, (void*)index);

    printf("Hello from main thread\n");

    for(size_t index = 0; index < number_threads; ++index)
      pthread_join(threads[index], NULL);

    return 0;
}
