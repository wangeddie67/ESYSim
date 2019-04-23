#include <exception>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//from ESYNet
#include "../esynet/esynet_interface.h"
//from ESYArch
#include "host.h"
#include "define.h"
#include "Noc.h"
extern "C" {
//#include "SIM_power.h"
//#include "SIM_router_power.h"
//#include "SIM_power_router.h"
//#include "host.h"
//#include "machine.h"
//#include "network.h"
//#include "options.h"
};
#define MULTI_VC

//from ESYNet
int simEsynetInit(const EsyNetworkCfg & cfg);
//void simEsynetOptions(struct opt_odb_t *odb);
int simEsynetRunSim(long long int sim_cycle);
int ReceiveMsgFromNet(long src, long dst, long long int stTime,
long long int msgNo);
//void simEsynetMessageInsert(long s1, long s2, long d1, long d2, long long int 
//sim_cycle, long size, long long msgNo, long long addr, int operation, int vc);
int simEsynetBufferSpace(long s1, long s2, int opt);
void simEsynetReport(FILE *fd);
//from ESYArch
extern long long sim_cycle;
extern Noc glb_network;

EsynetMessQueue  *gp_esynet_mess_queue;
EsynetFoundation *gp_esynet_foundation;
EsynetConfig     *gp_esynet_config;
EsynetSRGen      *gp_random_generator;
EsyNetworkCfg    *gp_network_config;
long long int messageQueued = 0;
char *gp_esynet_command;
vector< EsynetMessEvent > g_accept_list;

//void simEsynetOptions(struct opt_odb_t *odb)
//{
//	opt_reg_string (odb, "-mesh_network:command_line", "", &gp_esynet_command, 
//		/* default */"", /* print */ TRUE, /* format */ NULL);
//}

int simEsynetInit(const EsyNetworkCfg & cfg)
{
	gp_esynet_config = new EsynetConfig( EsynetConfig::CONFIG_INTERFACE);
	esynetConfigInit( gp_esynet_config, gp_esynet_command );
	cout << gp_esynet_config;
	
	gp_random_generator = new EsynetSRGen( gp_esynet_config->randomSeed() );
            
	/* generate network configuration */
	gp_network_config = new EsyNetworkCfg(cfg);
	esynetNetworkInit2( gp_network_config, gp_esynet_config );
    
	gp_esynet_foundation = new EsynetFoundation( gp_network_config,
		gp_esynet_config );
	cout << (*gp_esynet_foundation);

	gp_esynet_mess_queue = new EsynetMessQueue( 0.0, 
		gp_esynet_foundation, gp_esynet_config );
	esynetMessQueueInit( gp_esynet_mess_queue, gp_esynet_config );
}

int simEsynetRunSim(long long int sim_cycle)
{
	esynetRunSim( gp_esynet_mess_queue, gp_esynet_foundation, sim_cycle );

	vector< EsynetMessEvent > t_accept_list = esynetAcceptList();
	esynetClearAcceptList();
	
	for ( int i = 0; i < t_accept_list.size(); i ++ )
	{
		g_accept_list.push_back( t_accept_list[ i ] );
	}
	
	for ( int i = 0; i < g_accept_list.size(); i ++ )
	{
		if( glb_network.ReceiveMsgFromNet(g_accept_list[i].flit().sorAddr(),g_accept_list[i].flit().desAddr(),
			g_accept_list[ i ].flit().e2eStartTime(), 
			g_accept_list[ i ].flit().flitId() ) )
		{
			g_accept_list.erase( g_accept_list.begin() + i );
			i --;					
			messageQueued--;
		}
	}
	return 0;
}

void simEsynetMessageInsert(long src, long dst, long long int 
sim_cycle, long size, long long msgNo, int vc)
{	
	gp_esynet_mess_queue->insertMsg(src, dst, sim_cycle, size, msgNo,0, 
		(long)vc);
	messageQueued++;
}

int simEsynetBufferSpace(long s1, long s2, int opt)
{
	vector< long > sor_addr_t;
	sor_addr_t.resize(2);
	sor_addr_t[0] = s1;
	sor_addr_t[1] = s2;
    long src = gp_network_config->coord2Seq( sor_addr_t );
//	return FoundationRouter(sor_addr_t).isBufferFull();
#ifdef MULTI_VC
//	if(opt == -1)
//		return sim_net_ptr->processelement(src).suggestVC();
//	return sim_net_ptr->router(src).isBufferFull(0, opt) ? -1:opt;
#else
//	return sim_net_ptr->router(src).isBufferFull(0, 0);
#endif

	return 0;
}

void simEsynetReport(FILE *fd)
{
	gp_esynet_foundation->simulationResults();
}

int simEsynetMsgComplete(int src, int dst, long long int stTime, long long int msgNo){
	return(glb_network.ReceiveMsgFromNet(src,dst,stTime,msgNo));
}
