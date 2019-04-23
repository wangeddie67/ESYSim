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

#include "esynet_router_unit.h"

/*************************************************
  Function : 
    ostream & operator<<(ostream& os, const SimRouter& sr)
  Description : 
    print EsynetConfig of router $sr$ to ostream os
  Input:
            ostream& os  output stream
    const SimRouter& Ri  router unit to print.
*************************************************/
ostream & operator<<(ostream& os, const EsynetRouter & sr)
{
	/* print router id, pipeline scalar */
	os << "router(" << sr.m_router_id << ")";
	os << " pipe_scalar=" << sr.m_router_cfg->pipeCycle();
	/* print input neighbor */
	os << " input_neighbor={";
	for ( size_t i = 0; i < sr.m_input_port.size(); i ++ )
	{
		os << "(" << sr.m_input_port[ i ].inputNeighborAddrId() <<"," <<
					sr.m_input_port[ i ].inputNeighborAddrPort() << ")";
		/* last item, donot print spacer mark */
		if (i != sr.m_input_port.size() - 1)
		{
			os << ",";
		} /* else */
	} /* for (int i = 0; i < sr.physic_ports_; i ++) */
	os << "}";
	/* print output neighbor */
	os << "output_neighbor={";
	for (size_t i = 0; i < sr.m_output_port.size(); i ++)
	{
		os << "(" << sr.m_output_port[ i ].outputNeighborAddrId()  <<"," <<
					sr.m_output_port[ i ].outputNeighborAddrPort() << ")";
		/* last item, donot print spacer mark */
		if (i != sr.m_output_port.size() - 1)
		{
			os << ",";
		} /* else */
	} /* end of  for (int i = 0; i < Ri.physic_ports_; i ++) */
	os << "}";
	/* print return */
	os << endl;
    
	os << sr.m_router_fault_unit;
	for (size_t i = 0; i < sr.m_output_port.size(); i ++)
	{
		os << sr.m_port_fault_units[ i ];
		os << sr.m_link_fault_units[ i ];
		os << sr.m_switch_fault_units[ i ];
		os << sr.m_ecc_units[ i ];
	}
	return os;
}

/*************************************************
  Function : 
    SimRouter::SimRouter()
  Description: 
    Construct an empty entity for router.
*************************************************/
EsynetRouter::EsynetRouter() :
	m_network_cfg(),
	m_router_cfg(),
	m_argu_cfg(),
	m_router_id(),
	m_router_addr(),
	m_bypass(),
	m_input_port(),
	m_output_port(),
	m_vc_input_arbiter(),
	m_vc_output_arbiter(),
	m_port_input_arbiter(),
	m_port_output_arbiter(),
	m_power_unit(),
	m_router_fault_unit(),
	m_port_fault_units(),
	m_link_fault_units(),
	m_switch_fault_units(),
	m_ecc_units(),
	m_neighbor_faulty(),
	m_corner_faulty(),
	m_spare_line_pattern(),
	m_interleaving_enable(),
	m_interleaving_group_width(),
	m_bist_interval_timer(),
	m_bist_duration_timer(),
	m_port_bist_timer(),
	m_bist_router_state(),
	m_bist_port_state(),
	m_bist_neighbor_empty_ack(),
	m_curr_algorithm()
{
}

/*************************************************
 *  Function : 
 *    SimRouter::SimRouter(long phy, long vc, long inbuf, long outbuf, long id, 
 *        vector< long > ary, long flit)
 *  Description : 
 *    Construct an entity for router with specific argument.
 *  Inputs:
 *    long phy            number of physical port
 *    long vc             number of virtual channel
 *    long inbuf          size of input buffer
 *    long outbuf         size of output buffer
 *    long id             router id
 *    vector< long > ary  ary of network
 *    long flit           flit size
 *************************************************/
