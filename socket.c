#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)
{

    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    //initialize each file descriptors
    int fd_w;
    int fd_r;
    //sockfd is a file descriptor which includes the values returned by socket system call
    //newsockfd is a file descriptor which includes the values returned by accept system call
    //portno receives the port number to connect the server
    //clilen receives the size of the address of the client
    int sockfd;
    int newsockfd;
    int portno;
    int clilen;
    int n;

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    struct hostent *server;

    time_t start;
    time_t end;

    printf("Please input the number of elements of the array\n");

    int num;

    scanf("%d", &num);

    if (argc < 1)
    {

        fprintf(stderr, "Error, no port provided\n");
        exit(1);
    }

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

    if (id == -1)
    {

        printf("Error forking...\n");
        exit(1);
    }

    start = clock();

    if (id != 0)
    {

        printf("success forking...\n");

        //int P[num];

        //for(int i = 0; i < num; i++){

        //P[i] = 1 + rand()%100;

        //}

        //fd_w = open(argv[1], O_WRONLY);

        //write(fd_w, &start, sizeof(start));

        //if (i>255)
        //j = i%255;
        //char buffer[j];
        //char buffer[256];

        char *buffer = (char *)malloc(num);

        //circular buffer infinite

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
            error("Error opening socket");

        bzero((char *)&serv_addr, sizeof(serv_addr));

        portno = 8080;

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            error("Error on binding");

        listen(sockfd, 5);

        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (newsockfd < 0)
            error("Error on accept");

        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        //printf("Here is the message: %s\n", buffer);

        n = write(newsockfd, "I got your message", 18);
        if (n < 0)
            error("ERROR writing to socket");

        free(buffer);
    }

    else
    {

        printf("success reading...\n");

        //fd_r = open(argv[2], O_WRONLY);

        //int C[num];

        //char buffer[256];
        char *buffer = (char *)malloc(num);

        if (argc < 1)
        {

            fprintf(stderr, "usage %s hostname port\n", argv[0]);
            exit(0);
        }
        //sleep(5);
        portno = 8080;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        printf("socket\n");

        if (sockfd < 0)
            error("ERROR opening socket");

        server = gethostbyname("127.0.0.1");

        //printf("%f", server);

        if (server == NULL)
        {
            printf("ERROR, no such host\n");
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }

        //printf("host\n");

        bzero((char *)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);

        //void bcopy(char *s1, char *s2, int length)

        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            error("ERROR connecting");

        //printf("Please enter the message: ");
        /*bzero(buffer, 256);
	fgets(buffer,255,stdin);
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0)
	error("ERROR writing to socket");
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0)
	error("ERROR reading from socket");*/

        for (int i = 0; i < num; i++)
        {

            //n = write(newsockfd,"I got your message",18);
            //read(sockfd, &C[i], sizeof(int));
        }

        printf("success ending...\n");
        end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;
        printf("Time of execution : %f\n", seconds);

        //write(fd_r, &end, sizeof(end));
        free(buffer);
    }

    //close(fd_w);
    //close(fd_r);

    //close(newsockfd);
    return 0;
}
