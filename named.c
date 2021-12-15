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

#define MAX_SIZE 1250000

float seconds = 0.0;

int main(int argc, char* argv[]){

    int fd_named;
    time_t start;
    time_t end;
    
    mkfifo("/tmp/named", 0666);

    printf("Please input the number of elements of the array\n");

    int num;

    scanf("%d", & num);

    if (num > MAX_SIZE){

        printf("You can input less than 10000000");


    }

    int id = fork();

    if (id == -1){

        printf("Error \n");
        exit(1);
    }

    if (id == 0){

        //printf("Start");

        int P[num];

        for(int i = 0; i < num; i++){

            P[i] = 1 + rand()%num;

        }

        fd_named = open("/tmp/named", O_WRONLY);

        // Stores time seconds

        start = clock();

        for(int i = 0; i < num; i++){

            write(fd_named, &P[i], sizeof(P[i]));
        }
        
        
    }

    //printf("End");
        
    fd_named = open("/tmp/named", O_RDONLY);

    int C[num];
	
                
    for(int i = 0; i < num; i++){

    	read(fd_named, &C[i], sizeof(C[i]));

    }

    end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    
    close(fd_named);

    printf("Time of execution : %f\n", seconds);
    
    wait(NULL);

    return 0;
}
