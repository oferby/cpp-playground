#include <rdma/fabric.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_errno.h>
#include <rdma/fi_rma.h>

#include <iostream>
#include <string.h>

using namespace std;

#define	FIVER FI_VERSION(1, 1)

struct fi_info *fi, *hints;
struct fid_fabric *fabric;
struct fid_eq *eq;
struct fid_cq *cq;
struct fid_domain *domain;

struct fid_mr *mr;
int access = FI_REMOTE_WRITE | FI_REMOTE_READ | FI_SEND | FI_RECV;

struct fid_pep *pep;
struct fid_ep *ep;

void *buff;
size_t buff_size = 32 * 1024 * 1024;

struct keys {
	uint64_t rkey;
	uint64_t addr;
};

struct ctx {
	int ready;
	int count;
	int size;
};

struct ctx *ctx;
struct keys keys;


void
common_init(const char *addr, uint64_t flags) {

    int ret = fi_getinfo(FIVER, addr, "wormhole", flags, hints, &fi);
    if (ret) {
        perror("get_info");
        exit(1);
    }    

    ret = fi_fabric(fi->fabric_attr, &fabric, NULL);
    if (ret) {
        perror("fi_fabric");
        exit(1);
    }

    struct fi_eq_attr eq_attr = {
        .size = 0,
        .flags = 0,
        .wait_obj = FI_WAIT_UNSPEC,
        .signaling_vector = 0,
        .wait_set = NULL,
    };

    ret = fi_eq_open(fabric, &eq_attr, &eq, NULL);
    if (ret) {
        perror("fi_eq_open");
        exit(1);
    }

    ret = fi_domain(fabric, fi, &domain, NULL);
    if (ret) {
        perror("fi_domain");
        exit(1);
    }

    struct fi_cq_attr cq_attr = {
        .size = 0, // default by provider
        .flags = 0,
        .format = FI_CQ_FORMAT_MSG,
        .wait_obj = FI_WAIT_UNSPEC,
        .signaling_vector = 0,
        .wait_cond = FI_CQ_COND_NONE,
        .wait_set = NULL,
    };

    ret = fi_cq_open(domain, &cq_attr, &cq, NULL);
    if (ret) {
        perror("fi_cq_open");
        exit(1);
    }

    ret = fi_mr_reg(domain, buff, buff_size, 
        access, 0, 0, 0, &mr, NULL);
    if (ret) {
        perror("fi_mr_reg");
        exit(1);
    }

    


}

void
server_init() {

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