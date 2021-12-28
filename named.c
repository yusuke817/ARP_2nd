#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <termios.h>

int main(int argc, char *argv[])
{
    // Initialize the file descriptor
    int fd_named;
    // set the clock
    clock_t start;
    clock_t end;
    // create the named pipe
    mkfifo("/tmp/named", 0666);
    // user can decide the bytes of the data
    printf("Please input the bytes of transmitted data\n");

    int num;

    scanf("%d", &num);

    // 100mb is maximum size
    if (num > 100000000)
    {

        printf("ENTER AN AMOUNT OF LESS THAN 100000000 bytes\n");
        exit(-1);
    }
    //the amount of data should be positive
    if (num < 1)
    {

        printf("ENTER positive values\n");
        exit(-1);
    }

    // with fork(), the consumer and the producer are not separated into 2 parts
    int id = fork();

    if (id == -1)
    {

        printf("Error \n");
        exit(1);
    }

    //stop-watch begins
    start = clock();

    if (id == 0)
    {
        //child process plays a role of producer P
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *P = (char *)malloc(num);
        //open the pipe for writing the data
        fd_named = open("/tmp/named", O_WRONLY);
        //writing the data for the consumer
        write(fd_named, P, num);
        //release the momory occupied with malloc
        free(P);
    }

    else
    {
        //parent process plays a role of consumer C
        //open the pipe for readng the data
        fd_named = open("/tmp/named", O_RDONLY);
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *C = (char *)malloc(num);
        //reading the data from the producer
        read(fd_named, C, num);
        //stop-watch finishes
        end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        //printimg the calculation time
        printf("Time of execution : %f\n", seconds);
        //close the pipe
        close(fd_named);
        //release the momory occupied with malloc
        free(C);
        return 0;
    }
}