EsynetRouter::EsynetRouter(EsyNetworkCfg * network_cfg, long router_id,
	EsynetConfig * argument_cfg
) :
	m_network_cfg( network_cfg ),
	m_router_cfg( &network_cfg->router( router_id ) ),
	m_argu_cfg( argument_cfg ),
	m_router_id( router_id ),
	m_router_addr(),
	m_bypass( false ),
	m_input_port(),
	m_output_port(),
	m_vc_input_arbiter( network_cfg->router( router_id ).portNum() ),
	m_vc_output_arbiter( network_cfg->router( router_id ).portNum() ),
	m_port_input_arbiter(),
	m_port_output_arbiter(),
	m_power_unit( network_cfg->router( router_id ).portNum(), 
				network_cfg->router( router_id ).maxVcNum(), 
				network_cfg->flitSize( ATOM_WIDTH_ ), 0 ),
	m_statistics_unit(),
	m_router_fault_unit( router_id ),
	m_port_fault_units(),
	m_link_fault_units(),
	m_switch_fault_units(),
	m_ecc_units(),
	m_neighbor_faulty( network_cfg->router( router_id ).portNum(), false ),
	m_corner_faulty( network_cfg->router( router_id ).portNum(), false ),
	m_spare_line_pattern( network_cfg->router( router_id ).portNum(), 
						DataType( network_cfg->flitSize( ATOM_WIDTH_ ) ) ),
	m_interleaving_enable( false ),
	m_interleaving_group_width( 8 ),
	m_bist_interval_timer(),
	m_bist_duration_timer(),
	m_port_bist_timer( network_cfg->router( router_id ).portNum(), -1 ), 
	m_bist_router_state( esynet::BIST_ROUTER_NORMAL ),
	m_bist_port_state( 
		network_cfg->router( router_id ).portNum(), esynet::BIST_PORT_NORMAL ),
	m_bist_neighbor_empty_ack( 
		network_cfg->router( router_id ).portNum(), true ),
	m_bist_enable_channel( network_cfg->router( router_id ).portNum(), 0 ),
	m_curr_algorithm()
{
	/* temp initialization */
	m_router_addr = m_network_cfg->seq2Coord( m_router_id );

	/* initializate routing algorithm pointer */
	switch (m_argu_cfg->routingAlg())
	{
	case RA_XY:
		m_curr_algorithm = &EsynetRouter::algorithmXY;
		m_bypass = false;
		break;
	case RA_TXY:
		m_curr_algorithm = &EsynetRouter::algorithmTXY;
		m_bypass = false;
		break;
	case RA_DYXY:
		m_curr_algorithm = &EsynetRouter::algorithmDyXY;
		m_bypass = false;
		break;
	case RA_CORERESCUER:
		m_curr_algorithm = &EsynetRouter::algorithmRescuer;
		m_bypass = true;
		break;
	case RA_ERESCUER:
		m_curr_algorithm = &EsynetRouter::algorithmERescuer;
		m_bypass = true;
		break;
	case RA_HIPFAR:
		m_curr_algorithm = &EsynetRouter::algorithmHiPFaR;
		m_bypass = false;
		break;
	case RA_FTLR:
		m_curr_algorithm = &EsynetRouter::algorithmFTLR;
		m_bypass = true;
		break;
	case RA_ALTERTEST:
		m_curr_algorithm = &EsynetRouter::algorithmAlterTest;
		m_bypass = true;
		break;
	case RA_NONBLOCK:
		m_curr_algorithm = &EsynetRouter::algorithmNonBlock;
		m_bypass = true;
	case RA_FREESLOT:
		m_curr_algorithm = &EsynetRouter::algorithmNonBlock;
		m_bypass = false;
		break;
	}

	for ( long l_port = 0; l_port < m_router_cfg->portNum(); l_port ++ )
	{
		m_input_port.push_back(EsynetInputPort(&m_router_cfg->port(l_port)));
		m_output_port.push_back(EsynetOutputPort(&m_router_cfg->port(l_port)));
		if ( m_router_cfg->port( l_port ).totalVcNumber() < 1 ) 
		{
			m_neighbor_faulty[ l_port ] = true;
		}
        
		m_output_port[ l_port ].setOutputNeighborAddr( VCType(
			m_router_cfg->port( l_port ).neighborRouter(), 
			m_router_cfg->port( l_port ).neighborPort() ) ); 
        
		for ( int i = 0; i < m_network_cfg->routerCount(); i ++ )
		{
			for ( int j = 0; j < m_network_cfg->router( i ).portNum(); j ++ )
			{
				if ( m_network_cfg->router( i ).port( j ).neighborRouter() == 
						router_id && 
					m_network_cfg->router( i ).port( j ).neighborPort() == 
						l_port )
				{
					m_input_port[ l_port ].setInputNeighborAddr(
						VCType( i, j ) );
				}
			}
		} /* while( !router_cfg_reader->eof() ) */

		m_port_fault_units.push_back(EsynetPortFaultUnit(router_id, l_port));
		m_link_fault_units.push_back(EsynetLinkFaultUnit(
			EsyFaultConfigItem::FAULT_LINE, router_id, l_port, 
			m_network_cfg->dataPathWidth() ) );
		m_switch_fault_units.push_back(EsynetLinkFaultUnit(
			EsyFaultConfigItem::FAULT_SWITCH, router_id, l_port,
			m_network_cfg->dataPathWidth() ) );

		m_ecc_units.push_back(EsynetECCInterDecoder(
			router_id, l_port, m_network_cfg->dataPathWidth(), 
			false, m_argu_cfg->eccName(), m_argu_cfg->eccFipEnable() ) );

		for ( int vc = 0; vc < m_router_cfg->port( l_port ).inputVcNumber(); 
			 vc ++ )
		{
			m_vc_input_arbiter[ l_port ].push_back( EsynetArbiter(
				EsynetArbiter::ARBITER_MATRIX, 
				m_router_cfg->totalOutputVc() ) );
			m_vc_input_arbiter[ l_port ][ vc ].
				setOutputPortMap( m_network_cfg->router( router_id ) );
		}
		for ( int vc = 0; vc < m_router_cfg->port( l_port ).outputVcNumber(); 
			 vc ++ )
		{
			m_vc_output_arbiter[ l_port ].push_back( EsynetArbiter(
				EsynetArbiter::ARBITER_MATRIX, m_router_cfg->totalInputVc() ) );
			m_vc_output_arbiter[ l_port ][ vc ].
				setInputPortMap( m_network_cfg->router( router_id ) );
		}
		m_port_input_arbiter.push_back( EsynetArbiter(
			EsynetArbiter::ARBITER_MATRIX, 
			m_router_cfg->port( l_port ).inputVcNumber() ) );
		m_port_output_arbiter.push_back( EsynetArbiter(
			EsynetArbiter::ARBITER_MATRIX, m_router_cfg->totalInputVc() ) );
		m_port_output_arbiter[ l_port ].setInputPortMap( 
			m_network_cfg->router( router_id ) );
	} /* for (long l_port = 0; l_port < physic_ports_; l_port ++)  */
    
	if ( m_argu_cfg->bistEnable() )
	{
		bistOrderInit();
	}
	if ( m_argu_cfg->eccEnable() )
	{
		eccLocation( m_argu_cfg->eccStrategy() );
	}
}

