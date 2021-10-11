#include "udp_server.cpp"

int main(int argc, char* argv[]) {

    ConnectionServer conn_server;

    conn_server.start();

    while (1)
    {
        conn_server.handle_events();
    }


        

}
