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
    int n, m;
    int chunk = 1024;
    struct timespec begin, end;
    // set the clock
    clock_t start;
    clock_t finish;
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

    clock_gettime(CLOCK_REALTIME, &begin);

    //stop-watch begins
    start = clock();

    if (id == 0)
    {
        //child process plays a role of producer P
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *P = (char *)malloc(num);

        for (int j = 0; j < num; j++)
        {
            P[j] = 1 + rand() % 100;
        }
        //printf("last : %d\n", P[100]);

        //open the pipe for writing the data
        fd_named = open("/tmp/named", O_WRONLY);

        for (int k = 0; k < (num / chunk); k++)
        //for (int k = 0; k < num; k++)
        {
            //writing the data for the consumer
            n = write(fd_named, P + (k * chunk), chunk);
            m = (k + 1) * chunk;
            //write(u[1], P + (k * 10000), 100);

            //     while (received == 0)
            //     {
            //         ;
            //     }
            //     received = 0;
        }

        n = write(fd_named, P + m, num % chunk);

        //writing the data for the consumer
        //write(, P, num);
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
        for (int k = 0; k < (num / chunk); k++)
        {
            //writing the data for the consumer
            n = read(fd_named, C + (k * chunk), chunk);
            m = (k + 1) * chunk;
            // kill(id, SIGUSR1);
            //error = kill(id, SIGUSR1);
            //printf("dd, %d", error);
        }
        n = read(fd_named, C + m, num % chunk);
        //error = read(u[0], C, num);
        //printf("e %d", error);
        //stop-watch finishes
        clock_gettime(CLOCK_REALTIME, &end);
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed = seconds + nanoseconds * 1e-9;
        //stop-watch finishes
        finish = clock();
        printf("Time of execution : %.5f\n", elapsed);

        //reading the data from the producer
        //stop-watch finishes
        //end = clock();
        //float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        //printimg the calculation time
        //printf("Time of execution : %f\n", seconds);
        //close the pipe
        close(fd_named);
        //release the momory occupied with malloc
        free(C);
        return 0;
    }
}