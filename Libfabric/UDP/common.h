#pragma once

#include <rdma/fabric.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_errno.h>
#include <rdma/fi_rma.h>

#include <iostream>
#include <string.h>

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

void perrorx(const char *msg) {
    perror(msg);
    exit(1);
}

void
common_init(const char *addr, uint64_t flags) {

    int ret = fi_getinfo(FIVER, addr, "wormhole", flags, hints, &fi);
    if (ret) {
        perror("get_info");
        exit(1);
    }    

    // Fabric
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

    // Event Queue
    ret = fi_eq_open(fabric, &eq_attr, &eq, NULL);
    if (ret) 
        perrorx("fi_eq_open");

    // Domain
    ret = fi_domain(fabric, fi, &domain, NULL);
    if (ret) perrorx("fi_domain");

    struct fi_cq_attr cq_attr = {
        .size = 0, // default by provider
        .flags = 0,
        .format = FI_CQ_FORMAT_MSG,
        .wait_obj = FI_WAIT_UNSPEC,
        .signaling_vector = 0,
        .wait_cond = FI_CQ_COND_NONE,
        .wait_set = NULL,
    };

    // Compeltion Queue
    ret = fi_cq_open(domain, &cq_attr, &cq, NULL);
    if (ret) 
        perrorx("fi_cq_open");

    // Memory Region
    ret = fi_mr_reg(domain, buff, buff_size, 
        access, 0, 0, 0, &mr, NULL);
    if (ret) 
        perrorx("fi_mr_reg");

}