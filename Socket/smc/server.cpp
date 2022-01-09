#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 

int main(int argc, char *argv[]) {

    char buf[1024] = {0};

    if (3 != argc) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    
    in_addr_t ip = inet_addr(argv[1]);
    int portNum = atoi(argv[2]);

    int sd = socket(AF_SMC, SOCK_STREAM, 0);

    if ( sd == -1 ) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    } else if (sd == 898)
    {
        perror("found 898");
        exit(EXIT_FAILURE);
    }
    

    printf("Socket created!\n");
  
    struct sockaddr_in simpleServer;
    auto addresslen = sizeof(simpleServer);
    bzero(&simpleServer,addresslen);
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_addr.s_addr = ip;
    simpleServer.sin_port = htons(portNum);
   
    int status = bind(sd, (struct sockaddr *) &simpleServer, addresslen);
    if ( status != 0 ) {
        fprintf(stderr, "Could not bind socket!\n");
        exit(1);    
    }

    printf("Socket binded!\n");

    status = listen(sd, 3 );
    if ( status == -1 ) {
        fprintf(stderr, "Could not listen to socket!\n");
        exit(1);    
    }

    printf("Listening to socket!\n");

    while (true) {

        struct sockaddr_in clientAddress = {0};
        int clientAddressLen;
            
        int newsocket = accept(sd,(struct sockaddr *)&clientAddress,(socklen_t *) &clientAddressLen);
        if ( newsocket == -1 ) {
            fprintf(stderr, "Could not accept connection from socket!\n");
            exit(1);    
        }

        printf("connection accepted!\n");

        int sizeOfData = read(newsocket, &buf, 1024);

        printf("got %d characters: %s.\n", sizeOfData, buf);

        status = send(newsocket, &buf, sizeOfData, 0);
        if ( status == -1 ) {
            fprintf(stderr, "Could not send data from server!\n");
            exit(1);   
        }

        close(newsocket);

    };

    close(sd);

}