// Including all needed libraries

#include <errno.h>
#include <signal.h>
#include <strings.h>
#include <sys/wait.h>
#include <time.h>
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

// Defining some aesthetic stuff and the maximum SIZE that an array can have, filling it with integers
// that have a dimension of 2 or 4 byte. SIZE is 2000000 since the maximum array dimension is 8 MB, so 2000000
// integers which each occupies at least 2 byte (For the computation we have calculated that each integer occupies at least 4 bytes).
// This is coherent also for the circular buffer.

#define CYN "\e[0;36m"
#define RESET "\e[0m"
#define RED "\e[0;31m"
#define ERROR_CHAR (-1)
#define TRUE 1
#define FALSE 0
#define _SVID_SOURCE
#define SIZE 2000000

// The CHECK(X) function is usefull to write on shell whenever a system call return an error.
// The function will print the name of the file and the line at which it found the error.
// It will end the check exiting with code 1.

#define CHECK(X) (                                                 \
    {                                                              \
        int __val = (X);                                           \
        (__val == -1 ? (                                           \
                           {                                       \
                               fprintf(stderr, "ERROR ("__FILE__   \
                                               ":%d) -- %s\n",     \
                                       __LINE__, strerror(errno)); \
                               exit(EXIT_FAILURE);                 \
                               -1;                                 \
                           })                                      \
                     : __val);                                     \
    })

// Main in which we have developed the code

