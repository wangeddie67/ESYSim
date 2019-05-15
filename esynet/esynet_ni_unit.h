/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#ifndef ESYNET_NI_UNIT_H
#define ESYNET_NI_UNIT_H

/* including head file */
#include "esy_interdata.h"
#include "esy_networkcfg.h"
#include "esynet_config.h"
#include "esynet_ecc_unit.h"
#include "esynet_sim_base.h"
#include "esynet_statistics.h"

/* include library file */
#include <cmath>

/* namespace */
using namespace std;

/*************************************************
  Class Name : 
    ProcessingElement
    
  Description : 
    Model for processing element
    
  Public Types :
    enum TrafficRule
    
  Properties
    m_id                            : long
    m_router_num                    : long
    m_array_size                    : vector< long >
    m_physical_port                 : long
    mp_router                       : void *
    m_generate_traffic              : bool
    m_pir                           : double
    m_traffic                       : TrafficRule
    m_send_packet_count             : unsigned int
    m_recv_packet_count             : unsigned int
    m_ack_packet                    : bool
    m_retransmission                : bool
    m_retransmission_buffer         : vector< vector< EsynetFlit > >
    m_retransmission_timer_max      : double
    m_retransmission_timer          : vector< unsigned long >
    m_retransmission_receive_buffer : vector< EsynetFlit >
    m_period_bist_enable            : bool
    m_bist_interval_duration        : long
    m_bist_link_duration            : long
    m_bist_router_duration          : long
    m_bist_router_state             : BISTState
    m_bist_link_state               : vector< BISTState >
    m_bist_interval_timer           : long
    m_bist_router_timer             : long
    m_bist_link_timer               : vector< long >

  Public Functions :
         ProcessingElement( long local_id, long phy_port, void * router )
    void runBeforeRouter()
    void runAfterRouter()
    void runAfterInjection( const EsynetFlit & b )
    void runAfterInjection( const EsynetFlit & b )
    void startBistRouterFsm()
    void startBistLinkFsm( long port )
      
  Protected Functions :
    int randInt( int min, int max )
    void setBit( int &x, int w, int v )
    int getBit( int x, int w )
    double log2Ceil( double x )
    void trafficRandom( BenchmarkItem * p )
    void trafficTranspose1( BenchmarkItem * p )
    void trafficTranspose2( BenchmarkItem * p )
    void trafficBitReversal( BenchmarkItem * p )
    void trafficShuffle( BenchmarkItem * p )
    void trafficButterfly( BenchmarkItem * p )
*************************************************/
class EsynetNI : public EsynetSimBaseUnit
{
/* Properties */
private:
	/* General Information */
	/* Unique identification number */
	long m_id;

	EsyNetworkCfg * m_network_cfg;
	EsyNetworkCfgRouter * m_router_cfg;
	EsynetConfig * m_argu_cfg;

	/* injected flit queue */
	vector< EsynetFlit > m_inject_queue;
	/* selected virtual channel */
	long m_inject_vc;
	/* credit counter */
	vector< long > m_vc_counter;
	
	/* ejected flit queue */
	vector< EsynetFlit > m_eject_queue;

	/* ejected flit queue */
	vector< EsynetEvent > m_accept_list;

	EsynetNIStatistic m_statistic;

	/* initial random number */
	EsynetPayload m_init_data;
	/* size of 64 bits */
	long m_flit_size;

	/* Retransmission */
	/* retransmission buffer */
	vector< vector< EsynetFlit > > m_retransmission_buffer;
	/* retransmission counter */
	vector< unsigned long > m_retransmission_timer;
	/* retransmission receiver buffer */
	vector< EsynetFlit > m_retransmission_receive_buffer;

	bool m_flit_on_link;
	/* port empty require */
	bool m_empty_require;

	EsynetECCEncoder m_ecc_encoder;
	EsynetECCFinalDecoder m_ecc_decoder;
    
/* Public functions */
public:
	/* constructor function */
// 	EsynetNI() : EsynetSimBaseUnit() {}
	EsynetNI(EsyNetworkCfg * network_cfg,
		long id, EsynetConfig * argument_cfg);

	const EsynetNIStatistic & statistic() { return m_statistic; }
	/* functions run before simulationrouter */
	void runBeforeRouter();
	/* functions run before simulation of router */
	void runAfterRouter();

	/* inject packet */
	void injectPacket(const EsynetFlit& b);
	/* receive packet */
	void receiveFlit(long vc, const EsynetFlit & b);
	void receivePacketHandler();
	void receivePacket(const EsynetFlit & b);
	/* receive credit */
	void receiveCredit( long vc, long credit ) { m_vc_counter[ vc ] = credit ;}
	/* flit traversal */
	void flitTraversal();
	
	const vector< EsynetEvent > acceptList() const 
		{ return m_accept_list; }
	void clearAcceptList() { m_accept_list.clear(); }

	/* get suggest vc */
	long suggestVC();

	void setEmptyReqire( bool req ) { m_empty_require = req; }
	bool isEmpty();

	void clearFlitOnLink() { m_flit_on_link = false; }

	/* configure fault */
	void configFault( EsyFaultConfigList & fault );
	void eccBuffer();
    
/* Protected Functions */
protected:
	/* E2E Retransmission Functions */
	/* E2E retransmission timer and functions driven by timer overflow */
	void e2eRetransmissionTimerOverflow();
	/* inject packet into E2E retransmission buffer */
	void e2eRetransmissionInjection(const EsynetFlit & b);
	/* confirm E2E retransmission buffer */
	void e2eRetransmissionComfirm(const EsynetFlit & b);
};

#endif
