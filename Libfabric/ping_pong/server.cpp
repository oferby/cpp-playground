
#include <rdma/fabric.h>

// Fabric Endpoint
// https://manpages.debian.org/testing/libfabric-dev/fi_endpoint.3.en.html
#include <rdma/fi_endpoint.h>

// Connection management operations 
// https://manpages.debian.org/testing/libfabric-dev/fi_cm.3.en.html
#include <rdma/fi_cm.h> 

#include <rdma/fi_errno.h>
#include <rdma/fi_rma.h>


#include <iostream>

using namespace std;

struct fi_info *fi, *hints;

int main() {

    int err = fi_getinfo(FI_VERSION(1,11), NULL, NULL, 0, NULL, &fi);

    if (err) {
        cout << "error geting info!" << endl;
    }

    cout << fi->nic->device_attr->name << endl;

}