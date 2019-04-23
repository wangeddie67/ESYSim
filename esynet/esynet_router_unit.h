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

#ifndef ESYNET_ROUTER_UNIT_H
#define ESYNET_ROUTER_UNIT_H

/* including head file */
#include "esy_networkcfg.h"
#include "esynet_config.h"
#include "esynet_ecc_unit.h"
#include "esynet_port_unit.h"
#include "esynet_router_power.h"
#include "esynet_sim_base.h"
#include "esynet_statistics.h"
#include "esynet_arbiter.h"

#include <map>
#include <sstream>
#include <cmath>
#include <cstdlib>

class EsynetRouter : public EsynetSimBaseUnit
{
/* Properties */
private:
	EsyNetworkCfg * m_network_cfg;
	EsyNetworkCfgRouter * m_router_cfg;
	EsynetConfig * m_argu_cfg;

	/* router id */
	long m_router_id;
	AddrType m_router_addr;

	/* control flags */
	bool m_bypass;

	/* Components
	/* the input buffer module */
	vector< EsynetInputPort > m_input_port;
	/* output module */
	vector< EsynetOutputPort > m_output_port;

	/* Stastics parameters */
	/* power module */
	EsynetRouterPower m_power_unit;
	/* statics module */
	EsynetRouterStatistic m_statistics_unit;
	
	/* arbiter */
	vector< vector< EsynetArbiter > > m_vc_input_arbiter;
	vector< vector< EsynetArbiter > > m_vc_output_arbiter;
	vector< EsynetArbiter > m_port_input_arbiter;
	vector< EsynetArbiter > m_port_output_arbiter;

	/* fault injection */
	/* router fault flag */
	EsynetRouterFaultUnit m_router_fault_unit;
	/* port fault flag */
	vector< EsynetPortFaultUnit > m_port_fault_units;
	/* link fault injection unit */
	vector< EsynetLinkFaultUnit > m_link_fault_units;
	/* switch fault injection unit */
	vector< EsynetLinkFaultUnit > m_switch_fault_units;

	/* shared information */
	vector< bool > m_neighbor_faulty;
	vector< bool > m_corner_faulty;

	/* Ecc units */
	vector< EsynetECCInterDecoder > m_ecc_units;

	/* Spare lines */
	/* spare line pattern */
	vector< DataType > m_spare_line_pattern;

	/* Interleaving */
	/* interleaving enable */
	bool m_interleaving_enable;
	/* interleaving group width */
	long m_interleaving_group_width;

	/* Build-in Self-Test */
	/* Build-in Self-Test interval timer */
	long m_bist_interval_timer;
	/* Build-in Self-Test duration timer */
	long m_bist_duration_timer;
	vector< long > m_port_bist_timer;
	/* Build-in Self-Test state */
	BISTRouterState m_bist_router_state;
	vector< BISTPortState > m_bist_port_state;
	vector< bool > m_bist_neighbor_empty_ack;
	/* Build-in Self-Test, enabled channel by neighbor, for Alter-Test only */
	vector< long > m_bist_enable_channel;

	/* the routing algorithm is now used, this is a pointer, points to
		* the routing procedure */
	void (EsynetRouter::* m_curr_algorithm)(long, long, long, long);
/* Public Functions */
public:
	/* constructor */
	EsynetRouter();
	EsynetRouter(EsyNetworkCfg * network_cfg, long router_id,
		EsynetConfig * argument_cfg);
	/* router configuration */
	/* configure fault */
	void configFault( EsyFaultConfigList & fault );

	bool isWorking() const 
	{ return !(m_router_fault_unit.currentStateOut() && !m_bypass); }

	long creditCounter( long phy, long vc ) const
		{ return m_output_port[ phy ][ vc ].creditCounter(); }
	long creditCounter( const VCType port ) const
		{ return m_output_port[ port.first ][ port.second ].creditCounter(); }
	bool routerFault() const { return m_router_fault_unit.currentStateOut(); }

	/* function to access m_input_port */
	EsynetInputVirtualChannel & inputVirtualChannel(long phy, long vc)
		{ return m_input_port[ phy ][ vc ]; }
	const EsynetInputVirtualChannel & inputVirtualChannel(
		long phy, long vc) const
		{ return m_input_port[ phy ][ vc ]; }
	/* function to access m_input_port */
	EsynetOutputPort & outputPort(long phy)
		{ return m_output_port[ phy ]; }
	const EsynetOutputPort & outputPort(long phy) const
		{ return m_output_port[ phy ]; }
	/* function to access m_statistics_unit */
	const EsynetRouterStatistic & statistics() const 
		{ return m_statistics_unit; }
	/* function to access m_power_unit */
	EsynetRouterPower & powerUnit() { return m_power_unit; }
	
