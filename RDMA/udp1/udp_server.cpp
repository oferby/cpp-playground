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
#include "rdma_handler.h"

using namespace std;

#define MAX_EVENTS 10
#define PORT 8585



struct neighbor {
    sockaddr_in addr;
    app_dest dest;
    time_t lastHello;
};


class ConnectionServer {

private:

    map <string, neighbor> neighbor_map;

    int nfds, epollfd, status, sd;
    struct epoll_event ev, events[MAX_EVENTS];
    
    char *hello_msg;
    ssize_t msg_size;

    static char* get_hello_msg(struct app_dest *dest) {

        char *msg = (char*) malloc(sizeof "0000:000000:000000:00000000000000000000000000000000");

        char gid[33];
        gid_to_wire_gid(dest->gid, gid);
        sprintf(msg, "%04x:%06x:%06x:%s", dest->lid, dest->qpn,
                                dest->psn, gid);

        return msg;

    }

    static struct app_dest* get_dest(char *msg) {

        struct app_dest *rem_dest;
        memset(rem_dest, 0, sizeof rem_dest);

        char gid[33];
        sscanf(msg, "%x:%x:%x:%s", &rem_dest->lid, &rem_dest->qpn,
                                &rem_dest->psn, gid);
        free(msg);
        wire_gid_to_gid(gid, rem_dest->gid);

        return rem_dest;

    }


    
    void add_neighbor(struct sockaddr_in clientaddr) {

        string ip = inet_ntoa(clientaddr.sin_addr);
  
        if(neighbor_map.count(ip))
            puts("found in map");
        else
            puts("not found in map");

        if (neighbor_map.count(ip)  == 0 ) {
            puts("adding new addr");
            neighbor n = {
                .addr = clientaddr,
                .lastHello = time(nullptr)
            };
            neighbor_map[ip] = n;

        //     send_hello(clientaddr);
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

    void server_receive_event()
    {
        char recvbuf[65536] = { 0 }; 
        int len;
        struct sockaddr_in clientaddr;
        socklen_t client = sizeof(struct sockaddr);

        len = recvfrom(sd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&clientaddr, &client);
        if (len > 0) {
            char *ip = inet_ntoa(clientaddr.sin_addr);
            printf("got %i bytes from %s\n", len, ip);
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
            server_receive_event();
        }
        

    };

    void send_hello(char *dest) {

        printf("sending hello message to %s\n", dest);

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

        result = sendto(sd, this->hello_msg, this->msg_size, 0, (sockaddr*)&dest, sizeof(dest));
        if (result < 0) {
            perror("error sendin hellow message");
        } else {
            printf("%i bytes sent.\n", result);
        }
    }

    void set_hello_msg(struct app_dest *local_dest) {
        
        this->hello_msg = get_hello_msg(local_dest);
        this->msg_size = sizeof this->hello_msg;
        puts("hello message set");

    }


};
