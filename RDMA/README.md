# install rdma-core
sudo apt install rdma-core

# install libibverbs 
sudo apt install libibverbs-dev

or

sudo yum install libibverbs-devel

# Load RDMA driver
- sudo modprobe rdma_rxe

# Add RDMA interface
- sudo rdma link add rxe0 type rxe netdev enp0s3

# Compile
- g++ test1.c -o bin/test1  -libverbs




# ref:

https://www.mellanox.com/related-docs/prod_software/RDMA_Aware_Programming_user_manual.pdf


https://www.rdmamojo.com/


