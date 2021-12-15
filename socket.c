#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <sys/wait.h>
#include <time.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg){

    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]){

    int fd_np;
    int fd_w;
    int fd_r;
    int sockfd;
    int newsockfd;
    int portno;
    int clilen;

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr; 

    struct hostent *server;

    time_t start;
    time_t end;

    printf("Insert number of elements of the array\n");

    int num;

    scanf("%d", & num);

    if (num > 25000000){

        printf("Insert a number smaller than 25000000");

        scanf("%d", & num);
    }

    int id = fork();

    if (id == -1){

        printf("Error forking...\n");
        exit(1);
    }

    if (id != 0){

        printf("Server!\n");

        int P[num];

        for(int i = 0; i < num; i++){

            P[i] = 1 + rand()%100;

        }

        fd_w = open(argv[1], O_WRONLY);

        time(&start);

        printf("Time 0 : %ld\n", start);

        write(fd_w, &start, sizeof(start));

        if(argc < 2){

            fprintf(stderr, "Error, no port provided\n");
            exit(1);
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if(sockfd < 0)
            error("Error opening socket");

        portno = atoi(argv[1]);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error("Error on binding");

        listen(sockfd, 5);

        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0)
            error("Error on accept");

        for(int i = 0; i < num; i++){

            write(newsockfd, &P[i], sizeof(int));
        }
    }

    else{

        printf("Client!\n");

        fd_r = open(argv[2], O_WRONLY);

        int C[num];

        if (argc < 3) {

            fprintf(stderr,"usage %s hostname port\n", argv[0]);
            exit(0);
        }

        portno = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) 
            error("ERROR opening socket");

        server = gethostbyname(argv[1]);

        if (server == NULL) {

            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }

        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

        serv_addr.sin_port = htons(portno);

        bcopy( char *s1, char *s2, int length);

        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
            error("ERROR connecting");

        for(int i = 0; i < num; i++){

            read(sockfd, &C[i], sizeof(int));
        }

        time(&end);

        printf("Time 1 : %ld\n", end);

        write(fd_r, &end, sizeof(end));
    }

    close(fd_w);
    close(fd_r);

    close(newsockfd);

    return 0;
}