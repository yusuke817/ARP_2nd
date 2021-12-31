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
#include <semaphore.h>

static int received = 0;
int n, m;
int chunk = 1024;
struct timespec begin, end;
// void sig_handler(int sig)
// {
//     received = 1;
// }

int main(int argc, char *argv[])
{
    // set the clock
    time_t start;
    time_t finish;
    //for pipes to put the file descriptors, two arrays are needed
    //one process can write the data with one array
    //the other process can read the data with the other array
    int u[2];

    if (pipe(u) < 0)
    {
        exit(1);
    }

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
    pid_t id = fork();

    if (id == -1)
    {

        printf("Error \n");
        exit(1);
    }

    //stop-watch begins
    clock_gettime(CLOCK_REALTIME, &begin);

    //stop-watch begins
    //start = clock();

    if (id != 0)
    {
        //child process plays a role of producer P
        close(u[0]);

        // signal(SIGUSR1, sig_handler);

        // Dynamic memory which is malloc is used here to accept big amount of data
        char *P = (char *)malloc(num);

        for (int j = 0; j < num; j++)
        {
            P[j] = 1 + rand() % 100;
        }
        //printf("last : %d\n", P[100]);

        for (int k = 0; k < (num / chunk); k++)
        //for (int k = 0; k < num; k++)
        {
            //writing the data for the consumer
            n = write(u[1], P + (k * chunk), chunk);
            m = (k + 1) * chunk;
            //write(u[1], P + (k * 10000), 100);

            //     while (received == 0)
            //     {
            //         ;
            //     }
            //     received = 0;
        }

        n = write(u[1], P + m, num % chunk);

        //release the momory occupied with malloc
        free(P);
        close(u[1]);
    }

    else
    {
        //parent process plays a role of consumer C
        close(u[1]);
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *C = (char *)malloc(num);
        //reading the data from the producer
        int error;

        for (int k = 0; k < (num / chunk); k++)
        {
            //writing the data for the consumer
            n = read(u[0], C + (k * chunk), chunk);
            m = (k + 1) * chunk;
            // kill(id, SIGUSR1);
            //error = kill(id, SIGUSR1);
            //printf("dd, %d", error);
        }
        n = read(u[0], C + m, num % chunk);
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
        //float seconds = (float)(finish - start) / CLOCKS_PER_SEC;
        //printimg the calculation time
        // printf("last : %d\n", C[100]);
        //printf("Time of execution : %f\n", seconds);

        //write(fd_r, &end, sizeof(end));
        //release the momory occupied with malloc
        free(C);
        //wait(NULL);
        close(u[0]);
    }

    //wait(NULL);

    return 0;
}
