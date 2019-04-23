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

#define NONBLOCK_TEST_TIME	0
#define NONBLOCK_FREE_TIME	1
#define NONBLOCK_BLOCK_TIME	2

#define NONBLOCK_PACKET_SIZE 0
#define NONBLOCK_PACKET_COUNT 1

void EsynetRouter::bistOrderInit()
{
	long order = 0;

	long t_x = m_router_addr[ EsyNetworkCfg::AX_X ] / 2;
	long t_y = m_router_addr[ EsyNetworkCfg::AX_Y ] / 2;
	long t_router_id = t_x + t_y * 
		m_network_cfg->size( EsyNetworkCfg::AX_X ) / 2;
	long t_group = m_router_addr[ EsyNetworkCfg::AX_Y ] % 2 * 2 + 
				m_router_addr[ EsyNetworkCfg::AX_X ] % 2;
	order = t_router_id + t_group * m_network_cfg->routerCount() / 4;

	long order_offset = (order + m_argu_cfg->bistOffset()) % 
		m_network_cfg->routerCount();
	m_bist_interval_timer = m_argu_cfg->bistInterval() - 
		(m_argu_cfg->bistInterval() / m_network_cfg->routerCount()) * 
		(order_offset + 1);
}

bool EsynetRouter::bistEmptyCheck()
{
	for ( size_t port = 0; port < m_input_port.size(); port ++ )
	{
		if ( !m_input_port[ port ].isEmpty() )
		{
			return false;
		}
		if ( !m_output_port[ port ].isEmpty() )
		{
			return false;
		}
		if ( !m_bist_neighbor_empty_ack[ port ] )
		{
			return false;
		}
	}
	return true;
}

