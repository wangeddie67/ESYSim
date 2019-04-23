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

#define AX_X EsyNetworkCfg::AX_X
#define AX_Y EsyNetworkCfg::AX_Y

#define PLOCALPORT     0
#define PNORTHPORT     1
#define PNORTH1PORT    PNORTHPORT
#define PSOUTHPORT     2
#define PSOUTH1PORT    PSOUTHPORT
#define PWESTPORT      3
#define PEASTPORT      4
#define PNORTH2PORT    5
#define PSOUTH2PORT    6

VCType EsynetRouter::bypassInput2Output( const VCType & input )
{
	long outphy = -1;
	if ( m_argu_cfg->routingAlg() == esynet::RA_CORERESCUER )
	{
		switch ( input.first )
		{
		case PLOCALPORT:
			outphy = ( m_router_addr[AX_Y] == 0 )?PSOUTH1PORT:PNORTH1PORT; 
			break;
		case PNORTH1PORT: outphy = PSOUTH1PORT; break;
		case PNORTH2PORT: outphy = PLOCALPORT; break;
		case PSOUTH1PORT: outphy = PSOUTH2PORT; break;
		case PSOUTH2PORT:
			outphy = ( m_router_addr[AX_Y] == 0 )?PLOCALPORT:PNORTH2PORT;
			break;
		case PWESTPORT: outphy = PEASTPORT; break;
		case PEASTPORT: outphy = PWESTPORT; break;
		}
	}
	else if ( m_argu_cfg->routingAlg() == esynet::RA_ERESCUER )
	{
		switch ( input.first )
		{
		case PLOCALPORT:
			outphy = ( m_router_addr[AX_Y] == 0 )?PSOUTH1PORT:PNORTH2PORT; 
			break;
		case PNORTH1PORT: outphy = PSOUTH1PORT; break;
		case PSOUTH1PORT:
			outphy = ( m_router_addr[AX_Y] == 0 )?PLOCALPORT:PNORTH1PORT;
			break;
		case PWESTPORT: outphy = PEASTPORT; break;
		case PEASTPORT: outphy = PWESTPORT; break;
		case PNORTH2PORT: outphy = PLOCALPORT; break;
		case PSOUTH2PORT: outphy = PSOUTH2PORT; break;
		}
	}
	else if ( m_argu_cfg->routingAlg() == esynet::RA_ALTERTEST )
	{
		long northport = (bistEnableChannel())?PNORTH2PORT:PNORTH1PORT;
		long southport = (bistEnableChannel())?PSOUTH2PORT:PSOUTH1PORT;
		switch ( input.first )
		{
		case PLOCALPORT:
			outphy = ( m_router_addr[AX_Y] == 0 )?southport:northport; 
			break;
		case PNORTH1PORT:
		case PSOUTH1PORT:
		case PNORTH2PORT:
		case PSOUTH2PORT: outphy = PLOCALPORT; break;
		}
	}
	else if ( m_argu_cfg->routingAlg() == esynet::RA_NONBLOCK )
	{
		switch( input.first )
		{
		case PLOCALPORT: outphy = 
			( m_router_addr[ AX_X ] == m_network_cfg->size( AX_X ) - 1 )?
				PWESTPORT:PEASTPORT; break;
		case PNORTHPORT: outphy = PSOUTHPORT; break;
		case PSOUTHPORT: outphy = PNORTHPORT; break;
		case PWESTPORT: outphy =
			( m_router_addr[ AX_X ] == m_network_cfg->size( AX_X ) - 1 )?
				PLOCALPORT:PWESTPORT; break;
		case PEASTPORT: outphy = PLOCALPORT; break;
		}
	}
	return VCType( outphy, input.second );
}

VCType EsynetRouter::bypassOutput2Input( const VCType & output )
{
	return bypassInput2Output( output );
	long inphy = -1;
	if ( m_argu_cfg->routingAlg() == esynet::RA_CORERESCUER ||
		m_argu_cfg->routingAlg() == esynet::RA_ERESCUER ||
		m_argu_cfg->routingAlg() == esynet::RA_ALTERTEST ||
		m_argu_cfg->routingAlg() == esynet::RA_NONBLOCK )
	{
		return bypassInput2Output( output );
	}
	return VCType( inphy, output.second );
}

void EsynetRouter::bypassReceiveFlit(long a, long b, const EsynetFlit & c)
{
	VCType output = bypassInput2Output( VCType( a, b ) );
	long phy = output.first;

	double flit_delay_t = WIRE_DELAY_ + m_current_time;
	long wire_add_t = m_output_port[ phy ].outputNeighborAddrId();
	long wire_pc_t = m_output_port[ phy ].outputNeighborAddrPort();

	m_power_unit.powerLinkTraversal( phy, c.data() );

	addEvent(EsynetMessEvent::generateWireMessage(
		flit_delay_t, m_router_id, output.first, output.second, 
		wire_add_t, wire_pc_t, output.second, c ) );
	
	if ( c.flitType() == EsynetFlit::FLIT_HEAD )
	{
		addEvent( EsynetMessEvent( m_current_time, ET_ASSIGN,
			m_router_id, a, b, m_router_id, output.first, output.second, 
			EsynetFlit() ) );
	}
	else if ( c.flitType() == EsynetFlit::FLIT_TAIL )
	{
		addEvent( EsynetMessEvent( m_current_time, ET_ASSIGN_END,
			m_router_id, a, b, m_router_id, output.first, output.second, 
			EsynetFlit() ) );
	}
}

void EsynetRouter::bypassReceiveCredit( long phy, long vc, long credit )
{
	long inphy = bypassOutput2Input( VCType( phy, vc ) ).first;
	if ( inphy >= 0 )
	{
		long credit_id = m_input_port[ inphy ].inputNeighborAddrId();
		long credit_port = m_input_port[ inphy ].inputNeighborAddrPort();
		if ( credit_id >= 0 && credit_port >= 0 )
		{
			addEvent( EsynetMessEvent::generateCreditMessage(
				m_current_time + CREDIT_DELAY_,
				m_router_id, inphy, vc, credit_id, credit_port, vc,
				credit - 1 ) );
		}
	}
}

#undef PLOCALPORT
#undef PNORTHPORT
#undef PNORTH1PORT
#undef PSOUTHPORT
#undef PSOUTH1PORT
#undef PWESTPORT
#undef PEASTPORT
#undef PNORTH2PORT
#undef PSOUTH2PORT