#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <malloc.h>
#include <getopt.h>
#include <time.h>
#include <infiniband/verbs.h>

#define PORT_NUM 1

struct app_context {
	struct ibv_context	*ctx;
	struct ibv_comp_channel *channel;
	struct ibv_pd		*pd;
	struct ibv_mr		*mr;
	struct ibv_cq		*cq;
	struct ibv_qp		*qp;
	struct ibv_ah		*ah;
	char			*buf;
	int			 size;
	int			 send_flags;
	int			 rx_depth;
	int			 pending;
	struct ibv_port_attr     portinfo;
};

struct app_dest {
	int lid;
	int qpn;
	int psn;
	union ibv_gid gid;
};

enum ibv_mtu mtu_to_enum(int mtu)
{
	switch (mtu) {
	case 256:  return IBV_MTU_256;
	case 512:  return IBV_MTU_512;
	case 1024: return IBV_MTU_1024;
	case 2048: return IBV_MTU_2048;
	case 4096: return IBV_MTU_4096;
	default:   throw std::invalid_argument("invalid mtu");
	}
}

static void wire_gid_to_gid(const char *wgid, union ibv_gid *gid) { 
	char tmp[9];
	__be32 v32;
	int i;
	uint32_t tmp_gid[4];

	for (tmp[8] = 0, i = 0; i < 4; ++i) {
		memcpy(tmp, wgid + i * 8, 8);
		sscanf(tmp, "%x", &v32);
		tmp_gid[i] = be32toh(v32);
	}
	memcpy(gid, tmp_gid, sizeof(*gid));
}

static void gid_to_wire_gid(const union ibv_gid *gid, char wgid[]) {
	uint32_t tmp_gid[4];
	int i;

	memcpy(tmp_gid, gid, sizeof(tmp_gid));
	for (i = 0; i < 4; ++i)
		sprintf(&wgid[i * 8], "%08x", htobe32(tmp_gid[i]));
}


static void cleanup(struct app_context *ctx) {
    if(ctx->qp)
	    ibv_destroy_qp(ctx->qp);

    if (ctx->cq)
	    ibv_destroy_cq(ctx->cq);
    if (ctx->mr)
	    ibv_dereg_mr(ctx->mr);

    if (ctx->pd)
	    ibv_dealloc_pd(ctx->pd);

    if (ctx->ctx)
	    ibv_close_device(ctx->ctx);

    if (ctx->buf)
	    free(ctx->buf);

	free(ctx);

}


static void createQueuePair(struct app_context *app_ctx) {

		struct ibv_qp_attr attr;
		struct ibv_qp_init_attr init_attr = {
			.send_cq = app_ctx->cq,
			.recv_cq = app_ctx->cq,
			.cap     = {
				.max_send_wr  = 10,
				.max_recv_wr  = 10,
				.max_send_sge = 1,
				.max_recv_sge = 1
			},
			.qp_type = IBV_QPT_UD,
		};

		app_ctx->qp = ibv_create_qp(app_ctx->pd, &init_attr);
		if (!app_ctx->qp)  {
			fprintf(stderr, "Couldn't create QP\n");
			cleanup(app_ctx);
            exit(EXIT_FAILURE);
		}

}


static void do_qp_change(struct ibv_qp* qp, struct ibv_qp_attr *attr, int state, char *mode) {

    auto status = ibv_modify_qp(qp, attr, state);

    if (status == 0)
        printf("QP changed to %s\n", mode);
    else if (status == EINVAL)
        printf("Invalid value provided in attr or in attr_mask for mode %s\n", mode);
    else if (status == ENOMEM)
        printf("Not enough resources to complete this operation for mode %s\n", mode);
    else
        printf("QP modify status: %i for mode %s\n",status, mode);

    if (status != 0)
        exit(EXIT_FAILURE);

}


static void changeQueuePairState(struct app_context *app_ctx) {

		struct ibv_qp_attr attr = {
			.qp_state        = IBV_QPS_INIT,
            .qkey            = 0x11111111,
			.pkey_index      = 0,
			.port_num        = PORT_NUM
		};

        int state = IBV_QP_STATE              |
				    IBV_QP_PKEY_INDEX         |
				    IBV_QP_PORT               |
				    IBV_QP_QKEY;

        do_qp_change(app_ctx->qp, &attr, state, (char*) "INIT");


        memset(&attr, 0, sizeof(attr));
        attr.qp_state = ibv_qp_state::IBV_QPS_RTR;

        do_qp_change(app_ctx->qp, &attr, IBV_QP_STATE, (char*) "RTR");

        memset(&attr, 0, sizeof(attr));
        attr.qp_state = ibv_qp_state::IBV_QPS_RTS;
        attr.sq_psn = 0;

        do_qp_change(app_ctx->qp, &attr, IBV_QP_STATE | IBV_QP_SQ_PSN, (char*) "RTS");

}


static struct app_context* setup_context() {

    struct app_context *app_ctx;
    struct ibv_device   **dev_list;
    struct ibv_device   *ib_dev;

    memset(app_ctx, 0, sizeof app_ctx);

    dev_list = ibv_get_device_list(NULL);

    if (!dev_list) {
        perror("error getting IB device list");
        exit(EXIT_FAILURE);
    }

    ib_dev = *dev_list;
    if (!ib_dev) {
        perror("device list empty");
        exit(EXIT_FAILURE);
    }

    printf("using dev name: %s\n", ib_dev->name);

    app_ctx->ctx = ibv_open_device(ib_dev);
    if (!app_ctx->ctx) {
        perror("error creating context");
        exit(EXIT_FAILURE);
    }

    ibv_free_device_list(dev_list);

    app_ctx->pd = ibv_alloc_pd(app_ctx->ctx);

    int cq_size = 0x10;
    app_ctx->cq = ibv_create_cq(app_ctx->ctx, cq_size, nullptr, nullptr, 0);

    createQueuePair(app_ctx);

    changeQueuePairState(app_ctx);

    return app_ctx;
}


// static int app_context_connect(struct app_context *ctx, int port, int my_psn,
//     int sl, struct app_dest *dest, int sgid_idx ) {

//         struct ibv_ah_attr ah_attr = {
//             .is_global  = 0,
//             .dlid       = dest->lid,
//             .sl         = sl,
//             .src_path_bits  = 0,
//             .port_num       = port
//         };




//     } 

class RdmaHandler {
    
    struct app_context *app_ctx;

public:
    RdmaHandler() {
        app_ctx = setup_context();
    }

};


