#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <infiniband/verbs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
// #include <poll.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>

#define MAX_EVENTS 10

static ibv_device* get_ibv_device(char *dev_name) {

    struct ibv_device **dev_list = ibv_get_device_list(NULL);
    struct ibv_device *dev;

    if (!dev_list) {
        return NULL;
    }
    
    
    if (!dev_name) {
        dev = *dev_list;
        return dev;
    }

    while (*dev_list)
    {
        dev = *dev_list;
        if (!strcmp(dev->name,dev_name))
            return dev;
        dev_list++;
    }

    return NULL;

}


static int get_listening_socket(const char *servername, int port) {

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
    
    if (servername)
        simpleServer.sin_addr.s_addr = inet_addr(servername);
    else
        simpleServer.sin_addr.s_addr = INADDR_ANY;
    
    simpleServer.sin_port = htons(port);
   
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

    return sd;

}

static int use_conn_fd(int conn_sock) {

    puts("handling client socket event.");

    char buf[1024] = {0};
    
    int sizeOfData = read(conn_sock, &buf, 1024);
    if (sizeOfData == 0) {
        return -1;
    } 

    printf("got %d characters: %s.\n", sizeOfData, buf);

    char ack[] = "ACK!";
    auto status = write(conn_sock, &ack, sizeof(ack));
    if (status == -1) {
        perror("error wrinting to client socket\n");
    } else {
        printf("wrote to client socket %i bytes\n", status);
    }

    return 0;

}
 
static int setNonblocking(int fd) {
    int flags;

    /* If they have O_NONBLOCK, use the Posix way to do it */
    #if defined(O_NONBLOCK)
        /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
        if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
            flags = 0;
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    #else
        /* Otherwise, use the old way of doing it */
        flags = 1;
        return ioctl(fd, FIONBIO, &flags);
    #endif
} 

// use epoll for multi client connections handling
static void start_server(const char *servername, int port) {
    
    int listen_sock, new_sock, nfds, epollfd, status;
    struct epoll_event ev, events[MAX_EVENTS];

    listen_sock = get_listening_socket(servername, port);

    // create epoll fd
    epollfd = epoll_create1(0);
    if(epollfd<0) {
        puts("error creating epoll fd.");
        exit(1);

    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1 ) {
        perror("could not add listen socket to epoll.");
        exit(EXIT_FAILURE);
    }

    puts("listen socket added to epoll list.");

    while(1) {

        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll wait error!");
            exit(EXIT_FAILURE);
        }

        printf("got %i socket events.",nfds);

        for (int n = 0; n < nfds; ++n) {

            if(events[n].data.fd == listen_sock) {
                puts("got listen socker event.");
                struct sockaddr_in clientAddress = {0};
                int clientAddressLen;
                new_sock = accept(listen_sock, 
                    (struct sockaddr*) &clientAddress, 
                    (socklen_t *) &clientAddressLen);
                if (new_sock == -1) {
                    perror("error while connecting client socket.");
                    exit(EXIT_FAILURE);
                }                
                setNonblocking(new_sock);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = new_sock;
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, new_sock, &ev) == -1) {
                    perror("error adding client socket to epoll list.");
                    exit(EXIT_FAILURE);
                }
                puts("client socket added to epoll list.");

            } else {

                status = use_conn_fd(events[n].data.fd);
                if (status == -1) {
                    puts("client socket closed. removing from epoll list.");
                    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1 ) {
                        perror("error remoing client socket from epoll list.");
                        exit(EXIT_FAILURE);
                    }
                }

            }

        }


    }



}

static void start_client(const char *servername, int port) {

    char buf[1024] = {0};
    char hello[] = "Hello from client!";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock == -1) {
        fprintf(stderr, "error in create socket.\n");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(servername);

    int status = connect(sock, (const sockaddr * ) &serv_addr, sizeof(serv_addr));
    if (status == -1) {
        printf("error connecting to the server!\n");
        exit(1);
    }

    printf("connected.\n");
    

    printf("sending message to server...\n");
    send(sock, hello, sizeof(hello),0);

    printf("reading from server...\n");
    auto sizeOdData = read(sock, buf, sizeof(buf));
    printf("got from server: %s!\n", buf);

    sleep(10);

    close(sock);


}


static void usage()
{
	printf("Usage:\n");
	printf("test2 <options>\n");
	printf("Options:\n");
	printf("  -p, --port=<port>         listen on/connect to port <port> (default 8585)\n");
	printf("  -s, --server              start as a server\n");
    printf("  -i, --ip=<ip>             name or ip address of the server\n");
    printf("  -d, --dev=<dev-name>      RDMA device name\n");
	printf("  -m, --mtu=<size>          path MTU (default 1024)\n");
}


int main(int argc, char* argv[]) {

    char                    *servername = NULL;
	unsigned int             port = 8585;
    char                    *dev_name = NULL;
    bool                    is_server = false;

    struct ibv_device *ibv_dev;


	while (1) {
		int c;

		static struct option long_options[] = {
            { .name = "port",               .has_arg = 1, .flag = NULL, .val = 'p' },
			{ .name = "server",             .has_arg = 0, .flag = NULL, .val = 's' },
            { .name = "ip",                 .has_arg = 1, .flag = NULL, .val = 'i' },
            { .name = "dev",                .has_arg = 1, .flag = NULL, .val = 'd' },
            { .name = "mtu",                .has_arg = 1, .flag = NULL, .val = 'm' },
            {}
		};

        c = getopt_long(argc, argv, "p:scm:d:i:",
            long_options, NULL);

		if (c == -1) {
            break;
        }

		switch (c) {
            case 'p':
                port = strtoul(optarg, NULL, 0);
                if (port > 65535) {
                    usage();
                    return 1;
                }
                break;

            case 'i':
                servername = strdupa(optarg);
                break;
            
            case 's':
                is_server = true;
                break;
            
            case 'm':
                break;

            case 'd':
                dev_name = strdupa(optarg);
                break;

            default:
                usage();
                return 1;

        }


    };


    if (!servername && !is_server) {
        usage();
        return 1;
    }


    ibv_dev = get_ibv_device(dev_name);

    if (!ibv_dev) {
        printf("no RDMA device found!\n");
        return 1;
    }

    printf("using RDMA device %s\n", ibv_dev->name);

    if(is_server)
        start_server(servername, port);
    else
        start_client(servername, port);


}