	void setInputNeighborAddr( long port, const VCType & addr )
		{ m_input_port[ port ].setInputNeighborAddr( addr ); }
	void setOutputNeighborAddr( long port, const VCType & addr )
		{ m_output_port[ port ].setOutputNeighborAddr( addr ); }

	/* shared information */
	bool neighborFaulty( long phy )
		{ return m_router_fault_unit.currentStateOut() || 
			m_port_fault_units[ phy ].currentStateOut();}
	void setNeighborFaulty( long phy, bool faulty )
		{ m_neighbor_faulty[ phy ] = faulty; }
	void setCornerFaulty( long phy, bool faulty )
		{ m_corner_faulty[ phy ] = faulty; }
	void setBistPortState(long int phy, BISTPortState state);
	void setPortEmptyAck( long phy, bool ack )
		{ m_bist_neighbor_empty_ack[ phy ] = ack; }
	BISTPortState bistPortState( long phy );
	long bistEnableChannel() { return m_bist_enable_channel[ 0 ]; }
	long bistEnableChannel( long phy ) { return m_bist_enable_channel[ phy ]; }
	void setBistEnableChannel( long phy, long channel )
		{ m_bist_enable_channel[ phy ] = channel; }
			
	/* event handler function */
	/* receive flit */
	void receiveFlit(long a, long b, const EsynetFlit & c);
	void receiveFlitInBuffer(long a, long b, const EsynetFlit & c);
	/* receive credit processing of physical port phy and virtual channel vc */
	void receiveCredit( long phy, long vc, long credit );

	/* operate function of vc */
	/* get vc */
	VCType vcSelection( long a, long b );

	/* pipeline functions */
	/* simulate pipeline */
	void routerSimPipeline();
	/* routing decision */
	void routingDecision();
	/* virtual channel arbitration */
	void vcArbitration();
	/* switch arbitration */
	void swArbitration();
	/* flit cross switch */
	void flitOutbuffer();
	/* flit traversal */
	void flitTraversal();

	/* Build-in Self-Type */
	void bistOrderInit();
	/* state machine */
	void bistStateMachine();
	bool bistEmptyCheck();

	void faultStateUpdate();
	/* fault injection and recover */
	void portFaultInjection(long phy, EsynetFlit & flit_t);
	void switchFaultInjection(long phy, EsynetFlit & flit_t);
	/* report bit fault to port fault */
	void reportFault( long phy );

	void eccLocation(ECCStrategy strategy);
	void eccDecoder(const string & codename, EsynetFlit & flit_t);
	void eccBuffer();
   
	/* bypass */
	VCType bypassInput2Output( const VCType & input );
	VCType bypassOutput2Input( const VCType & output );
	void bypassReceiveFlit(long a, long b, const EsynetFlit & c);
	void bypassReceiveCredit( long phy, long vc, long credit );
	
	/* routing algorithm */
	/* XY, TXY, DyXY in sim_routing_xy.cc */
	void algorithmXY( long des_t, long sor_t,long s_ph, long s_vc );
	void algorithmTXY( long des_t, long sor_t, long s_ph, long s_vc );
	void algorithmDyXY( long des_t, long sor_t, long s_ph, long s_vc );
	/* gradient in sim_routing_gradient.cc */
	void algorithmGradient( long des_t, long sor_t, long s_ph, long s_vc );
	/* FON in sim_routing_fon.cc */
	void algorithmFON( long des_t, long sor_t, long s_ph, long s_vc );
	/* adaptive FRR and nstep FRR in sim_routing_frr.cc */
	void algorithmAFRR( long des_t, long sor_t, long s_ph, long s_vc );
	void algorithmNFRR( long des_t, long sor_t, long s_ph, long s_vc );
	/* rescuer in sim_routing_rescuer.cc */
	void algorithmRescuer( long des_t, long sor_t, long s_ph, long s_vc );
	void algorithmERescuer( long des_t, long sor_t, long s_ph, long s_vc );
	/* rescuer in sim_routing_nonblock.cc */
	void algorithmAlterTest( long des_t, long sor_t, long s_ph, long s_vc );
	void algorithmNonBlock( long des_t, long sor_t, long s_ph, long s_vc );
	/* HiPFaR in sim_routing_hipfar.cc */
	void algorithmHiPFaR( long des_t, long sor_t, long s_ph, long s_vc );
	/* FTLR in sim_routing_ftlr.cc */
	void algorithmFTLR( long des, long src, long s_ph, long s_vc );

	/* routing function called path execution */
	void routingAlgorithm( long des_t, long sor_t, long s_ph, long s_vc )
		{ (this->*m_curr_algorithm)( des_t, sor_t, s_ph, s_vc ); }

	/* print configuration of router to stream */
	friend ostream& operator<<(ostream& os, const EsynetRouter & sr);
};


#endif