/*************************************************
  Function: void sim_router_template::receive_credit(long phy, long vc)
  Description: physical port phy virtual channel vc gets a credit, increase
    the counter of this virtual channel
  Called by:
    void sim_foundation::receiveCreditMessage(MessEvent mesg)
  Inputs:
    long phy  physical port
    long vc   virtual channel
*************************************************/
void EsynetRouter::receiveCredit(long phy, long vc, long credit)
{
	m_output_port[ phy ][ vc ].setCreditCounter( credit );
	if ( m_router_fault_unit.currentStateOut() && m_bypass )
	{
		bypassReceiveCredit( phy, vc, credit );
	}
}

/*************************************************
  Function: void sim_router_template::receive_flit(long phy, long vc,
    EsynetFlit & flit)
  Description: receive a flit from port phy at virtual channel vc
  Called by:
    void sim_foundation::receiveWireMessage(MessEvent mesg)
  Inputs:
    long phy              physical port
    long vc               virtual channel
    EsynetFlit & flit  received flit
*************************************************/
void EsynetRouter::receiveFlit(long phy, long vc, const EsynetFlit & flit)
{
	EsynetFlit flit_t = flit;

	/* inject fault */
	if ( m_argu_cfg->faultInjectEnable() && 
		!m_router_cfg->port( phy ).networkInterface() )
	{
		portFaultInjection( phy, flit_t );
	}
	if ( m_router_fault_unit.currentStateOut() && m_bypass )
	{
		bypassReceiveFlit( phy, vc, flit );
		return;
	}
	/* skip testing packets, return credit direcly */
	if ( flit.testFlit() && flit.desAddr() == m_router_id &&
		vc < m_input_port[ phy ].vcNumber() - 1 )
	{
		long credit_id = m_input_port[ phy ].inputNeighborAddrId();
		long credit_port = m_input_port[ phy ].inputNeighborAddrPort();	
		addEvent( EsynetMessEvent::generateCreditMessage(
			m_current_time, m_router_id, phy, vc, credit_id, credit_port, vc,
			m_router_cfg->port( phy ).inputBuffer() - 
			m_input_port[ phy ][ vc ].flitCount() - 1 ) );
		return;
	}
    
	/* inject into ecc buffer */
	if ( m_ecc_units[ phy ].eccEnable() )
	{
		if ( m_argu_cfg->eccStrategy() == ECC_COUNT )
		{
			flit_t.eccCounterInc();
			if ( flit_t.eccCounter() >= m_argu_cfg->eccSpace() )
			{
				flit_t.eccCounterReset();
				m_ecc_units[ phy ].decoder( vc, flit_t );
			}
			else
			{
				receiveFlitInBuffer( phy, vc, flit_t );
			}
		}
		else
		{
			m_ecc_units[ phy ].decoder( vc, flit_t );
		}
	}
	/* inject into receive buffer directly */
	else
	{
		receiveFlitInBuffer( phy, vc, flit_t );
	}
}

