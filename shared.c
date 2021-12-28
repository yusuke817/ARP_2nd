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

int main(int argc, char* argv[]){

    int num=10;
    int sizea = 0;
    int sizeb = 0;
    int sizecb = 0;
    int i;
    int shmid;
    
    clock_t start;
    clock_t end;

    printf("Please input the size of producer buffer\n");

    scanf("%d", & sizea);

    printf("Please input the size of consumer buffer\nwhich should be smaller than the size of producer buffer\n");

    scanf("%d", & sizeb);

    printf("Please input the size of circular buffer\nwhich should be equal or smaller than the size of other two buffers\n");

    scanf("%d", & sizecb);

     if (sizea > 100000000 || sizeb > 100000000 || sizecb > 100000000 ){

         printf("ENTER AN AMOUNT OF LESS THAN 100000000 bytes\n");
         exit(-1);
     }

     if (sizea < 1 || sizeb < 1 || sizecb < 1){

         printf("ENTER positive values\n");
         exit(-1);
     }

     if (sizeb > sizea){

         printf("the size of consumer buffer should be equal or smaller than the size of producer buffer\n");
         exit(-1);
     }

     if (sizecb > sizea || sizecb > sizeb){

         printf("sizecb should be smaller than the size of other two buffers\n");
         exit(-1);
     }

    typedef struct{

        char data_cb[sizecb];
        int in;
        int out;
        sem_t full;
        sem_t empty;
        sem_t mutex;

    } Buffer;

    Buffer *ptr;
    
    key_t key = ftok("shmfile",65);

    /* shmid is the id of the shared memory address for our buffer */
    shmid = shmget(IPC_PRIVATE, sizeof(ptr->data_cb), IPC_CREAT | 0666);

    /*shmid = shmget(key, sizeof(ptr->buffer_data), IPC_CREAT | 0666);*/
    
    /* get a pointer to our buffer in shared memory */

    ptr = (Buffer*) shmat(shmid, NULL, 0);

    /* initialise the buffer */

    ptr->in = 0;
    ptr->out = 0;

    /* initialise our semaphores */

    sem_init(&ptr->empty, 1, sizecb);
    sem_init(&ptr->full, 1, 0);

    int id = fork();

    if (id == -1){

        printf("Error forking...\n");
        exit(1);
    }
        start = clock();

    if (id != 0){

        /* this is the producer process */

        int p = 0;
        char data_a[sizea];

        for(int j=0; j<sizea; j++){
        data_a[j] = 1 + rand()%100;    
        }

        while(p < sizea/sizecb){

            sem_wait(&ptr->empty);

            for(int j=0; j<sizecb; j++){
                if(sizea<j+p*sizecb){
                    ptr->data_cb[j]= data_a[j+p*sizecb]; 
                }

                else{
                    ptr->data_cb[j]= 0;
                }
            //ptr->in = (ptr->in + 1)%sizecb;                
            }            

            p++;

            sem_post(&ptr->full);
        }
    }

    else{
        //printf("Child starting\n"); 
        
	    key_t key = ftok("shmfile",65);

	    /* shmid is the id of the shared memory address for our buffer */

	    shmid = shmget(key, sizeof(ptr->data_cb), IPC_CREAT | 0666);
        char data_b[sizeb];
        int pos = 0;
        
        int c=0;

        while (c < sizeb/sizecb){

            sem_wait(&ptr->full);

            for(int j=0; j<sizecb; j++){

                if(sizeb < pos){
                    int h =ptr->data_cb[j];
                    data_b[pos++]=h; 
                }

                else{
                    break;
                }
               
            }            

            c++;

            sem_post(&ptr->empty);
        }
        
        end = clock();

        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    	printf("Time of execution : %f\n", seconds); 
          
    }
    printf("stopping");

    sem_destroy(&ptr->empty);
    sem_destroy(&ptr->full);
        
    shmdt(&ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