void EsynetRouter::bistStateMachine()
{
	m_bist_interval_timer ++;

	BISTRouterState next_state = m_bist_router_state;
	if ( m_bist_router_state == BIST_ROUTER_NORMAL )
	{
		if ( m_bist_interval_timer >= m_argu_cfg->bistInterval() )
		{
			m_bist_interval_timer = 0;
			
			if ( m_argu_cfg->bistTypeCode() == BIST_TARRA )
			{
				next_state = BIST_ROUTER_EMPTY;
				m_statistics_unit.increaseBistEmptyCounter();
			}
			else if ( m_argu_cfg->bistTypeCode() == BIST_BLOCKING )
			{
				next_state = BIST_ROUTER_TESTING;
				m_bist_duration_timer = 0;
			}
			else if ( m_argu_cfg->bistTypeCode() == BIST_NONBLOCK ||
                m_argu_cfg->bistTypeCode() == BIST_FREESLOT )
			{
				next_state = BIST_ROUTER_FREE;
				m_bist_duration_timer = 0;
			}
		}
	}
	else if ( m_bist_router_state == BIST_ROUTER_EMPTY )
	{
		m_statistics_unit.increaseBistEmptyCycle( 1.0 );
		
		if ( bistEmptyCheck() )
		{
			m_bist_duration_timer = 0;
			next_state = BIST_ROUTER_TESTING;
		}
	}
	else if ( m_bist_router_state == BIST_ROUTER_TESTING )
	{
		m_bist_duration_timer ++;
		if ( m_bist_duration_timer >= m_argu_cfg->bistDuration() )
		{
			m_bist_duration_timer = 0;
			if ( m_argu_cfg->bistTypeCode() == BIST_BLOCKING ||
                m_argu_cfg->bistTypeCode() == BIST_FREESLOT )
			{
				next_state = BIST_ROUTER_NORMAL;
			}
			else
			{
				next_state = BIST_ROUTER_RECOVER;
				m_statistics_unit.increaseBistRecoverCounter();
			}
		}
	}
	else if ( m_bist_router_state == BIST_ROUTER_RECOVER )
	{
		m_statistics_unit.increaseBistRecoverCycle( 1.0 );

		if ( bistEmptyCheck() )
		{
			next_state = BIST_ROUTER_NORMAL;
		}
	}
	else if ( m_bist_router_state == BIST_ROUTER_FREE )
	{
		m_bist_duration_timer ++;
		if ( m_bist_duration_timer >= m_argu_cfg->bistTiming()[ 
			NONBLOCK_FREE_TIME ] )
		{
			m_bist_duration_timer = 0;
			next_state = BIST_ROUTER_BLOCK;
		}
	}
	else if ( m_bist_router_state == BIST_ROUTER_BLOCK )
	{
		m_bist_duration_timer ++;
		if ( m_bist_duration_timer >= m_argu_cfg->bistTiming()[ 
			NONBLOCK_BLOCK_TIME ] )
		{
            m_bist_duration_timer = 0;
            if ( m_argu_cfg->bistTypeCode() == BIST_NONBLOCK )
            {
                next_state = BIST_ROUTER_EMPTY;
                m_statistics_unit.increaseBistEmptyCounter();
            }
            else
            {
                next_state = BIST_ROUTER_TESTING;
            }
		}
	}
	
	if ( m_bist_router_state == next_state )
	{
		return;
	}

	if ( next_state == BIST_ROUTER_TESTING )
	{
		m_router_fault_unit.setConstantState( true );
		addEvent(EsynetMessEvent(m_current_time, ET_FT_INJECTION,
			EsyFaultConfigItem::FAULT_ROUTER, m_router_id, -1, -1, -1, -1,
			EsynetFlit()));	
	}
	else if ( next_state == BIST_ROUTER_NORMAL )
	{
		m_router_fault_unit.setConstantState( false );
		addEvent(EsynetMessEvent(m_current_time, ET_FT_RECOVERY,
			EsyFaultConfigItem::FAULT_ROUTER, m_router_id, -1, -1, -1, -1,
			EsynetFlit()));
	}

	for ( size_t port = 0; port < m_input_port.size(); port ++ )
	{
		if ( next_state == BIST_ROUTER_EMPTY ||
			next_state == BIST_ROUTER_RECOVER )
		{
			m_port_fault_units[ port ].setConstantState( true );
			addEvent(EsynetMessEvent(m_current_time, ET_FT_INJECTION,
				EsyFaultConfigItem::FAULT_PORT, m_router_id, port, 
				-1, -1, -1, EsynetFlit()));
		}
		else if ( next_state == BIST_ROUTER_TESTING ||
			next_state == BIST_ROUTER_NORMAL )
		{
			m_port_fault_units[ port ].setConstantState( false );
			addEvent(EsynetMessEvent(m_current_time, ET_FT_RECOVERY,
				EsyFaultConfigItem::FAULT_PORT, m_router_id, port, -1, -1, 
				-1, EsynetFlit()));
		}
		
        for( long vc = 0; vc < m_input_port[ port ].vcNumber(); vc ++ )
        {
			if ( next_state == BIST_ROUTER_TESTING )
			{
				long inphy = bypassOutput2Input( VCType( port, vc ) ).first;
				if ( inphy >= 0 )
				{
					long credit_id = m_input_port[ inphy ].
						inputNeighborAddrId();
					long credit_port = m_input_port[ inphy ].
						inputNeighborAddrPort();
					if ( credit_id >= 0 && credit_port >= 0 )
					{
						addEvent( EsynetMessEvent::generateCreditMessage(
							m_current_time + CREDIT_DELAY_,
							m_router_id, inphy, vc, credit_id, credit_port, vc,
							creditCounter( port, vc ) - 1 ) );
					}
				}
			}
			else if ( next_state == BIST_ROUTER_NORMAL )
			{
				long credit_id = m_input_port[ port ].inputNeighborAddrId();
				long credit_port = m_input_port[ port ].inputNeighborAddrPort();
				if ( credit_id >= 0 && credit_port >= 0 )
				{
					addEvent( EsynetMessEvent::generateCreditMessage(
						m_current_time + CREDIT_DELAY_,
						m_router_id, port, vc, credit_id, credit_port, vc,
						m_router_cfg->port( port ).inputBuffer() - 
						m_input_port[ port ][ vc ].flitCount() - 1 ) );
				}
			}
		}
	}
	
	m_bist_router_state = next_state;
}

