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
    // set the clock
    time_t start;
    time_t end;
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
    int id = fork();

    if (id == -1)
    {

        printf("Error \n");
        exit(1);
    }
    //stop-watch begins
    start = clock();

    if (id != 0)
    {
        close(u[0]);
        //child process plays a role of producer P
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *P = (char *)malloc(num);

        for (int j = 0; j < num; j++)
        {
            P[j] = 1 + rand() % 100;
        }

        //writing the data for the consumer
        write(u[1], P, num);
        //printf("last : %d\n", P[num - 1]);

        //release the momory occupied with malloc
        free(P);
        //close(u[1]);
    }

    else
    {
        //parent process plays a role of consumer C
        close(u[1]);
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *C = (char *)malloc(num);
        //reading the data from the producer
        int error;
        read(u[0], C, num);
        //error = read(u[0], C, num);
        printf("e %d", error);
        //stop-watch finishes
        end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        //printimg the calculation time
        //printf("last : %d\n", C[num - 1]);
        printf("Time of execution : %f\n", seconds);

        //write(fd_r, &end, sizeof(end));
        //release the momory occupied with malloc
        free(C);
        //wait(NULL);
        //close(u[0]);
    }

    //wait(NULL);

    return 0;
}