void EsynetRouter::receiveFlitInBuffer(long phy, long vc, const EsynetFlit & flit)
{
	EsynetFlit flit_t = flit;
    
	/* insert flit into port phy virtual channel vc */
	m_input_port[ phy ][ vc ].addFlit( flit_t );
	/* power record */
	m_power_unit.powerBufferWrite( phy, flit_t.data() );
	/* insert ET_FLIT_RECEIVE event */
	addEvent(EsynetMessEvent(m_current_time,
		ET_FLIT_RECEIVE, -1, -1, -1, m_router_id, phy, vc, flit_t ) );
	/* head flit */
	if (flit_t.flitType() == EsynetFlit::FLIT_HEAD)
	{
		/* and there is no flit before it */
		if ( m_input_port[ phy ][ vc ].flitCount() == 1 )
		{
			/* change the state to ROUTING_ and insert ET_VC_STATE event */
			VCStateType oldstate = m_input_port[ phy ][ vc ].state();
			m_input_port[ phy ][ vc ].updateState( VCS_ROUTING );
			VCStateType newstate = m_input_port[ phy ][ vc ].state();
			addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
				m_router_id, phy, vc, m_router_id, phy, vc, 
				EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, oldstate, newstate, 
				0, 0, DataType() ) ) );
		} /* end of  if (input_module_.input(phy,vc).size() == 1) */
	} /* end of  if (flit.type() == EsynetFlit::FLIT_HEAD) */
	/* not head flit */
	else
	{
		/* and the state of port phy virtual channel vc is INIT_ */
		if ( m_input_port[ phy ][ vc ].state() == VCS_INIT )
		{
			/* change the state to SW_AB_ and insert ET_VC_STATE event */
			VCStateType oldstate = m_input_port[ phy ][ vc ].state();
			m_input_port[ phy ][ vc ].updateState( VCS_SW_AB );
			VCStateType newstate = m_input_port[ phy ][ vc ].state();
			addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
				m_router_id, phy, vc, m_router_id, phy, vc, 
				EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, oldstate, newstate, 
				0, 0, DataType() ) ) );
		} /* end of  if (input_module_.state(phy, vc) == INIT_) */
	} /* end of  else */
}

/*************************************************
 *  Function: void sim_router_template::routing_decision()
 *  Description: routing decision stage function, only two-dimension is 
 * supported.
 *************************************************/