BISTPortState EsynetRouter::bistPortState( long phy )
{
	if ( m_argu_cfg->bistEnable() )
	{
		if ( m_argu_cfg->bistTypeCode() == BIST_TARRA ||
			m_argu_cfg->bistTypeCode() == BIST_NONBLOCK ||
            m_argu_cfg->bistTypeCode() == BIST_FREESLOT)
		{
			switch( m_bist_router_state )
			{
				case BIST_ROUTER_EMPTY:
				case BIST_ROUTER_RECOVER: return BIST_PORT_EMPTY; break;
				case BIST_ROUTER_FREE: return BIST_PORT_FREE; break;
				case BIST_ROUTER_BLOCK: return BIST_PORT_BLOCK; break;
				default: return BIST_PORT_NORMAL;
			}
		}
		else
		{
//			switch( m_bist_router_state )
//			{
//				case BIST_ROUTER_NORMAL: return BIST_PORT_NORMAL; break;
//				default: return BIST_PORT_EMPTY; break;
//			}
			return BIST_PORT_NORMAL;
		}
	}
	else
	{
		return BIST_PORT_NORMAL;
	}
}
        
void EsynetRouter::setBistPortState(long int phy, BISTPortState state)
{
	bool trigger = false;
	if ( ( (m_bist_port_state[ phy ] != BIST_PORT_EMPTY) && 
		(state == BIST_PORT_EMPTY) ) || 
		( (m_bist_port_state[ phy ] == BIST_PORT_EMPTY) && 
		(state != BIST_PORT_EMPTY) ) )
	{
		for ( size_t inphy = 0; inphy < m_input_port.size(); inphy ++ )
		{
			for ( long invc = 0; invc < m_input_port[ inphy ].vcNumber(); 
					invc ++ )
			{
				if ( m_input_port[ inphy ][ invc ].state() == VCS_VC_AB )
				{
					VCStateType oldstate = 
						m_input_port[ inphy ][ invc ].state();
					m_input_port[ inphy ][ invc ].updateState( VCS_ROUTING );
					VCStateType newstate = 
						m_input_port[ inphy ][ invc ].state();
					addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
						m_router_id, inphy, invc, m_router_id, inphy, invc, 
						EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, 
						oldstate, newstate, 0, 0, DataType() ) ) );
				}
				if ( m_input_port[ inphy ][ invc ].state() == VCS_SW_AB )
				{
					if ( m_input_port[ inphy ][ invc ].getFlit().flitType() == 
										EsynetFlit::FLIT_HEAD )
					{
						VCType out_t = 
							m_input_port[ inphy ][ invc ].connection();
						m_output_port[ out_t.first ][ out_t.second ].release();
						VCStateType oldstate = 
							m_input_port[ inphy ][ invc ].state();
						m_input_port[ inphy ][ invc ].updateState( 
							VCS_ROUTING );
						VCStateType newstate = 
							m_input_port[ inphy ][ invc ].state();
						addEvent(EsynetMessEvent(m_current_time, ET_VC_STATE,
							m_router_id, inphy, invc, m_router_id, inphy, invc, 
							EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, 
							oldstate, newstate, 0, 0, DataType() ) ) );
					}
				}
			}
		}
	}
    else if ( (m_bist_port_state[ phy ] != BIST_PORT_FREE ) &&
		(state == BIST_PORT_FREE) )
	{
		long bist_phy = phy;
		long bist_vc = m_input_port[ phy ].vcNumber() - 1;
		long bist_id = m_input_port[ phy ].inputNeighborAddrId();
		vector< long > bist_addr = m_network_cfg->seq2Coord( bist_id );
		
		for ( int turn = 0; turn < m_router_cfg->portNum(); turn ++ )
		{
			long bist_dst;
			switch( (phy + turn) % m_router_cfg->portNum() )
			{
				case 0: bist_dst = bist_id; break;
				case 1: 
					if ( bist_addr[ EsyNetworkCfg::AX_Y ] == 0 )
					{
						continue;
					}
					else
					{
						bist_dst = bist_id - 
							m_network_cfg->size( EsyNetworkCfg::AX_X );
					}
					break;
				case 2:
					if ( bist_addr[ EsyNetworkCfg::AX_Y ] == 
							m_network_cfg->size( EsyNetworkCfg::AX_Y ) - 1 )
					{
						continue;
					}
					else
					{
						bist_dst = bist_id +
							m_network_cfg->size( EsyNetworkCfg::AX_X );
					}
					break;
				case 3:
					if ( bist_addr[ EsyNetworkCfg::AX_X ] == 0 )
					{
						continue;
					}
					else
					{
						bist_dst = bist_id - 1;
					}
					break;
				case 4:
					if ( bist_addr[ EsyNetworkCfg::AX_X ] ==
							m_network_cfg->size( EsyNetworkCfg::AX_X ) - 1 )
					{
						continue;
					}
					else
					{
						bist_dst = bist_id + 1;
					}
					break;
				default: continue; break;
			}
			

			/* initalization data */
			long flit_size = m_network_cfg->flitSize( ATOM_WIDTH_ );
			DataType init_data( flit_size );
			for ( long l_byte = 0; l_byte < flit_size; l_byte ++ )
			{
				init_data[ l_byte ] = 
					EsynetSRGenFlatUnsignedLongLong(0, MAX_64_);
			} /* for ( long l_byte = 0; l_byte < m_flit_size; l_byte ++ ) */   
	
			long packet_count = m_argu_cfg->bistFlit()[ NONBLOCK_PACKET_COUNT ];
			long packet_size = m_argu_cfg->bistFlit()[ NONBLOCK_PACKET_SIZE ];
			for ( int pac = 0; pac < packet_count; pac ++ )
			{
				for (long l = 0; l < packet_size; l ++)
				{
					DataType flit_data;
					/* set flit data */
					for(long i = 0; i < 
						m_network_cfg->flitSize( ATOM_WIDTH_ ); i ++)
					{
						init_data[i] = static_cast<AtomType>( 
							init_data[i] * CORR_EFF_ 
							+ EsynetSRGenFlatUnsignedLongLong(0, MAX_64_));
						/* set data in flit */
						flit_data.push_back(init_data[i]);
					} /* end of  for(long i = 0; i < flit_size_; i ++) */
					/* head flit */
					if (l == 0)
					{
						/* insert head flit */
						addEvent( EsynetMessEvent::generateWireMessage(
							m_current_time, m_router_id, bist_phy, bist_vc, 
							m_router_id, bist_phy, bist_vc, EsynetFlit(
								pac, packet_size, 
								EsynetFlit::FLIT_HEAD, m_router_id, bist_dst,
								m_current_time, m_current_time, flit_data,
								EsynetFlit::FLIT_TESTING ) ) );
					} /* end of  if (l == 0) */
					/* tail flit */
					else if( l == ( packet_size - 1) )
					{
						/* insert tail flit */
						addEvent( EsynetMessEvent::generateWireMessage(
							m_current_time, m_router_id, bist_phy, bist_vc, 
							m_router_id, bist_phy, bist_vc, EsynetFlit(
								pac, packet_size, 
								EsynetFlit::FLIT_TAIL, m_router_id, bist_dst,
								m_current_time, m_current_time, flit_data,
								EsynetFlit::FLIT_TESTING ) ) );
					} /* end of  else if(l == (size - 1) ) */
					else 
					{
						/* insert body flit */
						addEvent( EsynetMessEvent::generateWireMessage(
							m_current_time, m_router_id, bist_phy, bist_vc, 
							m_router_id, bist_phy, bist_vc, EsynetFlit(
								pac, packet_size, 
								EsynetFlit::FLIT_BODY, m_router_id, bist_dst,
								m_current_time, m_current_time, flit_data,
								EsynetFlit::FLIT_TESTING ) ) );
					} /* end of  else */
				}
			}
		}
	}
    else if ( (m_bist_port_state[ phy ] == BIST_PORT_BLOCK ) &&
		(state != esynet::BIST_PORT_BLOCK) )
	{
		long bist_phy = phy;
		long bist_vc = m_input_port[ phy ].vcNumber() - 1;

		bool head_found = false;
		for ( int i = 0; i < m_input_port[ bist_phy ][ bist_vc ].flitCount(); 
			 i ++ )
		{
			EsynetFlit flit_t = m_input_port[ bist_phy ][ bist_vc ].getFlit();
			head_found = ( flit_t.flitType() == EsynetFlit::FLIT_HEAD );
			
			if ( !head_found )
			{
				m_input_port[ bist_phy ][ bist_vc ].addFlit( flit_t );
			}
		}
	}
    
	m_bist_port_state[ phy ] = state;
}
