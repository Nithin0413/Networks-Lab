#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* child_thread(void* arg);
void cleanup_handler(void* arg);

int main()
{
    // create and start child thread
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, &child_thread, NULL);
    
    // wait for user input to stop child thread
    printf("Press enter to stop child thread.\n");
    getchar();
    
    // stop child thread
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);
    printf("Child thread stopped.\n");
    
    return 0;
}

void* child_thread(void* arg)
{
    pthread_cleanup_push(&cleanup_handler, NULL);
    
    while (1) {
        // block on read call
        char buffer[1024];
        int bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("read");
            break;
        }
        // process input
    }
    
    pthread_cleanup_pop(1);
    return NULL;
}

void cleanup_handler(void* arg)
{
    printf("Child thread terminating gracefully...\n");
}
