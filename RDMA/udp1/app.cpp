#include <stdio.h>
#include "rdma_handler.cpp"
#include "udp_server.cpp"

int main(int argc, char* argv[]) {
    
    ConnectionServer conn_server;    
    RdmaHandler rdmaHandler;

    app_dest *dest =  rdmaHandler.get_local_dest();
    conn_server.set_hello_msg(dest);
    conn_server.start();
   

    if (argc > 1) {
        conn_server.send_hello(argv[1]);
        
    }

    while (1)
    {
        conn_server.handle_events();
    }
        

}
