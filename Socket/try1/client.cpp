#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 

int PORT = 8989;

int main(int argc, char *argv[]) {

    char buf[1024] = {0};

    char hello[] = "Hello from client!";
    char buffer[1024] = {0};
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock == -1) {
        fprintf(stderr, "error in create socket.\n");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int status = connect(sock, (const sockaddr * ) &serv_addr, sizeof(serv_addr));
    if (status == -1) {
        printf("error connecting to the server!\n");
        exit(1);
    }

    printf("connected.\nsending message to server...\n");
    send(sock, hello, sizeof(hello),0);

    printf("reading from server...\n");
    auto sizeOdData = read(sock,buffer, sizeof(buffer));
    printf("got from server: %s!\n", buffer);

    close(sock);
    
    return 0;

}