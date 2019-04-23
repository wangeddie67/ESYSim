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

#define PLOCAL		VCType(0,0)
#define PNORTH1		VCType(1,0)
#define PSOUTH1		VCType(2,0)
#define PWEST		VCType(3,0)
#define PEAST		VCType(4,0)
#define PNORTH2		VCType(1,1)
#define PSOUTH2		VCType(2,1)
#define FDROUTER	0
#define FDNORTH		1
#define FDSOUTH		2
#define FDWEST		3
#define FDEAST		4
#define AX_X	0
#define AX_Y	1
#define FAULTY	1
#define LIVING	0
void EsynetRouter::algorithmFTLR(long des, long src, long s_ph, long s_vc)
{
	AddrType des_t = m_network_cfg->seq2Coord( des );
	AddrType sor_t = m_network_cfg->seq2Coord( src );
	// LIVING
	if ( routerFault() == false )
	{
		if ( m_input_port[s_ph][s_vc].getFlit().bypass() )
		{
			long maxindex = m_input_port[s_ph][s_vc].getFlit().bypassId()[0];
			if ( maxindex == 1 && m_router_addr[AX_Y] >= 1 )
				if ( m_neighbor_faulty[FDNORTH] )
					m_input_port[s_ph][s_vc].getFlit().clearBypass();
			if ( maxindex == 2 && m_router_addr[AX_Y] < 
				m_network_cfg->size(AX_Y) - 1 )
				if ( m_neighbor_faulty[FDSOUTH] )
					m_input_port[s_ph][s_vc].getFlit().clearBypass();
		}

		int pri[5];
		if ( !m_input_port[s_ph][s_vc].getFlit().bypass() )
		{
			// IF D=Local then Select <= Local;
			if (m_router_addr[AX_Y] == des_t[AX_Y] && 
				m_router_addr[AX_X] == des_t[AX_X])
			{
				pri[0] = 4; pri[1] = 0; pri[2] = 0; pri[3] = 0; pri[4] = 0;
			}
			// Elsif D=East then
			else if (m_router_addr[AX_Y] == des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
			}
			// Elsif D=West then
			else if (m_router_addr[AX_Y] == des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
			}
			// Elsif D=North then
			else if (m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] == des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 3; pri[4] = 2;
			}
			// Elsif D=South then
			else if (m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] == des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 3; pri[4] = 2;
			}
			// Elsif D=NorthEast then
			else if (m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4;
			}
			// Elsif D=NorthWest then
			else if (m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1;
			}
			// Elsif D=SouthEast then
			else if (m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 1; pri[4] = 4;
			}
			// Elsif D=SouthWest then
			else //if (address_[AX_Y] < des_t[AX_Y] && address_[AX_X] > 
	//des_t[AX_X])
			{
				pri[0] = 0; pri[1] = 2; pri[2] = 3; pri[3] = 4; pri[4] = 1;
			}
		}
		else
		{
			switch( m_input_port[s_ph][s_vc].getFlit().bypassId()[0] )
			{
				case 1:  
					pri[0] = 0; pri[1] = 4; pri[2] = 1; pri[3] = 3; pri[4] = 2; 
					break;
				case 2:  
					pri[0] = 0; pri[1] = 1; pri[2] = 4; pri[3] = 2; pri[4] = 3; 
					break;
				case 3:  
					pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 4; pri[4] = 1; 
					break;
				case 4:  
					pri[0] = 0; pri[1] = 3; pri[2] = 2; pri[3] = 1; pri[4] = 4; 
					break;
				default: 
					pri[0] = 4; pri[1] = 0; pri[2] = 0; pri[3] = 0; pri[4] = 0; 
					break;
			}

			m_input_port[s_ph][s_vc].getFlit().eraseBypassId();
		}

		int maxpri1 = pri[1];
		int maxindex1 = 1;
		for (int i = 0; i < 5; i ++)
		{
			if (pri[i] > maxpri1)
			{
				maxpri1 = pri[i];
				maxindex1 = i;
			}
		}

		// check port
		for (int i = 1; i < 5; i ++)
		{
			if ( i == 1 && m_router_addr[AX_Y] == 1 )
			{
				if ( m_neighbor_faulty[ FDNORTH ] && 
					m_output_port[ FDNORTH ].outputNeighborAddrId() == des )
					pri[ i ] = pri[ i ];
				else
					if ( m_neighbor_faulty[ i ] )
						pri[ i ] = 0;
			}
			else if ( i == 2 && 
				m_router_addr[AX_Y] < m_network_cfg->size( AX_Y ) - 1 )
			{
				if ( m_neighbor_faulty[ FDSOUTH ] && 
					m_output_port[ FDSOUTH ].outputNeighborAddrId() == des )
					pri[ i ] = pri[ i ];
				else
					if ( m_neighbor_faulty[ i ] )
						pri[ i ] = 0;
			}
			else
			{
				if ( m_neighbor_faulty[ i ] )
					pri[i] = 0;
			}
		}
		if ( m_input_port[s_ph][s_vc].getFlit().bypass() )
		{
			if (s_ph != 0)
				if ( s_ph == 5 || s_ph == 6 )
					pri[s_ph - 4] = 0;
				else
					pri[s_ph] = 0;
		}

		int maxpri = pri[1];
		int maxindex = 1;
		for (int i = 0; i < 5; i ++)
		{
			if (pri[i] > maxpri)
			{
				maxpri = pri[i];
				maxindex = i;
			}
		}
		if (maxpri <= 0)
			maxindex = s_ph;

		if ( maxindex != maxindex1 )
		{
			if ( src == 1 && des == 2 )
				cout << maxindex << " " << maxindex1 << endl;

			vector<long> step;
			if (maxindex1 == 1)
				if (maxindex == 2)
				{
					if (m_router_addr[AX_X] > des_t[AX_X])
					{
						step.push_back( 3 ); step.push_back( 1 );
					}
					else
					{
						step.push_back( 4 ); step.push_back( 1 );
					}
				}
				else if (maxindex == 3)
				{
					step.push_back( 1 ); step.push_back( 4 );
				}
				else // (maxindex == 4)
				{
					step.push_back( 1 ); step.push_back( 3 );
				}
			else if (maxindex1 == 2)
				if (maxindex == 1)
				{
					if (m_router_addr[AX_X] > des_t[AX_X])
					{
						step.push_back( 3 ); step.push_back( 2 );
					}
					else
					{
						step.push_back( 4 ); step.push_back( 2 );
					}
				}
				else if (maxindex == 3)
				{
					step.push_back( 2 ); step.push_back( 4 );
				}
				else // (maxindex == 4)
				{
					step.push_back( 2 ); step.push_back( 3 );
				}
			else if (maxindex1 == 3)
				if (maxindex == 1)
				{
					step.push_back( 3 ); step.push_back( 2 );
				}
				else if (maxindex == 2)
				{
					step.push_back( 3 ); step.push_back( 1 );
				}
				else // (maxindex == 4)
				{
					if (m_router_addr[AX_Y] > des_t[AX_Y])
					{
						step.push_back( 1 ); step.push_back( 3 );
					}
					else
					{
						step.push_back( 2 ); step.push_back( 3 );
					}
				}
			else //if (maxindex1 == 4)
				if (maxindex == 1)
				{
					step.push_back( 4 ); step.push_back( 2 );
				}
				else if (maxindex == 2)
				{
					step.push_back( 4 ); step.push_back( 1 );
				}
				else // (maxindex == 3)
				{
					if (m_router_addr[AX_Y] > des_t[AX_Y])
					{
						step.push_back( 1 ); step.push_back( 4 );
					}
					else
					{
						step.push_back( 2 ); step.push_back( 4 );
					}
				}
			m_input_port[s_ph][s_vc].getFlit().setBypass();
			m_input_port[s_ph][s_vc].getFlit().setBypassId(step[0], step[1]);
		}

		long vc_t = 0;
		if ( m_input_port[s_ph][s_vc].getFlit().bypass() )
			vc_t = 1;

		m_input_port[s_ph][s_vc].addRouting( VCType(maxindex,vc_t));

		if ( m_input_port[s_ph][s_vc].getFlit().bypass() )
		{
			if ( m_input_port[s_ph][s_vc].getFlit().bypassId().size() == 0 )
				m_input_port[s_ph][s_vc].getFlit().clearBypass();
		}
	}
	// FAULT
	else
	{
		if (s_ph == 0)		// From Local
			if (m_router_addr[AX_Y] == 0)		// North top line
				m_input_port[s_ph][s_vc].addRouting( PSOUTH1 );
			else
				m_input_port[s_ph][s_vc].addRouting( PNORTH1 );
		else if (s_ph == 1)		// From North VC1
			m_input_port[s_ph][s_vc].addRouting( PLOCAL );
		else if (s_ph == 5)		// From North VC2
			m_input_port[s_ph][s_vc].addRouting( PLOCAL );
		else if (s_ph == 2)		// From South VC1
			m_input_port[s_ph][s_vc].addRouting( PLOCAL );
		else if (s_ph == 6)		// From South VC2
			if (m_router_addr[AX_Y] == 0)	// North top line
				m_input_port[s_ph][s_vc].addRouting( PLOCAL );	
			else
				m_input_port[s_ph][s_vc].addRouting( PNORTH2 );
		else if (s_ph == 3)	// Bypass from East to West
			m_input_port[s_ph][s_vc].addRouting( PEAST );
		else				// Bypass from West to East
			m_input_port[s_ph][s_vc].addRouting( PWEST );
	}
}
#undef PLOCAL
#undef PNORTH1
#undef PSOUTH1
#undef PWEST
#undef PEAST
#undef PNORTH2
#undef PSOUTH2
#undef FDROUTER
#undef FDNORTH
#undef FDSOUTH
#undef FDWEST
#undef FDEAST
#undef AX_X
#undef AX_Y
#undef FAULTY
#undef LIVING



