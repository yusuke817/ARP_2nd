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

#define MAX_SIZE 10000000

int main(int argc, char* argv[]){

    int fd_named;
    time_t seconds0;
    time_t seconds1;
    
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

        printf("Producer!");

        int P[num];

        for(int i = 0; i < num; i++){

            P[i] = 1 + rand()%100;

        }

        fd_named = open("/tmp/named", O_WRONLY);

        // Stores time seconds

        time(&seconds0);
        
        printf("Time 0 : %ld\n", seconds0);

        for(int i = 0; i < num; i++){

            write(fd_named, &A[i], sizeof(A[i]));
        }
    }

    printf("Consumer!");
        
    fd_named = open("/tmp/named", O_RDONLY);

    int C[num];
	
                
    for(int i = 0; i < num; i++){

    	read(fd_named, &C[i], sizeof(C[i]));

    }

    // Stores time seconds

    time(&seconds1);
    
    printf("Time 1 : %ld\n", seconds1);
    close(fd_named);

    time_t tot = seconds1 - seconds0;

    printf("Time of execution : %ld\n", tot);
    
    wait(NULL);

    return 0;
}