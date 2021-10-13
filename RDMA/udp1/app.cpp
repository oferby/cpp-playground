#include "udp_server.cpp"
#include "rdma_handler.cpp"

int main(int argc, char* argv[]) {

    ConnectionServer conn_server;
    RdmaHandler rdmaHandler {};

    conn_server.start();

    if (argc > 1) {
        conn_server.send_hello(argv[1]);
        // exit(0);
    }

    while (1)
    {
        conn_server.handle_events();
    }


        

}
