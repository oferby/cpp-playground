#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <infiniband/verbs.h>


int main(int argc, char* argv[]) {

    char        *server_name;
    u_int32_t   tcp_port;
    int         sock;

    struct ibv_device   **dev_list;
    struct ibv_device   *ib_dev;
    struct ibv_context  *ctx;
    struct ibv_device_attr *ib_device_attr; 
    struct ibv_port_attr port_attr;

    struct ibv_cq *cq; // complition queue
 
    struct ibv_pd *pd; // protection domain
    struct ibv_mr *mr; // mamory region

    struct ibv_qp *qp;    // queue pair
    struct ibv_qp_init_attr qp_init_attr;

    size_t b_size = 1024;
    char *buffer;

    // struct for data exchange
    struct data_info {

        uint64_t    addr; // buffer address
        u_int32_t   rkey; 
        u_int32_t   qp_number;
        uint16_t    lid;    // local ID
        uint8_t     gid;

    };   


    dev_list = ibv_get_device_list(NULL);

    if (!dev_list) {
        perror("error getting IB device list");
        return 1;
    }

    ib_dev = *dev_list;
    if (!ib_dev) {
        perror("device list empty");
        return 1;
    }

    printf("dev name: %s\n", ib_dev->name);
    printf("node type: %s \n", ibv_node_type_str(ib_dev->node_type));
    
    ctx = ibv_open_device(ib_dev);
    if (!ctx) {
        perror("error creating context");
        return 1;
    }

    ibv_free_device_list(dev_list);


    ib_device_attr = (struct ibv_device_attr *) malloc(sizeof (struct ibv_device_attr));
    if (ibv_query_device(ctx, ib_device_attr) == -1) {
        perror("could not get device attributes");
        return 1;
    }
    
    printf("number of ports in the device: %i\n", ib_device_attr->phys_port_cnt);


    ibv_query_port(ctx, 1, &port_attr);
    printf("port lid: %i\n",port_attr.lid);
    printf("port status: %s\n", ibv_port_state_str(port_attr.state));


    pd = ibv_alloc_pd(ctx);
    if (!pd) {
        perror("error allocating Protection Domain (PD)");
        return 1;
    }

    printf("Protection Domain (PD) allocated.\n");

    printf("number of complition vectors: %i\n", ctx->num_comp_vectors);
    cq = ibv_create_cq(ctx, 10, NULL, NULL, 0);

    if (!cq) {
        perror("could not create Complition Queue (CQ).");
        return 1;
    }

    printf("CQ allocated.\n");

    buffer = (char*) malloc(b_size);

    mr = ibv_reg_mr(pd, buffer, b_size, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);

    if (!mr) {
        perror("could not register memory region (MR).\n");
        return 1;
    }

    printf("memory region (MR) registered. Local Key: %i\n", mr->lkey);


    // *******************************************************************************************

    memset(&qp_init_attr, 0, sizeof(qp_init_attr));
    qp_init_attr.send_cq = cq;
    qp_init_attr.recv_cq = cq;
    qp_init_attr.qp_type = IBV_QPT_RC;
    qp_init_attr.cap.max_send_wr  = 5;
    qp_init_attr.cap.max_recv_wr  = 5;
    qp_init_attr.cap.max_send_sge = 1;
    qp_init_attr.cap.max_recv_sge = 1;

    qp = ibv_create_qp(pd, &qp_init_attr);

    if (!qp) {
        perror("could not create queue pair (QP).\n");
        return 1;
    }

    puts("queue pair (QP) created.\n");

    // *************************************  INIT   *****************************************

    struct ibv_qp_attr attr;
    int flags;
    int rc;

    memset(&attr, 0, sizeof(attr));
    attr.qp_state = ibv_qp_state::IBV_QPS_INIT;
    attr.port_num = 1;
    attr.pkey_index = 0;
    attr.qp_access_flags = 0;

    rc = ibv_modify_qp(qp, &attr, 
		  IBV_QP_STATE      |
		  IBV_QP_PKEY_INDEX |
		  IBV_QP_PORT       |
		  IBV_QP_ACCESS_FLAGS);
    if (rc)
        puts("error changing QP state to INIT");
    else
        puts("QP state changed to INIT");

    // *****************************************   RTR   ********************************************

    memset(&attr, 0, sizeof(attr));
    
    attr.qp_state		= ibv_qp_state::IBV_QPS_RTR;
    attr.path_mtu		= ibv_mtu::IBV_MTU_1024;
    attr.dest_qp_num	= 0x11; // remote_qpn;
    attr.rq_psn		= 0; // remote_psn;
    attr.max_dest_rd_atomic	= 1;
    attr.min_rnr_timer	= 0x12;
    attr.ah_attr.is_global	   = 0;
    attr.ah_attr.dlid	   = 0; // remote_lid;
    attr.ah_attr.sl		   = 0; // my_sl;
    attr.ah_attr.src_path_bits = 0;
    attr.ah_attr.port_num	   = 1; // my_port;


    rc = ibv_modify_qp(qp, &attr, 
		  IBV_QP_STATE              |
		  IBV_QP_AV                 |
		  IBV_QP_PATH_MTU           |
		  IBV_QP_DEST_QPN           |
		  IBV_QP_RQ_PSN             |
		  IBV_QP_MAX_DEST_RD_ATOMIC |
		  IBV_QP_MIN_RNR_TIMER );

    if (rc == 0)
        puts("QP changed to RTR");
    else if (rc == EINVAL)
        puts("RTR: Invalid value provided in attr or in attr_mask");
    else if (rc == ENOMEM)
        puts("RTR: Not enough resources to complete this operation");
    else
        printf("RTR: QP modify status: %i\n",rc);

    // *********************************************  RTS   *************************************
     
    memset(&attr, 0, sizeof(attr));
    attr.qp_state		= IBV_QPS_RTS;
    attr.sq_psn        = 0;

    rc = ibv_modify_qp(qp, &attr, IBV_QP_STATE | IBV_QP_SQ_PSN);

    switch(rc)
    {
        case 0:
            puts("QP changed to RTS");
            break;
        case EINVAL:
            puts("RTS: Invalid value provided in attr or in attr_mask");
            break;
        case ENOMEM:
            puts("RTS: Not enough resources to complete this operation");
            break;
        default:
            printf("RTS: QP modify status: %i\n",rc);
    };



    ibv_destroy_qp(qp);
    printf("QP destroied.");
    ibv_dereg_mr(mr);
    printf("memory region (MR) deregistered.\n");
    ibv_destroy_cq(cq);
    printf("CQ destroied.\n");
    ibv_dealloc_pd(pd);
    printf("PD deallocated.\n");
    ibv_close_device(ctx);
    printf("device closed.\n");

}