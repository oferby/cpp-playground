#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <iostream>

using namespace std;

int PORT = 8989;

int main(int argc, char *argv[]) {

    char buf[1024] = {0};

    /* make sure we have a port number
    */
    // if (2 != argc) {
    //     fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    //     exit(1);
    // }

    // int portNum = atoi(argv[1]);

    int sd = socket(AF_INET, SOCK_STREAM, 0);

    if ( sd == -1 ) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    }

    printf("Socket created!\n");

    

    struct sockaddr_in simpleServer;
    auto addresslen = sizeof(simpleServer);
    bzero(&simpleServer,addresslen);
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_addr.s_addr = INADDR_ANY;
    simpleServer.sin_port = htons(PORT);
   
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

    struct sockaddr_in clientAddress = {0};
    int clientAddressLen;
        
    int newsocket = accept(sd,(struct sockaddr *)&clientAddress,(socklen_t *) &clientAddressLen);
    if ( newsocket == -1 ) {
        fprintf(stderr, "Could not accept connection from socket!\n");
        exit(1);    
    }

    printf("connection accepted!\n");

    int sizeOfData = read(newsocket, &buf, 1024);

    printf("got %d characters.\n", sizeOfData);

    status = send(newsocket, &buf, sizeOfData, 0);
    if ( status == -1 ) {
        fprintf(stderr, "Could not send data from server!\n");
        exit(1);   
    }


}