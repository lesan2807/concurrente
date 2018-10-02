#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
  size_t number;
  size_t count;
  size_t* next_to_print;
  pthread_mutex_t lock;
}thread_info_t;

void* hello(void* data)
{
    thread_info_t* my_info = (thread_info_t*)data;
    while(*my_info->next_to_print < my_info->count)
      ;
    printf("Hello world from thread %zu of %zu\n", my_info->count, my_info->number);
    pthread_mutex_lock(&my_info->lock);
    ++(*my_info->next_to_print);
    pthread_mutex_unlock(&my_info->lock);
    return NULL;
}

int main(int argc, char* argv[])
{
  size_t number_threads = sysconf( _SC_NPROCESSORS_ONLN);
  if(argc > 1 )
    number_threads = strtoull(argv[1], NULL, 10);
  pthread_t threads[number_threads];
  size_t next = 0;
  thread_info_t* info = calloc(number_threads, sizeof(thread_info_t));
    for(size_t index = 0; index < number_threads; ++index)
    {
      info[index].number = number_threads-1;
      info[index].count = index;
      info[index].next_to_print = &next;
      pthread_mutex_init(&info[index].lock, NULL);
      pthread_create(&threads[index], NULL, hello, (void*)&info[index]);
    }

    printf("Hello from main thread\n");

    for(size_t index = 0; index < number_threads; ++index)
    {
      pthread_join(threads[index], NULL);
      pthread_mutex_destroy(&info[index].lock);
    }

    free(info);
    return 0;
}