int main(int argc, char* argv[]){

    // Thanks to these lines of code we can report on the file.txt (logfile.txt) 
	// all the operations we have done. The logfile.txt is created in the master process, in fact
	// there is the append inside the function fopen() that is not "a" like in every other process, but "w"-->
	// Creates an empty file for writing. If a file with the same name already exists, 
	// its content is erased and the file is considered as a new empty file. Whereas in other processes there is the append "a"-->
	// Appends to a file. Writing operations, append data at the end of the file. 
	// The file is created if it does not exist. 
	// We can "concatenate" all the things we do in every single process, reporting them in the logfile.txt, thanks
	// to the "a" append.

    FILE *out = fopen("logfile.txt", "a");
    if(out == NULL){

        printf("Error opening FILE");
    }

    // Initializing the variables needed for the named pipes that will transfer the time values
    // about the start and finish of the transfer directly to the master process.

    int fd_time0;
    int fd_time1;

    // Initialise shmid that is the id of the shared memory address for our buffer 

    int kBsize;
    int i;

    // Initialise shmid that is the id of the shared memory address for our buffer 

    int shmid;

    // The C library function clock_t clock(void) returns the number of clock ticks
    // elapsed since the program was launched.
    
    clock_t seconds0;
    clock_t seconds1;

    printf(CYN "\nEnter the amount of kB you want to transfer (maximum quantity is 100000 kB)" RESET "\n");

    scanf("%d", & kBsize);

    // The maximum amount of kB was requested by the professor.

    while (kBsize > 100000){

        printf(RED "ENTER AN AMOUNT OF KB LESS THAN 10000" RESET "\n");

        scanf("%d", & kBsize);
    }

    // The C library function time_t time(time_t *seconds) returns the time since 
    // the Epoch (00:00:00 UTC, January 1, 1970), measured in seconds

    time_t current_time;

    time(&current_time);

    // For the logfile

    fprintf(out, "AMOUNT OF KB TO TRANSFER: %d    Time:  %s", kBsize, ctime(&current_time));

    CHECK(fclose(out));

    // We have done a simple conversion. num represents the number of integers that must be transferred.

    int num = kBsize / 0.004;

    // Creating the Struct named Buffer needed for the sared memory.
    // A struct in the C programming language (and many derivatives) is a composite data type 
    // (or record) declaration that defines a physically grouped list of variables under one 
    // name in a block of memory, allowing the different variables to be accessed via a single 
    // pointer or by the struct declared name which returns the same address.

    typedef struct{

        char file_path[SIZE];
        int in;
        int out;
        sem_t full;
        sem_t empty;
        sem_t mutex;

    } Buffer;

    Buffer *ptr;

    // Shmid is the id of the shared memory address for our buffer 

    shmid = CHECK(shmget(IPC_PRIVATE, sizeof(ptr->file_path), IPC_CREAT | 0666));

    // Get a pointer to our buffer in shared memory 

    ptr = (Buffer*) shmat(shmid, NULL, 0);

    // Initialise the buffer 

    ptr->in = 0;
    ptr->out = 0;

    // Initialise our semaphores (2nd param "1" means shared betweeen processes)

    CHECK(sem_init(&ptr->empty, 1, SIZE));
    CHECK(sem_init(&ptr->full, 1, 0));

    // Thanks to the fork() we have created the consumer and the 
    // producer without using 2 separate scripts

    int id = CHECK(fork());

    if (id == -1){

        printf("Error forking...\n");
        exit(1);
    }

    if (id != 0){

        // The father process represent the producer P.

        // Taking the time in which the producer start writing the data to the consumer process (child)
        // and sending directly to the master process that will compute the exact transfer time.
        // To convert the number of CPU clock cycles into seconds, we need to use the CLOCKS_PER_SEC constant, which is also defined in the time.h header.
        // We send the time through a named pipe.

        CHECK(fd_time0 = open(argv[1], O_WRONLY));

        seconds0 = clock();

        double time_taken0 =(double) seconds0 / CLOCKS_PER_SEC;

        printf("Time 0 : %f\n", time_taken0);

        CHECK(write(fd_time0, &time_taken0, sizeof(time_taken0)));

        // Through this important while we have implemented the logic of: Circular buffer, semaphores and shared memory.
        // Circular buffers (also known as ring buffers) are fixed-size buffers that work as if the memory is contiguous & circular in nature. 
        // As memory is generated and consumed, data does not need to be reshuffled-rather, the head/tail pointers are adjusted. When data is added, the head pointer advances. When data is consumed, the tail pointer advances. 
        // If you reach the end of the buffer, the pointers simply wrap around to the beginning.

        // "a" is the counter that stop the while at the achievement of the number "num".

        int a = 0;

         // Shared memory: shared memory is memory that may be simultaneously accessed by multiple programs with an intent to provide communication among 
        // them or avoid redundant copies

        // Thanks to the semaphores, write operation will not go beyond the read operation so 
        // that any data is lost. Also the read operation doesn't have to do nothing if nothing has to be read.

        while(a < num){

            // Logic of circular buffer in writing

            sem_wait(&ptr->empty);

            ptr->file_path[ptr->in] = 1 + rand()%100;; 
            ptr->in = (ptr->in + 1)%SIZE;

            a++;

            sem_post(&ptr->full);
        }
    }

    else{
        
        // Child process --> consumer
        
        int b;

        while (b < num){

            // Logic of circular buffer in reading

            sem_wait(&ptr->full);

            int h = ptr->file_path[ptr->out];
            ptr->out = (ptr->out + 1)%SIZE;

            b++;

            sem_post(&ptr->empty);
        }

        // Taking the time in which the producer start writing the data to the consumer process (child)
        // and sending directly to the master process that will compute the exact transfer time.
        // To convert the number of CPU clock cycles into seconds, we need to use the CLOCKS_PER_SEC constant, which is also defined in the time.h header.
        // We send the time through a named pipe.

        // Open the pipe for writing between the producer and consumer for transfer data

        CHECK(fd_time1 = open(argv[2], O_WRONLY));

        // Stores time in seconds

        seconds1 = clock();

        double time_taken1 =(double) seconds1 / CLOCKS_PER_SEC;

        printf("Time 1 : %f\n", time_taken1);

        CHECK(write(fd_time1, &time_taken1, sizeof(time_taken1)));
    }

    // Destroying semaphores

    CHECK(sem_destroy(&ptr->empty));
    CHECK(sem_destroy(&ptr->full));
    
    // Detach the shared memory and deallocate the memory segment 
    
    shmdt(&ptr); 
    CHECK(shmctl(shmid, IPC_RMID, 0));

    // Closing pipes

    close(fd_time1); 
    close(fd_time0);

    return 0;
}

