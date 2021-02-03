#include <rdma/fabric.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>

// may need this too
#include <rdma/fi_cm.h>
#include <rdma/fi_tagged.h>
#include <rdma/fi_rma.h>
#include <rdma/fi_errno.h>

#include <iostream>

using namespace std;

struct fi_info *hints, *fi;

int main() {

    int err = fi_getinfo(FI_VERSION(1,11), NULL, NULL, 0, NULL, &fi);
    
    if (err) {
        cout << "got error: " << err << "\n";

    }

    // find provider with hints on what we are looking for

    // first allocate memory
    hints = fi_allocinfo();

    hints->ep_attr->type = FI_EP_RDM;
    hints->caps = FI_MSG | FI_TAGGED | FI_RMA;
    hints->mode = FI_CONTEXT;
    hints->fabric_attr->prov_name = strdup("psm2");

    // err = fi_getinfo(FI_VERSION(1,11), NULL, NULL, 0, hints, &fi);

    // free info memory
    fi_freeinfo(hints);

    // **** creating objects needed for communication *****
    
    // ** create fabric object **
    struct fid_fabric *fabric;
    err = fi_fabric(fi->fabric_attr, &fabric, NULL);
    if (err) {
        cout << "got error while creating fabric object\n";
        exit(1); 
    }

    // create event queue (EQ), used to collect async events
    struct fi_eq_attr eq_attr;
    struct fid_eq *eq;

    memset(&eq_attr, 0, sizeof(eq_attr));
    eq_attr.size = 64;
    eq_attr.wait_obj = FI_WAIT_UNSPEC;   
    err = fi_eq_open(fabric, &eq_attr, &eq, NULL);
    if (err) {
        cout << "got error while creating event queue object\n";
        exit(1); 
    }

    // ** create domain object **
    // used as a layer of abstraction for physical resources

    struct fid_domain *domain;
    err = fi_domain(fabric, fi, &domain, NULL);
    if (err) {
        cout << "got error while creating domain object\n";
        exit(1); 
    }

    // create endpoint (EP) to be used for communication
    struct fid_ep *ep;
    err = fi_endpoint(domain, fi, &ep, NULL);
    if (err) {
        cout << "got error while creating endpoint object\n";
        exit(1); 
    }    

    // create address vector (AV) bind to endpoint and event queue
    // for connectionless - specify the address of the remote endpoint. 
    // This needs translation by address vector
    struct fi_av_attr av_attr;
    struct fid_av *av;

    memset(&av_attr, 0, sizeof(av_attr));
    av_attr.type = FI_AV_MAP;
    err = fi_av_open(domain, &av_attr, &av, NULL);
    if (err) {
        cout << "got error while creating address vector\n";
        exit(1); 
    }    

    // bind the address vector to the endpoint
    // completion queue is used to report completions and errors 
    // of communication operations.
    err = fi_ep_bind(ep, (fid_t)av, 0);
    err = fi_av_bind(av, (fid_t)eq, 0);

    // create completion queue and bind to endpoint
    struct fi_cq_attr cq_attr;
    struct fid_cq *cq;

    memset(&cq_attr, 0, sizeof(cq_attr));
    cq_attr.format = FI_CQ_FORMAT_TAGGED;
    cq_attr.size = 1000;
    err = fi_cq_open(domain, &cq_attr, &cq, NULL);

    // bind completion queue to endpoint
    err = fi_ep_bind(ep, (fid_t)cq, FI_TRANSMIT|FI_RECV);

    // we can use counter for completion instead of queue
    struct fi_cntr_attr cntr_attr;
    struct fid_cntr * cntr;
    
    memset(&cntr_attr, 0, sizeof(cntr_attr));

    err = fi_cntr_open(domain, &cntr_attr, &cntr, NULL);

    // counter can be bound to endpoint
    err = fi_ep_bind(ep, (fid_t)cntr, FI_SEND | FI_RECV | FI_READ | FI_WRITE);

    // create memory resion for remote memory access (RMA)
    const size_t len = 10;
    uint64_t requested_key;
    const char *buf[10]; // ??
    struct fid_mr *mr;
    err = fi_mr_reg(domain, buf, len, FI_REMOTE_READ | FI_REMOTE_WRITE, 0,
         requested_key, 0, &mr, NULL);

    // local descriptor
    void *desc = fi_mr_desc(mr);
    // remote access key
    uint64_t rkey = fi_mr_key(mr);

    // counter can be bound to a memory region to count incoming remote writes
    // to the memory region
    struct fid_cntr *mr_cntr;
    err =  fi_mr_bind(mr, (fid_t)mr_cntr, FI_REMOTE_WRITE);

    // enable the endpoint
    err = fi_enable(ep);

    // get endpoint address format
    bool _ = fi->addr_format == FI_SOCKADDR;

    // get endpoint address
    void *addr;
    size_t addrlen = 0;
    fi_getname((fid_t)ep,addr,&addrlen);

    addr = malloc(addrlen);
    err = fi_getname((fid_t)ep, addr,&addrlen);

    struct fi_av_attr *av_attr;
    struct fid_av *av;

    cout << "Done!\n"; 

}

