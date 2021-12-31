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

    // the producer cannot send the data at one time when the size of data is too big
    // so with chunk, I developped the program so that producer can send the data separately with small units
    int n, m;
    int chunk = 102400;

    // to measure the wall time which is the time passed in the execution, the structure is set
    struct timespec begin, end;

    //set the clock for CPU time calculation
    time_t start;
    time_t finish;

    // A sockaddr_in includes an internet address.
    // The variable serv_addr will include server address, and cli_addr will include client address
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    // The variable server is a pointer for a structure of type hostent.
    struct hostent *server;

    // user can decide the bytes of the data
    printf("Please input the number of elements of the array\n");

    int num;

    scanf("%d", &num);

    if (argc < 1)
    {

        fprintf(stderr, "Error, no port provided\n");
        exit(1);
    }
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

        printf("Error forking...\n");
        exit(1);
    }

        // Dynamic memory which is malloc is used here to accept big amount of data    
        char *buffer = (char *)malloc(num);

        // the array is filled with random data
        for (int j = 0; j < num; j++)
        {
            buffer[j] = 1 + rand() % 100;
        }

    //stop-watch for wall time begins
    clock_gettime(CLOCK_REALTIME, &begin);
            
    //stop-watch for CPU time begins
    start = clock();

    if (id != 0)
    {
        //parent process plays a role of producer P

        // The socket() system call creates a new socket. It takes three arguments: address domain of the socket, the type of socket and protocol.
        // Firstly, AF_INET is the Internet domain for two hosts on the Internet.
        // Secondly, SOCK_STREAM is a TCP socket
        // Thirdly,  when we set protocol as zero, system can choose the most proper protocol.

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
            error("Error opening socket");

        // Function bzero() initiates all the values in a buffer. It has two arguments which are pointer to the buffer and the size of the buffer.
        bzero((char *)&serv_addr, sizeof(serv_addr));

        // The port number requested by the server
        portno = 8080;

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        // The bind() system call binds a socket to an address which has three arguments: socket file descriptor, a pointer to a structure of
        // type sockaddr, the length of the address structure.

        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            error("Error on binding");

        // The listen system call enables the process to get the socket for connections. It has two arguments which are
        // socket file descriptor and maximum size of the queue of pending connections.
        listen(sockfd, 5);

        clilen = sizeof(cli_addr);

        // The accept() system call brings the process to block until a client connects to the server.
        // This has three arguments: socket file descriptor, a pointer to the address of the client, the length of this structure.
        // This system call makes the process when a connection from a client has been achieved without problems.
        // This returns a new file descriptor, and all communication on this connection should be done using the new file descriptor. The

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

        if (newsockfd < 0)
            error("Error on accept");

        //sleep(1);

        // Except the end of the data, the data is transmitted in the fixed unit which is "chunk"
        for (int k = 0; k < (num / chunk) - 1; k++)
        {
            //writing the data for the consumer
            n = write(newsockfd, buffer + (k * chunk), chunk);
            m = (k + 1) * chunk;
            if (n < 0)
                error("ERROR writing to socket");
        }

        // in the end, the rest of the data is transmitted
        n = write(newsockfd, buffer + m, num % chunk);

        wait(NULL);
        close(sockfd);
        free(buffer);
    }

    else
    {
        // child process plays a role of consumer C
        // Dynamic memory which is malloc is used here to accept big amount of data
        char *buffer = (char *)malloc(num);

        if (argc < 1)
        {

            fprintf(stderr, "usage %s hostname port\n", argv[0]);
            exit(0);
        }

        // Client should know the host the server is running which is 127.0.0.1 here and the port number the server is listening.

        portno = 8080;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
            error("ERROR opening socket");

        // gethostbyname() function return the hostent structure or a null pointer if an error occurs.
        server = gethostbyname("127.0.0.1");

        if (server == NULL)
        {
            printf("ERROR, no such host\n");
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }

        bzero((char *)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);

        // The connect function is called by the client to build a connection to the server. It has three arguments:
        // the socket file descriptor, the address of the host to be connected with the port number and the size of this address.
        // This function returns 0 when it successes and -1 when it fails.
        while (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            usleep(1);
        }

        // Except the end of the data, the data is read in the fixed unit which is "chunk"
        for (int k = 0; k < (num / chunk) - 1; k++)
        {
            n = read(sockfd, buffer + (k * chunk), chunk);
            m = (k + 1) * chunk;
            if (n < 0)
                error("ERROR reading from socket");
        }

        // in the end, the rest of the data is read
        n = read(sockfd, buffer + m, num % chunk);

        close(sockfd);

        //stop-watch for wall time finishes
        clock_gettime(CLOCK_REALTIME, &end);
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed = seconds+ nanoseconds * 1e-9;

        //stop-watch for wall time finishes
        finish = clock();
        float cpu_seconds = (float)(finish - start) / CLOCKS_PER_SEC;

        //printimg the calculation time
        printf("wall (passed) time in execution : %.5f\n", elapsed);
        printf("cpu time: %f\n", cpu_seconds);

        //release the momory occupied with malloc
        free(buffer);
    }

    return 0;
}
