#include <pthread.h>
#include <stdio.h>

void* hello(void* data)
{
    size_t thread_number = (size_t)data;
    printf("Hello world from thread %zu\n", thread_number); 
    return NULL;
}

int main()
{
    pthread_t thread; 
    pthread_create(&thread, NULL, hello, (void*)2);
    printf("Hello from main thread\n"); 
    pthread_join(thread, NULL);
    return 0; 
}
