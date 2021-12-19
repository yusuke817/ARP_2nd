#include <errno.h>
#include <signal.h>
#include <strings.h>
#include <sys/wait.h>
#include <time.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define CYN "\e[0;36m"
#define RESET "\e[0m"
#define RED "\e[0;31m"
#define ERROR_CHAR (-1)
#define TRUE 1
#define FALSE 0
#define _SVID_SOURCE
#define SIZE 2000000

int main(int argc, char* argv[]){


    int fd_w;
    int fd_r;
    int size;
    int i;
    int shmid;
    
    clock_t start;
    clock_t end;

    printf(CYN "Enter the amount of kB you want to transfer (maximum quantity is 100000 kB)" RESET "\n");

    scanf("%d", & size);

    while (size > 100000){

        printf(RED "ENTER AN AMOUNT OF KB LESS THAN 10000" RESET "\n");

        scanf("%d", & size);
    }

    int num = size / 0.004;

    typedef struct{

        char file_path[SIZE];
        int in;
        int out;
        sem_t full;
        sem_t empty;
        sem_t mutex;

    } Buffer;

    Buffer *ptr;

    /* shmid is the id of the shared memory address for our buffer */

    shmid = shmget(IPC_PRIVATE, sizeof(ptr->file_path), IPC_CREAT | 0666);

    /* get a pointer to our buffer in shared memory */

    ptr = (Buffer*) shmat(shmid, NULL, 0);

    /* initialise the buffer */

    ptr->in = 0;
    ptr->out = 0;

    /* initialise our semaphores (2nd param 1 means shared betweeen processes */

    sem_init(&ptr->empty, 1, SIZE);
    sem_init(&ptr->full, 1, 0);

    // fork

    int id = fork();

    if (id == -1){

        printf("Error forking...\n");
        exit(1);
    }

    if (id != 0){

        /* this is the producer process */

        fd_w = open(argv[1], O_WRONLY);

        start = clock();

        double time_taken0 =(double) start / CLOCKS_PER_SEC;

        printf("Time 0 : %f\n", time_taken0);

        write(fd_w, &time_taken0, sizeof(time_taken0));

        int a = 0;

        while(a < num){

            a++;

            sem_wait(&ptr->empty);

            ptr->file_path[ptr->in] = 1 + rand()%100;; 
            ptr->in = (ptr->in + 1)%SIZE;

            sem_post(&ptr->full);
        }
    }

    else{
        
        /* this is the consumer process */
        
        int b;

        while (b < num){

            b++;

            sem_wait(&ptr->full);

            int h = ptr->file_path[ptr->out];
            ptr->out = (ptr->out + 1)%SIZE;

            sem_post(&ptr->empty);
        }

        fd_r = open(argv[2], O_WRONLY);

        end = clock();

        double time_taken1 =(double) end / CLOCKS_PER_SEC;

        printf("Time 1 : %f\n", time_taken1);

        write(fd_r, &time_taken1, sizeof(time_taken1));
          
    }

    sem_destroy(&ptr->empty);
    sem_destroy(&ptr->full);
    
    /* detach the shared memory and deallocate the memory segment */
    
    shmdt(&ptr);
    shmctl(shmid, IPC_RMID, 0);

    close(fd_w);
    close(fd_r);

    /* finally, close the signature file */

    return 0;
}