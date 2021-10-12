#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <map>

using namespace std;

#define MAX_EVENTS 10
#define PORT 8585




struct neighbor {
    sockaddr_in addr;
    time_t lastHello;
};

// static int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr)
// {
//     int result;
//     addrinfo* result_list = NULL;
//     addrinfo hints = {};
//     hints.ai_family = family;
//     hints.ai_socktype = SOCK_DGRAM; 
//     result = getaddrinfo(hostname, service, &hints, &result_list);
//     if (result == 0)
//     {
//         memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
//         freeaddrinfo(result_list);
//     }

//     return result;
// }


class ConnectionServer {

private:
    int nfds, epollfd, status, sd;
    struct epoll_event ev, events[MAX_EVENTS];

    map<string, neighbor> neighbor_map {};

    void add_neighbor(struct sockaddr_in clientaddr) {

        char *ip = inet_ntoa(clientaddr.sin_addr);

        if (neighbor_map.find(ip) == neighbor_map.end()) {
            puts("adding new addr");
            neighbor n = {
                .addr = clientaddr,
                .lastHello = time(nullptr)
            };
            neighbor_map[ip] = n;

            send_hello(clientaddr);
        }
            
        else {
            printf("address %s exists. updating last hellow time.\n", ip);
            neighbor_map[ip].lastHello = time(nullptr);

        }

    }


public:



    void start() {

        sd = socket(AF_INET, SOCK_DGRAM, 0);

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

    void server_udp_handle()
    {
        char recvbuf[65536] = { 0 }; 
        int len;
        struct sockaddr_in clientaddr;
        socklen_t client = sizeof(struct sockaddr);

        len = recvfrom(sd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&clientaddr, &client);
        if (len > 0) {
            char *ip = inet_ntoa(clientaddr.sin_addr);
            printf("got %s from %s\n",recvbuf, ip);
            add_neighbor(clientaddr);
        }
    }

    void handle_events() {

        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll wait error!");
            exit(EXIT_FAILURE);
        } else if (nfds > 0) {
            puts("got socket event.");
            server_udp_handle();
        }
        

    };

    void send_hello(char* dest) {

        printf("sending hello message to %s\n",dest);

        sockaddr_in servaddr;
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(dest);
        servaddr.sin_port = htons(PORT);

        send_hello(servaddr);


    }

    void send_hello(sockaddr_in dest) {
    
        printf("sending hello to %s\n", inet_ntoa(dest.sin_addr));

        int result;
        const char* msg = "hello!";
        size_t msg_length = strlen(msg);

        result = sendto(sd, msg, msg_length, 0, (sockaddr*)&dest, sizeof(dest));
        if (result < 0) {
            perror("error sendin hellow message");
        } else {
            printf("%i bytes sent.\n", result);
        }
    }


};
