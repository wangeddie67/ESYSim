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

#define PLOCAL	0
#define PNORTH	1
#define PSOUTH	2
#define PWEST	3
#define PEAST	4

#define VCLOCAL		VCType(PLOCAL,0)
#define VCNORTH1	VCType(PNORTH,0)
#define VCNORTH2	VCType(PNORTH,1)
#define VCSOUTH1	VCType(PSOUTH,0)
#define VCSOUTH2	VCType(PSOUTH,1)
#define VCWEST		VCType(PWEST,0)
#define VCEAST		VCType(PEAST,0)

#define VCNORTH(vc)	VCType(PNORTH,vc)
#define VCSOUTH(vc)	VCType(PSOUTH,vc)

#define FDNORTH    1
#define FDSOUTH    2
#define FDWEST     3
#define FDEAST     4

#define FDNORTHWEST 1
#define FDSOUTHEAST 2
#define FDSOUTHWEST 3
#define FDNORTHEAST 4

void EsynetRouter::algorithmNonBlock(
    long des, long src, long s_ph, long s_vc )
{
	AddrType des_t = m_network_cfg->seq2Coord( des );
	AddrType sor_t = m_network_cfg->seq2Coord( src );

	if ( ( s_vc == m_router_cfg->port( s_ph ).inputVcNumber() - 1 ) && 
		m_argu_cfg->bistEnable() && 
		m_argu_cfg->bistTypeCode() == esynet::BIST_NONBLOCK )
	{
		long t_port = s_ph;
		long t_vc = ( des_t[AX_X] > sor_t[AX_X] )?0:1;
		if ( t_port == PNORTH || t_port == PSOUTH )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( VCType( t_port, t_vc ) );
		}
		else if ( t_port == PWEST || t_port == PEAST )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( VCType( t_port, 0 ) );
		}
	}
	/* LIVING */
	else
	{
		VCType x_dir, y_dir;
		long x_dir_port, y_dir_port;
		long delta_x, delta_y;
		long vc;
		bool des_is_east_border;
		bool corner_is_disabled = false ;
		
		delta_x = abs( static_cast<int>( des_t[AX_X] - m_router_addr[AX_X] ) );
		delta_y = abs( static_cast<int>( des_t[AX_Y] - m_router_addr[AX_Y] ) );
		vc = ( des_t[AX_X] > sor_t[AX_X] )?0:1;
		x_dir = ( des_t[AX_X] > m_router_addr[AX_X] )?VCEAST:VCWEST;
		x_dir_port = x_dir.first;
		if ( vc == 0 )
		{
			y_dir = ( des_t[AX_Y] > m_router_addr[AX_Y] )?VCSOUTH1:VCNORTH1;
		}
		else
		{
			y_dir = ( des_t[AX_Y] > m_router_addr[AX_Y] )?VCSOUTH2:VCNORTH2;
		}
		y_dir_port = y_dir.first;
		des_is_east_border = 
			( des_t[ AX_X ] == m_network_cfg->size( AX_X ) - 1 )?true:false;
		if ( x_dir_port == PEAST && y_dir_port == PNORTH )
		{
			corner_is_disabled = m_corner_faulty[ FDNORTHEAST ];
		}
		else if ( x_dir_port == PWEST && y_dir_port == PNORTH )
		{
			corner_is_disabled = m_corner_faulty[ FDNORTHWEST ];
		}
		else if ( x_dir_port == PEAST && y_dir_port == PSOUTH )
		{
			corner_is_disabled = m_corner_faulty[ FDSOUTHEAST ];
		}
		else if ( x_dir_port == PWEST && y_dir_port == PSOUTH )
		{
			corner_is_disabled = m_corner_faulty[ FDSOUTHWEST ];
		}

		if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
			m_router_addr[AX_X] == des_t[AX_X] )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( VCLOCAL );
		}
		else if ( ( m_router_addr[ AX_Y ] == des_t[ AX_Y ] ) &&
			( m_router_addr[ AX_X ] != des_t[ AX_X ] ) )
		{
			if ( !m_neighbor_faulty[ x_dir_port ] )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
			else if ( m_output_port[ PWEST ].outputNeighborAddrId() == des ) 
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( VCWEST );
			}
			else if ( ( m_output_port[ PEAST ].outputNeighborAddrId() == des ) 
				&& des_is_east_border )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( VCEAST );
			}
			else
			{
				if ( ( m_neighbor_faulty[ FDNORTH ] == true &&
					m_neighbor_faulty[ FDSOUTH ] == false ) || 
					( m_router_addr[ AX_Y ] == 0 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH(vc) );
				}
				else if ( ( m_neighbor_faulty[ FDNORTH ] == false &&
					m_neighbor_faulty[ FDSOUTH ] == true ) ||
				( m_router_addr[ AX_Y ] == m_network_cfg->size( AX_Y ) - 1 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH(vc) );
				}
				else
				{
					if ( creditCounter( VCNORTH(vc) ) >=
						creditCounter( VCSOUTH(vc) ) )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH(vc) );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH(vc) );
					}
				}
			}
		}
		else if ( ( m_router_addr[ AX_Y ] != des_t[ AX_Y ] ) &&
			( m_router_addr[ AX_X ] == des_t[ AX_X ] ) )
		{
			if ( m_neighbor_faulty[ y_dir_port ] && 
				m_output_port[ y_dir_port ].outputNeighborAddrId() == des )
			{
				if ( des_is_east_border )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( VCWEST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( VCEAST );
				}
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
			}
		}
		else if ( ( m_router_addr[ AX_Y ] != des_t[ AX_Y ] ) &&
			( m_router_addr[ AX_X ] != des_t[ AX_X ] ) )
		{
			if ( delta_x == 1 && delta_y == 1 && corner_is_disabled )
			{
				if ( m_router_addr[ AX_X ] > des_t[ AX_X ] )
				{
					if ( s_ph == PWEST )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( 
							VCType( y_dir_port, 0 ) );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
					}
				}
				else
				{
					if ( des_is_east_border )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
					}
				}
			}
			else if ( delta_x == 1 && des_is_east_border )
			{
				if ( !m_neighbor_faulty[ y_dir_port ] )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
				}
			}
//			else if ( delta_x == 1 && corner_is_disabled )
//			{
//				m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
//			}
			else if ( delta_y == 1 && corner_is_disabled )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
			else
			{
				if ( m_neighbor_faulty[ x_dir_port ] == true &&
					m_neighbor_faulty[ y_dir_port ] == false )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
				}
				else if ( m_neighbor_faulty[ x_dir_port ] == false &&
					m_neighbor_faulty[ y_dir_port ] == true )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
				}
				else
				{
					if ( creditCounter( x_dir ) >
						creditCounter( y_dir ) )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
					}
				}
			}
		}
	}
}

#undef PLOCAL
#undef PNORTH
#undef PSOUTH
#undef PWEST
#undef PEAST

#undef VCLOCAL
#undef VCNORTH1
#undef VCNORTH2
#undef VCSOUTH1
#undef VCSOUTH2
#undef VCWEST
#undef VCEAST

#undef VCNORTH(vc)
#undef VCSOUTH(vc)

#undef FDNORTH
#undef FDSOUTH
#undef FDWEST
#undef FDEAST

#undef FDNORTHWEST
#undef FDSOUTHEAST
#undef FDSOUTHWEST
#undef FDNORTHEAST

#undef AX_X
#undef AX_Y



