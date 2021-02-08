#include "common.h"

using namespace std;


void
server_init() {
    
    int ret;

    keys.rkey = fi_mr_key(mr);
    
    // Passive Endpoint
    ret = fi_passive_ep(fabric, fi, &pep, NULL);
    if (ret) 
        perrorx("fi_passive_ep");

    // Associate a passive endpoint with an event queue
    if (fi_pep_bind(pep, &eq->fid, 0)) perrorx("fi_pep_bind");

    if (fi_listen(pep)) perrorx("fi_listen");

    
    

    
}

void
run_server() {

    hints = fi_allocinfo();
	if (!hints) {
		perror("fi_allocinfo");
		
	}

	hints->addr_format = FI_SOCKADDR_IN;
	hints->ep_attr->type = FI_EP_MSG;
	hints->domain_attr->mr_mode = FI_MR_BASIC;
	hints->caps = FI_MSG | FI_RMA;
	hints->mode = FI_CONTEXT | FI_LOCAL_MR | FI_RX_CQ_DATA;

    common_init("10.10.0.120", FI_SOURCE);

    server_init();

}


int main() {

    buff = malloc(buff_size);
    if (!buff) {
        cout << "error alocating buffer memory" << endl;
        exit(1);
    }

    run_server();

}