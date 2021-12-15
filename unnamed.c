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

int main(int argc, char* argv[]){

    int fd_w;
    int fd_r;
    time_t start;
    time_t end;

    int u[2];
  
    if (pipe(u) < 0){
        exit(1);
    }

    printf("Insert number of elements of the array\n");

    int num;

    scanf("%d", & num);

    if (num > 25000000){

        printf("Insert a number smaller than 25000000");

        scanf("%d", & num);

    }

    int id = fork();

    if (id != 0){

        printf("Producer!\n");

        int P[num];
        for(int i = 0; i < num; i++){

            P[i] = 1 + rand()%100;

        }
        fd_w = open(argv[1], O_WRONLY);

        time(&start);

        printf("Time 0 : %ld\n", start);

        write(fd_w, &start, sizeof(start));

        for(int i = 0; i < num; i++){

            write(u[1], &P[i], sizeof(P[i]));
        }
    }

    else{

        printf("Consumer!\n");

        fd_r = open(argv[2], O_WRONLY);

        int P[num];
                    
        for(int i = 0; i < num; i++){

            read(u[0], &P[i], sizeof(P[i]));

        }

        time(&end);

        printf("Time 1 : %ld\n", end);

        write(fd_r, &end, sizeof(end));

    }
    close(fd_w);
    close(fd_r);

    close(p[0]);
    close(p[1]);

    return 0;
}