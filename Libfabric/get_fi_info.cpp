#include <rdma/fabric.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>
// #include<rdma/fi_atomic.h>

// may need this too
#include <rdma/fi_cm.h>
#include <rdma/fi_tagged.h>
#include <rdma/fi_rma.h>
#include <rdma/fi_errno.h>

#include <iostream>
#include<string>

using namespace std;

struct fi_info *hints, *fi;

int main() {



    // find provider with hints on what we are looking for

    // first allocate memory
    hints = fi_allocinfo();

    hints->ep_attr->type = FI_EP_RDM;
    hints->caps = FI_MSG | FI_TAGGED | FI_RMA;
    // hints->mode = FI_CONTEXT;
    // hints->fabric_attr->prov_name = strdup("psm2");

    int err = fi_getinfo(FI_VERSION(1,11), NULL, NULL, 0, hints, &fi);

    if (err) {
        cout << "got error: " << err << "\n";
        exit(1);
    }

    int i = 0;
    while (fi != NULL) {
        i++;
        fi = fi->next;
    }
    
    printf("got %d FIs\n", i);

    // *** 

    hints = fi_allocinfo();

    // hints->caps = FI_READ | FI_WRITE | FI_REMOTE_READ 
        // | FI_REMOTE_WRITE | FI_RMA;
    // the same result
    hints->caps = FI_RMA;
    err = fi_getinfo(FI_VERSION(1,11), NULL, NULL, 0, hints, &fi);

    if (err) {
        cout << "got error: " << err << "\n";
        exit(1);
    }

    i = 0;
    while (fi != NULL) {

        fi_fabric_attr *fattr = fi->fabric_attr;

        // cout << fi_tostr(fattr, fi_type::FI_TYPE_FABRIC_ATTR) << endl;
        
        i++;
        fi = fi->next;
    }
    
    printf("got %d FIs\n", i);

    // ***********************************

    hints = fi_allocinfo();

    char prov[] = "tcp;ofi_rxm";
    hints->fabric_attr->prov_name = prov;

    err = fi_getinfo(FI_VERSION(1,11), "10.10.0.120", NULL, 0, hints, &fi);

    if (err) {
        cout << "got error: " << err << "\n";
        exit(1);
    }

    i = 0;
    while (fi != NULL) {
        fi_fabric_attr *fattr = fi->fabric_attr;
        cout << fi_tostr(fattr, fi_type::FI_TYPE_FABRIC_ATTR) << endl;
        i++;
        fi = fi->next;
    }
    
    printf("got %d FIs\n", i);


}