void EsynetRouter::routingDecision()
{
	/* for other physical ports */
	for ( size_t pc_l = 0; pc_l < m_input_port.size(); pc_l ++ )
	{
		/* for each vc in physical port pc */
		for ( long vc_l = 0; vc_l < m_input_port[ pc_l ].vcNumber(); vc_l ++ )
		{
			EsynetFlit flit_t;
			
			/* for the FLIT_HEAD flit, the state is ROUTING_ */
			if( m_input_port[ pc_l ][ vc_l ].state() == VCS_ROUTING )
			{
				/* get source and destination and source address */
				flit_t = m_input_port[ pc_l ][ vc_l ].getFlit();
				long des_t = flit_t.desAddr();
				long sor_t = flit_t.sorAddr();
                
				/* remove all routing requirement and insert ET_ROUTING_END
					* event */
				for ( size_t seq = 0; seq < 
					m_input_port[ pc_l ][ vc_l ].routing().size(); seq ++ )
				{
					addEvent(EsynetMessEvent(m_current_time, ET_ROUTING_END,
						m_router_id, pc_l, vc_l, m_router_id,
						m_input_port[ pc_l ][ vc_l ].routing()[ seq ].first,
						m_input_port[ pc_l ][ vc_l ].routing()[ seq ].second, 
						flit_t ) );
				} /* end of  for (size_t seq = 0;  */
				m_input_port[ pc_l ][ vc_l ].clearRouting();
				/* remove all crouting requirement andvcSelection insert 
					* ET_CROUTING_END event */
				m_input_port[ pc_l ][ vc_l ].clearConnection();
				/* call routing algorithm to insert routing requirement and
					* insert ET_ROUTING event */
				
/*				if ( m_argu_cfg->faultInjectEnable() )
				{
					long sor_faultbit = flit_t.faultPattern()[0];
					long des_faultbit = flit_t.faultPattern()[0];
					if ( m_argu_cfg->eccName() == ECC_HM74 )
					{
						sor_faultbit = (sor_faultbit & 15) + 
							((sor_faultbit & 384) >> 3);
						des_faultbit = ((des_faultbit & 1536) >> 9) + 
							((des_faultbit & 245760) >> 12);
					}
					else if ( m_argu_cfg->eccName() == ECC_HM128 )
					{
						sor_faultbit = sor_faultbit & 63;
						des_faultbit = ((des_faultbit & 192) >> 6) + 
							((des_faultbit & 61440) >> 10);
					}
					else
					{
						sor_faultbit = sor_faultbit & 63;
						des_faultbit = (des_faultbit & 4032) >> 6;
					}
					sor_t = sor_t ^ sor_faultbit;
					des_t = des_t ^ des_faultbit;
					if (sor_faultbit > 0 || des_faultbit > 0)
					{
						flit_t.setDrop();
					}
				}*/
				
				(this->*m_curr_algorithm)( des_t, sor_t, pc_l, vc_l );
				if ( m_input_port[ pc_l ][ vc_l ].routing().size() > 0 )
				{
					for ( size_t seq = 0; seq < 
						m_input_port[ pc_l ][ vc_l ].routing().size(); seq ++ )
					{
						addEvent(EsynetMessEvent(m_current_time, ET_ROUTING,
							m_router_id, pc_l, vc_l, m_router_id,
							m_input_port[ pc_l ][ vc_l ].routing()[ seq ].first,
							m_input_port[ pc_l ][ vc_l ].routing()[ seq ].
								second, 
							flit_t ) );
					} /* end of  for (size_t seq = 0;  */
					/* change the state to VC_AB_, insert ET_VC_STATE event */
					VCStateType oldstate = m_input_port[ pc_l ][ vc_l ].state();
					m_input_port[ pc_l ][ vc_l ].updateState( VCS_VC_AB );
					VCStateType newstate = m_input_port[ pc_l ][ vc_l ].state();
					addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
						m_router_id, pc_l, vc_l, m_router_id, pc_l, vc_l,  
						EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, oldstate, 
						newstate, 0, 0, DataType() ) ) );
				}
			} /* end of  if(input_module_.state(pc_l, vc_l) == ROUTING_) */
		} /* for(long vc_l = 0; vc_l < vc_number_; vc_l++) */
	} /* end of  for(long i = 1; i < physic_ports_; i ++) */
}

// *****************************************************//
// vc arbitration algorithms, which is the implementation//
// of selection function in routing                     //
// *****************************************************//
//***************************************************************************//
//choose one of the candidate routing vc
pair< long, long > EsynetRouter::vcSelection(long a, long b)
{
	const vector< VCType > & vc_can_t = m_input_port[ a ][ b ].routing();
	long r_size_t = (long)vc_can_t.size();

	for ( long i = 0; i < r_size_t; i ++ )
	{
		VCType v_t = vc_can_t[ i ];
		bool port_avail = false;
		if ( m_argu_cfg->vcShare() == VC_SHARE )
		{
			if ( ( m_output_port[ v_t.first ][ v_t.second ].usage() == 
					VC_FREE ) )
			{
				port_avail = true;
			}
		}
		else
		{
			if ( m_output_port[ v_t.first ][ v_t.second ].creditCounter() == 
				m_input_port[ v_t.first ].inputBufferSize() )
			{
				port_avail = true;
			}
		}

		if ( port_avail )
		{
			if ( m_argu_cfg->bistEnable() )
			{
				if ( !( m_bist_port_state[ vc_can_t[ i ].first ] == 
					BIST_PORT_EMPTY && 
					m_input_port[ a ][ b ].getFlit().flitType() 
					== EsynetFlit::FLIT_HEAD ) )
				{
					m_vc_input_arbiter[ a ][ b ].setRequest( vc_can_t[ i ] );
				}
			}
			else
			{
				m_vc_input_arbiter[ a ][ b ].setRequest( vc_can_t[ i ] );
			}
		}
	}
    
	VCType win = m_vc_input_arbiter[ a ][ b ].grantVc();
	if ( win.first >= 0 && win.second >= 0 )
	{
		return win;
	}
	else
	{
		return VC_NULL;
	}
}

