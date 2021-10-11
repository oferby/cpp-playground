#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <string.h>

#define MAX_EVENTS 10
#define PORT 8585

class ConnectionServer {

private:
    int nfds, epollfd, status;
    struct epoll_event ev, events[MAX_EVENTS];

public:

    void start() {

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

        puts("Socket binded!");

        status = listen(sd, 3 );
        if ( status == -1 ) {
            fprintf(stderr, "Could not listen to socket!\n");
            exit(1);    
        }

        printf("Listening to socket!\n");

        // create epoll fd
        epollfd = epoll_create1(0);
        if(epollfd<0) {
            puts("error creating epoll fd.");
            exit(1);

        }

        ev.events = EPOLLIN;
        ev.data.fd = sd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sd, &ev) == -1 ) {
            perror("could not add listen socket to epoll.");
            exit(EXIT_FAILURE);
        }

        puts("listen socket added to epoll list.");


    };


    void handle_events() {

        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll wait error!");
            exit(EXIT_FAILURE);
        } else if (nfds > 0) {
            puts("got socket event.");
        }
        

    };

};
