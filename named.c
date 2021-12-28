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

float seconds = 0.0;

int main(int argc, char *argv[])
{

    int fd_named;
    //int sizea = 0;
    //int sizeb = 0;
    clock_t start;
    clock_t end;

    mkfifo("/tmp/named", 0666);
    printf("Please input the number of elements of the array\n");

    int num;

    scanf("%d", &num);

    if (num > 100000000)
    {

        printf("ENTER AN AMOUNT OF LESS THAN 100000000 bytes\n");
        exit(-1);
    }

    if (num < 1)
    {

        printf("ENTER positive values\n");
        exit(-1);
    }

    int id = fork();

    //printf("Start");

    if (id == -1)
    {

        printf("Error \n");
        exit(1);
    }

    start = clock();

    if (id == 0)
    {

        //printf("Start");

        //char P[sizea];
        char *P = (char *)malloc(num);

        //for (int i = 0; i < sizea; i++)
        //{

        //P[i] = 1 + rand() % 1000;
        //}

        fd_named = open("/tmp/named", O_WRONLY);

        // Stores time seconds

        write(fd_named, P, num);
        free(P);
    }

    else
    {
        //printf("End");

        fd_named = open("/tmp/named", O_RDONLY);

        //char C[sizeb];
        char *C = (char *)malloc(num);

        read(fd_named, C, num);

        end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        printf("Time of execution : %f\n", seconds);

        close(fd_named);
        free(C);
        return 0;
    }
}