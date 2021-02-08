#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>

#include <iostream>

using namespace std;

struct nl_sock *sk;


static int my_callback_func(struct nl_msg *mgs, void *arg) {

    cout << "message received" << endl;

    return 0;

}




int main() {

        
    sk = nl_socket_alloc();

    nl_socket_disable_seq_check(sk);

    nl_socket_modify_cb(sk, NL_CB_VALID, NL_CB_CUSTOM, my_callback_func, NULL);

    nl_connect(sk, NETLINK_ROUTE);

    nl_socket_add_membership(sk, RTNLGRP_LINK);


    while (1)
    {
        if (!nl_recvmsgs_default(sk)) {
            break;
        }
    }
    
    nl_socket_drop_membership(sk, RTNLGRP_LINK);

    nl_close(sk);

    nl_socket_free(sk);

    cout << "Done!" << endl;
}