//***************************************************************************//
void EsynetRouter::vcArbitration()
{
	AtomType vc_request = 0;
	
	for ( size_t inphy = 0; inphy < m_input_port.size(); inphy ++ )
	{
		for ( long invc = 0; invc < m_input_port[ inphy ].vcNumber(); invc ++ )
		{
			VCType vc_t;
			if ( m_input_port[ inphy ][ invc ].state() == VCS_VC_AB )
			{
				vc_t = vcSelection( inphy, invc );

				if ( ( vc_t.first >= 0 ) && ( vc_t.second >= 0 ) )
				{
					m_vc_output_arbiter[ vc_t.first ][ vc_t.second ].
						setRequest( VCType( inphy, invc ) );
					vc_request = vc_request | 
						(0x1LL << inphy * m_router_cfg->maxVcNum() + invc);
				}
			}
		}
	}
    
	for ( size_t outphy = 0; outphy < m_output_port.size(); outphy ++ )
	{
		for ( long outvc = 0; outvc < m_output_port[ outphy ].vcNumber();
				outvc ++ )
		{
			if ( m_output_port[ outphy ][ outvc ].usage() == VC_FREE )
			{
				if ( m_argu_cfg->bistEnable() )
				{
					for ( int inphy = 0; inphy < m_input_port.size(); inphy ++ )
					{
						for ( int invc = 0; 
							invc < m_input_port[ inphy ].vcNumber(); invc ++ )
						{
							if ( m_bist_port_state[ inphy ] == BIST_PORT_EMPTY )
							{
								m_vc_output_arbiter[ outphy ][ outvc ].
									setHighestPriority( VCType( inphy, invc ) );
							}
							else if ( m_bist_port_state[ outphy ] == 
								BIST_PORT_BLOCK && 
								invc == m_router_cfg->port( inphy ).
									inputVcNumber() - 1 ) 
							{
								m_vc_output_arbiter[ outphy ][ outvc ].
									setHighestPriority( VCType( inphy, invc ) );
							}
							else if ( m_bist_port_state[ outphy ] == 
								BIST_PORT_FREE && 
								invc == m_router_cfg->port( inphy ).
									inputVcNumber() - 1 ) 
							{
								m_vc_output_arbiter[ outphy ][ outvc ].
									setLowestPriority( VCType( inphy, invc ) );
							}
						}
					}
				}

				VCType vc_win = 
					m_vc_output_arbiter[ outphy ][ outvc ].grantVc();
				if ( vc_win.first >= 0 && vc_win.second >= 0 )
				{
					VCStateType oldstate = 
						m_input_port[ vc_win.first ][ vc_win.second ].state();
					m_input_port[ vc_win.first ][ vc_win.second ].
						updateState( VCS_SW_AB );
					VCStateType newstate = 
						m_input_port[ vc_win.first ][ vc_win.second ].state();
					addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
						m_router_id, vc_win.first, vc_win.second, 
						m_router_id, vc_win.first, vc_win.second,  
						EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, oldstate, 
						newstate, 0, 0, DataType() ) ) );

					m_input_port[ vc_win.first ][ vc_win.second ].
							assignConnection( VCType( outphy, outvc ) );
					m_output_port[ outphy ][ outvc ].acquire( vc_win );
					addEvent(EsynetMessEvent(m_current_time, ET_ASSIGN,
						m_router_id, vc_win.first, vc_win.second, 
						m_router_id, outphy, outvc, EsynetFlit()));
					m_power_unit.powerVCArbiter( outphy, outvc, vc_request,
						vc_win.first * m_input_port[ 
							vc_win.first ].vcNumber() + vc_win.second );
				}
			}
			else
			{
				m_vc_output_arbiter[ outphy ][ outvc ].clearRequest();	
			}
		}
	}
}
//***************************************************************************//

