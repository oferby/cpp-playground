#include <stdio.h>
#include "rdma_handler.cpp"
#include "udp_server.cpp"

int main(int argc, char* argv[]) {
    
    RdmaHandler rdmaHandler;

    ConnectionServer conn_server;    

    conn_server.start();
    conn_server.set_hello_msg(rdmaHandler.get_local_dest());

    if (argc > 1) {
        conn_server.send_hello(argv[1]);
        
    }

    while (1)
    {
        conn_server.handle_events();
    }


        

}
