#include <map>
#include <string>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <iostream>

using namespace std;

struct S {
    int i;
};

struct app_dest {
	uint16_t lid;
	int qpn;
	int psn;
	union ibv_gid *gid;
};

struct neighbor {
    sockaddr_in addr;
    app_dest dest;
    time_t lastHello;
};

int main(int argc, char *argv[]) {

    S s = {
        .i = 3
    };

    neighbor n = {
        .lastHello = time(nullptr)
    };

    map <std::string, neighbor > my_map;

    if (my_map.count("A")) {
        puts("has");
    } 
    else
        puts("has not");

    my_map.insert({"A", n});

    if (my_map.count("A")) {
        puts("has");
    } 
    else
        puts("has not");
}