//***************************************************************************//
//switch arbitration pipeline stage
void EsynetRouter::swArbitration()
{
	for ( size_t inphy = 0; inphy < m_input_port.size(); inphy ++ )
	{
		for( long invc = 0; invc < m_input_port[ inphy ].vcNumber(); invc ++ )
		{
			if( m_input_port[ inphy ][ invc ].state() == VCS_SW_AB ) 
			{
				VCType out_t = m_input_port[ inphy ][ invc ].connection();
				if( m_output_port[ out_t.first ][ out_t.second ].
					creditCounter() > 0 )
				{
					m_port_input_arbiter[ inphy ].setRequest( invc );
				}
			}
		}

		long win_t = m_port_input_arbiter[ inphy ].grant();
		if ( win_t >= 0 )
		{
			VCType r_t = m_input_port[ inphy ][ win_t ].connection();
			m_port_output_arbiter[ r_t.first ].setRequest( 
				VCType( inphy, win_t ) );
		}
	}

	for( size_t outph = 0; outph < m_output_port.size(); outph ++ )
	{
		VCType vc_win = m_port_output_arbiter[ outph ].grantVc();
		if ( vc_win.first >= 0 && vc_win.second >= 0 )
		{
			VCStateType oldstate = 
				m_input_port[ vc_win.first ][ vc_win.second ].state();
			m_input_port[ vc_win.first ][ vc_win.second ].updateState( 
				VCS_SW_TR );
			VCStateType newstate = 
				m_input_port[ vc_win.first ][ vc_win.second ].state();
			addEvent( EsynetMessEvent( m_current_time, ET_VC_STATE,
				m_router_id, vc_win.first, vc_win.second, 
				m_router_id, vc_win.first, vc_win.second, 
				EsynetFlit( 0, 0, (EsynetFlit::FlitType)0, 
					oldstate, newstate, 0, 0, DataType() ) ) );
		}
	}
}

