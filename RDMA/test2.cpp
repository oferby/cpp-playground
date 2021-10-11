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


static void start_server(const char *servername, int port) {

    char buf[1024] = {0};

    int sd = socket(AF_INET, SOCK_STREAM, 0);

    if ( sd == -1 ) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    }

    printf("Socket created!\n");
  
    struct sockaddr_in simpleServer;
    auto addresslen = sizeof(simpleServer);
    bzero(&simpleServ#if __BYTE_ORDER == __LITTLE_ENDIAN
    static inline uint64_t htonll(uint64_t x) { return bswap_64(x); }
    static inline uint64_t ntohll(uint64_t x) { return bswap_64(x); }
#elif __BYTE_ORDER == __BIG_ENDIAN
    static inline uint64_t htonll(uint64_t x) { return x; }
    static inline uint64_t ntohll(uint64_t x) { return x; }
#else
    #error __BYTE_ORDER is neither __LITTLE_ENDIAN nor __BIG_ENDIAN
#endif

/* structure of test parameters */
struct config_t 
{
    const char *dev_name; /* IB device name */
    char *server_name; /* server host name */
    u_int32_t tcp_port; /* server TCP port */
    int ib_port; /* local IB port to work with */
    int gid_idx; /* gid index to use */
};

/* structure to exchange data which is needed to connect the QPs */
struct cm_con_data_t 
{
    uint64_t addr; /* Buffer address */
    uint32_t rkey; /* Remote key */
    uint32_t qp_num; /* QP number */
    uint16_t lid; /* LID of the IB port */
    uint8_t gid[16]; /* gid */
} __attribute__ ((packed));

/* structure of system resources */
struct resources 
{
    struct ibv_device_attr  
    device_attr;
    /* Device attributes */
    struct ibv_port_attr port_attr; /* IB port attributes */
    struct cm_con_data_t remote_props; /* values to connect to remote side */
    struct ibv_context *ib_ctx; /* device handle */
    struct ibv_pd *pd; /* PD handle */
    struct ibv_cq *cq; /* CQ handle */
    struct ibv_qp *qp; /* QP handle */
    struct ibv_mr *mr; /* MR handle for buf */
    char *buf; /* memory buffer pointer, used for RDMA and send ops */
    int sock; /* TCP socket file descriptor */
};

struct config_t config = 
{
    NULL,   /* dev_name */
    NULL,   /* server_name */
    19875,  /* tcp_port */
    1,      /* ib_port */
    -1      /* gid_idx */
};port = htons(port);
   
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
    

    // printf("sending message to server...\n");
    // send(sock, hello, sizeof(hello),0);

    // printf("reading from server...\n");
    // auto sizeOdData = read(sock,buffer, sizeof(buffer));
    // printf("got from server: %s!\n", buffer);

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