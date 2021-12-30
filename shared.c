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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[])
{

    int num = 10;
    int sizea = 0;
    int sizecb = 0;
    int i;
    int shmid;

    // set the clock
    clock_t start;
    clock_t end;

    // user can decide the bytes of the data
    printf("Please input the bytes of transmitted data\n");

    scanf("%d", &sizea);

    // user can decide the size of circular buffer
    printf("Please input the size of circular buffer which should be equal or smaller than the bytes of transmitted data\n");

    scanf("%d", &sizecb);

    // 100mb is maximum size
    if (sizea > 100000000 || sizecb > 100000000)
    {

        printf("ENTER AN AMOUNT OF LESS THAN 100000000 bytes\n");
        exit(-1);
    }

    //the amount of data should be positive
    if (sizea < 1 || sizecb < 1)
    {

        printf("ENTER positive values\n");
        exit(-1);
    }

    //circular buffer should be equal or smaller than the size of transmitted data
    if (sizecb > sizea)
    {

        printf("sizecb should be smaller than the size of other two buffers\n");
        exit(-1);
    }

    //Cestting the struct named Buffer needed for the shared memory.
    typedef struct
    {

        char data_cb[sizecb];
        int in;
        int out;
        sem_t full;
        sem_t empty;
        sem_t mutex;

    } Buffer;

    Buffer *ptr;

    key_t key = ftok("shmfile", 65);

    /* shmid is the id of the shared memory address for buffer */
    shmid = shmget(IPC_PRIVATE, sizeof(ptr->data_cb), IPC_CREAT | 0666);

    /* get a pointer to buffer in shared memory */

    ptr = (Buffer *)shmat(shmid, NULL, 0);

    /* initialise the buffer */

    ptr->in = 0;
    ptr->out = 0;

    /* initialise semaphores */

    sem_init(&ptr->empty, 1, sizecb);
    sem_init(&ptr->full, 1, 0);

    // with fork(), the consumer and the producer are not separated into 2 parts
    int id = fork();

    if (id == -1)
    {

        printf("Error forking...\n");
        exit(1);
    }

    //stop-watch begins    
    start = clock();

    if (id != 0)
    {

	    //parent process plays a role of producer P

        int p = 0;

        // Dynamic memory which is malloc is used here to accept big amount of data
        char *data_a = (char *)malloc(sizea);

        // write the random value in data_a array
        for (int j = 0; j < sizea; j++)
        {
            data_a[j] = 1 + rand() % 100;
        }

        // the data in sizea is sent several times which is equal to sizea / sizecb with circular buffer
        while (p < sizea / sizecb)
        {
            //circular buffer in writing
            sem_wait(&ptr->empty);

            // with circular buffer, data is sent
            for (int j = 0; j < sizecb; j++)
            {
                if (sizea < j + p * sizecb)
                {
                    ptr->data_cb[j] = data_a[j + p * sizecb];
                }

                else
                {
                    ptr->data_cb[j] = 0;
                }
            }

            p++;
            

            sem_post(&ptr->full);
        }

	    //release the memory occupied with malloc
        free(data_a);
    }

    else
    {
	    // child process plays a role of consumer C
        key_t key = ftok("shmfile", 65);

        /* shmid is the id of the shared memory address for our buffer */

        shmid = shmget(key, sizeof(ptr->data_cb), IPC_CREAT | 0666);

        char *data_b = (char *)malloc(sizea);

        int pos = 0;

        int c = 0;

        // the data in sizea is received several times which is equal to sizea / sizecb with circular buffer
        while (c < sizea / sizecb)
        {

            sem_wait(&ptr->full);

            // with circular buffer, data is received
            
            for (int j = 0; j < sizecb; j++)
            {

                if (sizea < pos)
                {
                    int h = ptr->data_cb[j];
                    data_b[pos++] = h;
                }

                else
                {
                    break;
                }
            }

            c++;
            
            sem_post(&ptr->empty);
        }

	    //release the memory occupied with malloc
        free(data_b);

        //stop-watch finishes
        end = clock();

        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	    //printimg the calculation time        
        printf("Time of execution : %f\n", seconds);
    }
    printf("stopping");

    //destroy semaphore
    sem_destroy(&ptr->empty);
    sem_destroy(&ptr->full);

    //detach the shared memory and release the memory
    shmdt(&ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}