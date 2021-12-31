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

    // the producer cannot send the data at one time when the size of data is too big
    // so with chunk, I developped the program so that producer can send the data separately with small units
    int n, m;
    int chunk = 1024;

    // to measure the wall time which is the time passed in the execution, the structure is set
    struct timespec begin, end;

    //set the clock for CPU time calculation
    time_t start;
    time_t finish;

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

    //stop-watch for wall time begins
    clock_gettime(CLOCK_REALTIME, &begin);

    //stop-watch for CPU time begins
    start = clock();

    if (id == 0)
    {
        //child process plays a role of producer P
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *P = (char *)malloc(num);

        // the array is filled with random data
        for (int j = 0; j < num; j++)
        {
            P[j] = 1 + rand() % 100;
        }

        //open the pipe for writing the data
        fd_named = open("/tmp/named", O_WRONLY);

        // Except the end of the data, the data is transmitted in the fixed unit which is "chunk"
        for (int k = 0; k < (num / chunk); k++)
        {
            //writing the data for the consumer
            n = write(fd_named, P + (k * chunk), chunk);
            m = (k + 1) * chunk;
        }

        // in the end, the rest of the data is transmitted
        n = write(fd_named, P + m, num % chunk);

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

        // Except the end of the data, the data is read in the fixed unit which is "chunk"
        for (int k = 0; k < (num / chunk); k++)
        {
            //writing the data for the consumer
            n = read(fd_named, C + (k * chunk), chunk);
            m = (k + 1) * chunk;
        }

        // in the end, the rest of the data is read
        n = read(fd_named, C + m, num % chunk);

        //stop-watch for wall time finishes
        clock_gettime(CLOCK_REALTIME, &end);
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed = seconds + nanoseconds * 1e-9;

        //stop-watch for wall time finishes
        finish = clock();
        float cpu_seconds = (float)(finish - start) / CLOCKS_PER_SEC;

        //printimg the calculation time
        printf("wall (passed) time in execution : %.5f\n", elapsed);
        printf("cpu time: %f\n", cpu_seconds);

        //close the pipe
        close(fd_named);
        //release the momory occupied with malloc
        free(C);
        return 0;
    }
}