#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <infiniband/verbs.h>


int main(int argc, char* argv[]) {


    struct ibv_device   **dev_list;
    struct ibv_device *ib_dev;
    struct ibv_context *ctx;
    struct ibv_device_attr *ib_device_attr; 
    struct ibv_port_attr *ib_port_attr;

    struct ibv_cq *cq;

    struct ibv_pd *pd;
    struct ibv_mr *mr;
    

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

    ib_port_attr = (struct ibv_port_attr*) malloc(sizeof (struct ibv_poll_cq_attr));
    if (ibv_query_port(ctx, 1, ib_port_attr) == -1){
        perror("could not get port attributes.");
        return 1;
    } 

    printf("port status: %s\n", ibv_port_state_str(ib_port_attr->state));

    cq = ibv_create_cq(ctx, 10, NULL, NULL,10);

    if (!cq) {
        perror("could not create CQ.");
        return 1;
    }

    pd = ibv_alloc_pd(ctx);
    if (!pd) {
        perror("error allocating PD");
        return 1;
    }



    ibv_destroy_cq(cq);
    ibv_dealloc_pd(pd);
    ibv_close_device(ctx);

}