//***************************************************************************//
//flit out buffer to the output buffer
void EsynetRouter::flitOutbuffer()
{
	for( size_t phy = 0; phy < m_input_port.size(); phy ++ )
	{
		for( long vc = 0; vc < m_input_port[ phy ].vcNumber(); vc ++ )
		{
			if( m_input_port[ phy ][ vc ].state() == VCS_SW_TR ) 
			{
				VCType out_t = m_input_port[ phy ][ vc ].connection();
				m_output_port[ out_t.first ][ out_t.second ].decCreditCounter();

				long credit_id = m_input_port[ phy ].inputNeighborAddrId();
				long credit_port = m_input_port[ phy ].inputNeighborAddrPort();
				if ( credit_id >= 0 && credit_port >= 0 )
				{
					addEvent( EsynetMessEvent::generateCreditMessage(
						m_current_time + CREDIT_DELAY_,
						m_router_id, phy, vc, credit_id, credit_port, vc,
						m_router_cfg->port( phy ).inputBuffer() - 
						m_input_port[ phy ][ vc ].flitCount() - 1 ) );
				}
				long in_size_t = 
					(long)m_input_port[ phy ][ vc ].flitCount();
				EsynetFlit flit_t = m_input_port[phy][vc].getFlit();
				m_input_port[ phy ][ vc ].removeFlit();
				m_power_unit.powerBufferRead( phy, flit_t.data());
				m_power_unit.powerCrossbarTraversal( phy, out_t.first, 
                    flit_t.data() );

				if ( m_argu_cfg->faultInjectEnable() )
				{
					switchFaultInjection( out_t.first, flit_t );
				}
				m_output_port[ out_t.first ].addFlit( flit_t );
				addEvent(EsynetMessEvent(m_current_time, ET_FLIT_SWITCH,
					m_router_id, phy, vc, m_router_id, out_t.first, 
					out_t.second, flit_t) );

				m_output_port[ out_t.first ].addAdd( out_t);
				if (flit_t.flitType() == EsynetFlit::FLIT_TAIL ||
					flit_t.flitSize() == 1 ) 
				{
					addEvent(EsynetMessEvent(m_current_time, ET_ASSIGN_END,
						m_router_id, 
						m_output_port[ out_t.first ][  out_t.second ].
							assign().first, 
						m_output_port[ out_t.first ][ out_t.second ].
							assign().second, 
						m_router_id, out_t.first, out_t.second, EsynetFlit()));
					m_output_port[ out_t.first ][ out_t.second ].release();
				}
				if( in_size_t > 1 ) 
				{
					if (flit_t.flitType() == EsynetFlit::FLIT_TAIL ||
						flit_t.flitSize() == 1 ) 
					{
						if( m_argu_cfg->vcShare() == VC_MONO ) 
						{
							if ( in_size_t != 1 ) 
							{
								cout<< phy <<":"<<in_size_t<<endl;
							}
						}
						VCStateType oldstate = 
							m_input_port[ phy ][ vc ].state();
						m_input_port[ phy ][ vc ].updateState( VCS_ROUTING );
						VCStateType newstate = 
							m_input_port[ phy ][ vc ].state();
						addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
							m_router_id, phy, vc, m_router_id, phy, vc, 
							EsynetFlit(0, 0, (EsynetFlit::FlitType)0, oldstate, 
							newstate, 0, 0, DataType() ) ) );
					}
					else 
					{
						VCStateType oldstate = 
							m_input_port[ phy ][ vc ].state();
						m_input_port[ phy ][ vc ].updateState( VCS_SW_AB );
						VCStateType newstate = 
							m_input_port[ phy ][ vc ].state();
						addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
							m_router_id, phy, vc, m_router_id, phy, vc, 
							EsynetFlit(0, 0, (EsynetFlit::FlitType)0, oldstate, 
							newstate, 0, 0, DataType() ) ) );
					}
				}
				else 
				{
					VCStateType oldstate = 
						m_input_port[ phy ][ vc ].state();
					m_input_port[ phy ][ vc ].updateState( VCS_INIT );
					VCStateType newstate = 
						m_input_port[ phy ][ vc ].state();
					addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
						m_router_id, phy, vc, m_router_id, phy, vc, 
						EsynetFlit(0, 0, (EsynetFlit::FlitType)0, oldstate, 
						newstate, 0, 0, DataType() ) ) );
				}
			}
		}
	}
}

//***************************************************************************//
//flit traversal through link 
void EsynetRouter::flitTraversal()
{
	for ( size_t phy = 0; phy < m_output_port.size(); phy ++ )
	{
		if ( m_output_port[ phy ].flitCount() > 0 ) 
		{
			double flit_delay_t = WIRE_DELAY_ + m_current_time;
			long wire_add_t = m_output_port[ phy ].outputNeighborAddrId();
			long wire_pc_t = m_output_port[ phy ].outputNeighborAddrPort();

			EsynetFlit flit_t = m_output_port[phy].getFlit();

			VCType outadd_t = m_output_port[ phy ].getAdd();
			m_power_unit.powerLinkTraversal( phy, flit_t.data() );

			m_output_port[ phy ].removeFlit();
			m_output_port[ phy ].removeAdd();
			m_output_port[ phy ].setFlitOnLink();
			
			addEvent(EsynetMessEvent::generateWireMessage(
				flit_delay_t, m_router_id, phy, 0,  wire_add_t, wire_pc_t,
				outadd_t.second, flit_t ) );
			addEvent(EsynetMessEvent(m_current_time, ET_FLIT_SEND,
				m_router_id, phy, outadd_t.second, -1, -1, -1, flit_t) );
		}
	}
}
//***************************************************************************//
//routing pipeline stages 
void EsynetRouter::routerSimPipeline()
{
	/* fault injection function */
	if ( m_argu_cfg->faultInjectEnable() )
	{
		faultStateUpdate();
	}

	if ( !m_router_fault_unit.currentStateOut() )
	{
		/* stage 5 flit traversal */
		flitTraversal();
		/* stage 4 flit output buffer */
		flitOutbuffer();
		/* stage 3 swETh arbitration */
		swArbitration();
		/* stage 2, vc arbitration */
		vcArbitration();
		/* stage 1, routing decision */
		routingDecision();
	}

	if ( m_argu_cfg->bistEnable() )
	{
		bistStateMachine();
	}
	if ( m_argu_cfg->eccEnable() )
	{
		eccBuffer();
	}
}
