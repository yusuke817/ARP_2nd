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

#define MAX_SIZE 25000000

float seconds = 0.0;

int main(int argc, char* argv[]){

    int fd_w;
    int fd_r;
    time_t start;
    time_t end;

    int u[2];
  
    if (pipe(u) < 0){
        exit(1);
    }

    printf("Please input the number of elements of the array\n");

    int num;

    scanf("%d", & num);

    if (num > MAX_SIZE){

        printf("Insert a number smaller than 1250000\n");

        scanf("%d", & num);

    }

    int id = fork();

    if (id != 0){

        int P[num];
        for(int i = 0; i < num; i++){

            P[i] = 1 + rand()%num;

        }
        fd_w = open(argv[1], O_WRONLY);
        
        start = clock();

        write(fd_w, &start, sizeof(start));
        
        
        for(int i = 0; i < num; i++){

            write(u[1], &P[i], sizeof(P[i]));
        }
    }

    else{

        fd_r = open(argv[2], O_RDONLY);

        int C[num];
                    
        for(int i = 0; i < num; i++){

            read(u[0], &C[i], sizeof(C[i]));

        }
                
    end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Time of execution : %f\n", seconds);    

        write(fd_r, &end, sizeof(end));
        
    }
    close(fd_w);
    close(fd_r);
    
    
    close(u[0]);
    close(u[1]);
    
    //wait(NULL);
    
    return 0;
}
