#include <infiniband/verbs.h>
#include <endian.h>
#include <map>
#include <vector>

#ifndef RDMA_HANDLER 
#define RDMA_HANDLER 

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
	struct ibv_port_attr     *portinfo;
   
    uint64_t wid = 0;
    std::map <uint64_t,ibv_sge*> *sge_map;
    std::vector<ibv_sge*> *available_send_sge_vector;

};

struct app_dest {
	uint16_t lid;
	int qpn;
	int psn;
	union ibv_gid *gid;
	ibv_ah *ah;
};


struct neighbor {
    sockaddr_in *addr;
    app_dest *dest;
    time_t lastHello;
};


void print_gid(const union ibv_gid *gid) {
		static char gid_tmp[33];
        inet_ntop(AF_INET6, gid, gid_tmp, INET6_ADDRSTRLEN);
	    printf(" ** GID %s\n", gid_tmp);

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


void print_dest(struct app_dest * dest) {
		static char gid[33];
        inet_ntop(AF_INET6, dest->gid, gid, sizeof gid);
	    printf("  address:  LID 0x%04x, QPN 0x%06x, PSN 0x%06x: GID %s\n",
			dest->lid, dest->qpn, dest->psn, gid);
}


#endif

