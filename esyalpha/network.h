#ifndef NETWORK_H
#define NETWORK_H
#include <stdio.h>
#include "host.h"
#include "misc.h"
#include "machine.h"
#include "stats.h"
#include "smt.h"

/* 
 * This Module implements a number of network mechanisms. The following networks are supported:
	Optical: Free space optical interconnect using VCSELs and PDs to communicate
		Mesh: Conventional network using electrical to communicate 
implementated with esynet
	Combination: Multi chips network simulation, can combining two different networks, intra-network can select optical and inter-chip network is Mesh and vise versa. 
*/
#define FSOI_NET_INTER	1
#define FSOI_NET_INTRA	2
#define MESH_NET_INTER  3
#define MESH_NET_INTRA	4

/* network types */
char *network_type;	/* network types: FSOI/MESH/COMB */
char *inter_cfg; 	/* network configuration within chips */
char *intra_cfg;  	/* network configuration between chips */
int total_chip_num;  	/* the number of chips in system */
int Line_chip_num;
int Column_chip_num;
enum network_class {
	FSOI,
	MESH,
	COMB,
	HYBRID
};

void scheduleThroughNetwork(struct DIRECTORY_EVENT *event, long start, unsigned int type, int vc);
void esynetMessageInsert(long s1, long s2, long d1, long d2, long long int 
sim_cycle, long size, counter_t msgNo, md_addr_t addr, int operation, int vc);


void EUP_entry_init(int id);
int EUP_entry_replacecheck(int id, md_addr_t addr);
int EUP_entry_check(int id, md_addr_t addr);
void EUP_entry_allocate(int id, md_addr_t addr);
void EUP_entry_dellocate(int id, md_addr_t addr);
int EarlyUpgrade(struct DIRECTORY_EVENT *event);
int EarlyUpgrateGenerate(struct DIRECTORY_EVENT *event);
#endif
