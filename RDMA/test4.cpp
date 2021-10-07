#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <infiniband/verbs.h>
#include <string.h>


struct ibv_qp* createQueuePair(struct ibv_pd* pd, struct ibv_cq* cq) {
  struct ibv_qp_init_attr queue_pair_init_attr;
  memset(&queue_pair_init_attr, 0, sizeof(queue_pair_init_attr));
  queue_pair_init_attr.qp_type = IBV_QPT_RC;
  queue_pair_init_attr.sq_sig_all = 1;       // if not set 0, all work requests submitted to SQ will always generate a Work Completion.
  queue_pair_init_attr.send_cq = cq;         // completion queue can be shared or you can use distinct completion queues.
  queue_pair_init_attr.recv_cq = cq;         // completion queue can be shared or you can use distinct completion queues.
  queue_pair_init_attr.cap.max_send_wr = 1;  // increase if you want to keep more send work requests in the SQ.
  queue_pair_init_attr.cap.max_recv_wr = 1;  // increase if you want to keep more receive work requests in the RQ.
  queue_pair_init_attr.cap.max_send_sge = 1; // increase if you allow send work requests to have multiple scatter gather entry (SGE).
  queue_pair_init_attr.cap.max_recv_sge = 1; // increase if you allow receive work requests to have multiple scatter gather entry (SGE).

  return ibv_create_qp(pd, &queue_pair_init_attr);
}

bool changeQueuePairStateToInit(struct ibv_qp* queue_pair) {
  struct ibv_qp_attr init_attr;
  memset(&init_attr, 0, sizeof(init_attr));
  init_attr.qp_state = ibv_qp_state::IBV_QPS_INIT;
  init_attr.port_num = 1;
  init_attr.pkey_index = 0;
  init_attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE;

  return ibv_modify_qp(queue_pair, &init_attr, IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS) == 0 ? true : false;
}

bool changeQueuePairStateToRTR(struct ibv_qp* queue_pair, int ib_port, uint32_t destination_qp_number, uint16_t destination_local_id) {
  struct ibv_qp_attr rtr_attr;
  memset(&rtr_attr, 0, sizeof(rtr_attr));
  rtr_attr.qp_state = ibv_qp_state::IBV_QPS_RTR;
  rtr_attr.path_mtu = ibv_mtu::IBV_MTU_1024;
  rtr_attr.rq_psn = 0;
  rtr_attr.max_dest_rd_atomic = 1;
  rtr_attr.min_rnr_timer = 0x12;
  rtr_attr.ah_attr.is_global = 0;
  rtr_attr.ah_attr.sl = 0;
  rtr_attr.ah_attr.src_path_bits = 0;
  rtr_attr.ah_attr.port_num = ib_port;
  
  rtr_attr.dest_qp_num = destination_qp_number;
  rtr_attr.ah_attr.dlid = destination_local_id;

  return ibv_modify_qp(queue_pair, &rtr_attr, IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU | IBV_QP_DEST_QPN | IBV_QP_RQ_PSN | IBV_QP_MAX_DEST_RD_ATOMIC | IBV_QP_MIN_RNR_TIMER) == 0 ? true : false;
}


int main(int argc, char* argv[]) {

    struct ibv_device   **dev_list;
    struct ibv_device   *ib_dev;
    struct ibv_context  *ctx;
    
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


    struct ibv_pd* pd = ibv_alloc_pd(ctx);

    int cq_size = 0x10;
    struct ibv_cq* cq = ibv_create_cq(ctx, cq_size, nullptr, nullptr, 0);

    struct ibv_qp* qp = createQueuePair(pd, cq);


    if (changeQueuePairStateToInit(qp))
        puts("QP changed to INIT");
    else
        puts("error changing QP to INIT");

    if (changeQueuePairStateToRTR(qp, 1, 0x11, 0))
        puts("QP changed to RTR");
    else
        puts("error changing QP to RTR");


}

