#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <malloc.h>
#include <getopt.h>
#include <time.h>
#include <arpa/inet.h>
#include <map>
#include <vector>

#include "rdma_handler.h"

using namespace std;

#define PORT_NUM 1
#define QKEY 0x11111111
#define GID_IDX 0
#define IB_PORT 1
#define MSG_SIZE 1500
#define CQ_SIZE 10
#define MAX_WR 10
#define MAX_SGE 1




class RdmaHandler {
    
private:

    app_context app_ctx;
    app_dest *local_dest;
    map <uint64_t,ibv_sge*> sge_map;
    vector<ibv_sge*> available_send_sge_vector;

    int status;
    ibv_wc wc;
    

    static void cleanup(app_context *ctx) {
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


    static void createQueuePair(app_context *app_ctx) {

		struct ibv_qp_attr attr;
		struct ibv_qp_init_attr init_attr = {
			.send_cq = app_ctx->cq,
			.recv_cq = app_ctx->cq,
			.cap     = {
				.max_send_wr  = MAX_WR,
				.max_recv_wr  = MAX_WR,
				.max_send_sge = MAX_SGE,
				.max_recv_sge = MAX_SGE,
                
			},
			.qp_type = IBV_QPT_UD,
            .sq_sig_all = 1
		};

		app_ctx->qp = ibv_create_qp(app_ctx->pd, &init_attr);
		if (!app_ctx->qp)  {
			fprintf(stderr, "Couldn't create QP\n");
			cleanup(app_ctx);
            exit(EXIT_FAILURE);
		}

    }


    static void do_qp_change(ibv_qp* qp, ibv_qp_attr *attr, int state, char *mode) {

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


    static void setup_memory(app_context *app_ctx) {
        
        puts("setting up memory.");

        int mr_size =  ( MSG_SIZE + 40 ) * MAX_WR;
        int alignment = sysconf(_SC_PAGESIZE);
        app_ctx->buf = (char*) memalign(alignment, mr_size);

        if (!app_ctx->buf) {
            perror("error creating memory buffer.");
            cleanup(app_ctx);
            exit(EXIT_FAILURE);
        }

        app_ctx->mr = ibv_reg_mr(app_ctx->pd, app_ctx->buf, mr_size, IBV_ACCESS_LOCAL_WRITE);
        if (!app_ctx->mr) {
            perror("error registering memory");
            cleanup(app_ctx);
            exit(EXIT_FAILURE);
        }

        uint32_t msg_size = MSG_SIZE + 40;

        uint64_t mem_addr = (uintptr_t) app_ctx->buf;

        for (int i = 0; i < MAX_WR; i++) {
                
            mem_addr += msg_size;

            ibv_sge *sge = (ibv_sge*) malloc(sizeof sge);
            sge->addr = mem_addr;
            sge->length = msg_size;
            sge->lkey = app_ctx->mr->lkey;

            ibv_recv_wr rec_wr = {
                .wr_id = app_ctx->wid++,
                .sg_list = sge,
                .num_sge = 1, 
            };

            ibv_recv_wr *bad_wr;

            if (ibv_post_recv(app_ctx->qp, &rec_wr, &bad_wr)) {
                perror("error posting RR.");
                cleanup(app_ctx);
                exit(EXIT_FAILURE);    
            } 

            app_ctx->sge_map->insert(make_pair(rec_wr.wr_id,sge));

        }

        for (int i = 0; i < MAX_WR; i++) {
                
            mem_addr += msg_size;

            ibv_sge *sge = (ibv_sge*) malloc(sizeof sge);
            sge->addr = mem_addr;
            sge->length = msg_size;
            sge->lkey = app_ctx->mr->lkey;

            app_ctx->available_send_sge_vector->push_back(sge);

        }


        
        puts("memory and WRs added.");

    }

    static void changeQueuePairState(app_context *app_ctx) {

		struct ibv_qp_attr attr = {
			.qp_state        = IBV_QPS_INIT,
            .qkey            = QKEY,
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

        puts("QP ready.");

    }

    static void setup_context(app_context *app_ctx) {

        ibv_device   **dev_list;
        ibv_device   *ib_dev;
        int status;


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

        ibv_port_attr port_attr;
        status = ibv_query_port(app_ctx->ctx, IB_PORT, &port_attr);
        if (status == -1) {
            perror("could not get port info");
            exit(EXIT_FAILURE);
        }
        app_ctx->portinfo = &port_attr;

        app_ctx->pd = ibv_alloc_pd(app_ctx->ctx);

        app_ctx->cq = ibv_create_cq(app_ctx->ctx, CQ_SIZE, nullptr, nullptr, 0);

        createQueuePair(app_ctx);

        changeQueuePairState(app_ctx);

        setup_memory(app_ctx);       

    }

    void handle_rr() {
        
        printf("WC: received %i\n",wc.byte_len);

        if (wc.wc_flags && IBV_WC_GRH) {
            puts("GRH exists in payload.");

        }




    }

    void handle_sr(){

        printf("WC: sent %i bytes\n",wc.byte_len);

    }

    void handle_wc() {

        puts("handling WC.");

        switch (wc.opcode) {

            case IBV_WC_SEND :
                handle_sr();
                break;

            case IBV_WC_RECV :
                handle_rr();
                break;

            default : 
                puts("got wrong WC opcode");
                break;

        }


    }

public:
    RdmaHandler() {
        
        memset(&app_ctx, 0, sizeof app_ctx);
        app_ctx.sge_map = &sge_map;
        app_ctx.available_send_sge_vector = &available_send_sge_vector;
        setup_context(&app_ctx);
        
        int status;
        
        local_dest = (app_dest*) malloc(sizeof local_dest);
        memset(local_dest, 0, sizeof local_dest);
        local_dest->lid = app_ctx.portinfo->lid;
        local_dest->qpn = app_ctx.qp->qp_num;
        local_dest->psn = 1;
        
        local_dest->gid = (ibv_gid*) malloc(sizeof local_dest->gid);

        status = ibv_query_gid(app_ctx.ctx, IB_PORT, GID_IDX, local_dest->gid);
        if (status == -1) {
            perror("could not get GID");
            exit(EXIT_FAILURE);
        }

        print_dest(local_dest);
        

    }

    app_dest* get_local_dest() {
        return local_dest;
    }
    
    app_context* get_app_context() {
        return &app_ctx;
    }

    void poll_complition() {

            status = ibv_poll_cq(app_ctx.cq, 1, &wc);
            if (status < 0) {
                perror("error getting WC.");
                return;

            }

            if( status > 0 && wc.status == ibv_wc_status::IBV_WC_SUCCESS) 
                handle_wc();

    }


    void create_send_request(const char *data, size_t len, app_dest *dest) {

        if (available_send_sge_vector.empty()) {
            puts("there is no SGE available to SR.");
            return;
        }

        puts("creating send request");

        ibv_sge *sge = available_send_sge_vector.back();
        available_send_sge_vector.pop_back();

        ibv_send_wr *send_wr = new ibv_send_wr();
        send_wr->wr_id = app_ctx.wid++,
        send_wr->sg_list = sge;
        send_wr->num_sge = 1;
        send_wr->opcode = IBV_WR_SEND;
        send_wr->wr.ud.ah = dest->ah;

        ibv_send_wr *bad_wr;

        // status = ibv_post_send(app_ctx.qp, send_wr, &bad_wr);

    }

};


