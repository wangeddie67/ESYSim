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
#define PNORTH1PORT    1
#define PSOUTH1PORT    2
#define PWESTPORT      3
#define PEASTPORT      4
#define PNORTH2PORT    5
#define PSOUTH2PORT    6

#define PLOCAL     VCType(0,0)
#define PNORTH1    VCType(1,0)
#define PSOUTH1    VCType(2,0)
#define PWEST      VCType(3,0)
#define PEAST      VCType(4,0)
#define PNORTH2    VCType(5,0)
#define PSOUTH2    VCType(6,0)

#define FDNORTH    1
#define FDSOUTH    2
#define FDWEST     3
#define FDEAST     4

#define FDNORTHWEST 1
#define FDSOUTHEAST 2
#define FDSOUTHWEST 3
#define FDNORTHEAST 4

void EsynetRouter::algorithmRescuer(
	long des, long src, long s_ph, long s_vc )
{
	AddrType des_t = m_network_cfg->seq2Coord( des );
	AddrType sor_t = m_network_cfg->seq2Coord( src );

	/* IF D=Local then Select <= Local; */
	if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
			m_router_addr[AX_X] == des_t[AX_X] )
	{
		m_input_port[ s_ph ][ s_vc ].addRouting( PLOCAL );
	}
	/* Elsif D=East then */
	else if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X] )
	{
		/* If Neighbor(E)=Healthy then Select<=E; */
		if ( m_neighbor_faulty[ FDEAST ] == false )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
		}
		/* Else */
		else
		{
			/* If Neighbor(E)=D then */
			if ( m_router_addr[AX_X] + 1 == des_t[AX_X] )
			{
				/* If Neighbor(N)=avaiable then Select<=N(VC1); */
				if ( m_neighbor_faulty[ FDNORTH ] == false )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
				}
				/* Else Select<=S(VC1); */
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
				}
			}
			/* Else Select<=E; */
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
		}
	}
	/* Elsif D=West then */
	else if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X] )
	{
		/* If Neighbor(W)=Healthy then Select<=W; */
		if ( m_neighbor_faulty[ FDWEST ] == false )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
		}
		/* Else */
		else
		{
			/* If Neighbor(W)=D then */
			if ( m_router_addr[AX_X] - 1 == des_t[AX_X] )
			{
				/* If Neighbor(N)=available then Select<=N(VC2); */
				if ( m_neighbor_faulty[ FDNORTH ] == false )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
				}
				/* Else Select<=S(VC2); */
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
				}
			}
			/* Else Select<=W */
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
		}
	}
	/* Elsif D=North then */
	else if ( m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] == des_t[AX_X] )
	{
		/* If Neighbor(N)=Faulty then Select<=N(VC2) */
		if ( m_neighbor_faulty[ FDNORTH ] == true )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
		}
		/* Else */
		else
		{
			/* If Packet=eastward then Select<=N(VC1); */
			if ( des_t[AX_X] > sor_t[AX_X] )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
			}
			/* Else Select<=N(VC2) */
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
			}
		}
	}
	/* Elsif D=South then */
	else if ( m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] == des_t[AX_X] )
	{
		/* If (Neighbor(S)=Faulty and Neighbor(S) = D) 
			* then Select<=S(VC1); */
		if ( m_neighbor_faulty[ FDSOUTH ] == true )
		{
			if ( des_t[AX_Y] == m_router_addr[AX_Y] + 1 )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
			}
		}
		/* Else */
		else
		{
			/* If Packet=westward then Select<=S(VC2) */
			if ( des_t[AX_X] < sor_t[AX_X] )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
			}
			/* Else Select<=S(VC1); */
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
			}
		}
	}
	/* Elsif D=NorthEast then */
	else if ( m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X] )
	{
		/* If (Delta(X)=1 and Delta(Y)>1) then */
		if ( ( m_router_addr[AX_Y] - des_t[AX_Y] > 1 ) && 
				( des_t[AX_X] - m_router_addr[AX_X] == 1 ) )
		{
			/* If Neighbor(N)=available then Select<=N(VC1); 
				* else Select<=E; */
			if ( m_neighbor_faulty[ FDNORTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
		}
		/* Elsif (Delta(X)>=1 and Delta(Y)=1) then */
		else if ( ( m_router_addr[AX_Y] - des_t[AX_Y] == 1 ) && 
					( des_t[AX_X] - m_router_addr[AX_X] >= 1 ) )
		{
			/* If Neighbor(E)=available then Select<=E; 
				* else Select<=N(VC1); */
			if ( m_neighbor_faulty[ FDEAST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
			}
		}
		/* Else	-- either E or N can be selected, maybe based on some
			* congestion value but we can code like
			* If Neighbor(N) = available then Select<=N(VC1); else Select<=E 
			*/
		else
		{
			if ( m_neighbor_faulty[ FDNORTH ] == false && 
					m_neighbor_faulty[ FDEAST ] == true )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
			}
			else if ( m_neighbor_faulty[ FDNORTH ] == true && 
						m_neighbor_faulty[ FDEAST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
			else 
			{
				if ( creditCounter( PEAST ) >= creditCounter( PNORTH1 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
				}
			}
		}
	}
	/* Elsif D=NorthWest then */
	else if (m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X] )
	{
		/* If (Delta(X)=1 and Delta(Y)>1) then */
		if ( ( m_router_addr[AX_Y] - des_t[AX_Y] > 1 ) && 
				( m_router_addr[AX_X] - des_t[AX_X] == 1 ) )
		{
			/* If Neighbor(N)=available then Select<=N(VC2); 
				* else Select<=W; */
			if ( m_neighbor_faulty[ FDNORTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
		}
		/* Elsif (Delta(X)>=1 and Delta(Y)=1) then */
		else if ( ( m_router_addr[AX_Y] - des_t[AX_Y] == 1 ) && 
					( m_router_addr[AX_X] - des_t[AX_X] >= 1 ) )
		{
			/* If Neighbor(W)=available then Select<=W; 
				* else Select<=N(VC2); */
			if ( m_neighbor_faulty[ FDWEST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
			}
		}
		/* Else	-- either W or N can be selected, maybe based on some 
			* congestion value but we can code like
			* If Neighbor(N)=aviable then Select<=N(VC2); else Select<=W; */
		else
		{
			if ( m_neighbor_faulty[ FDNORTH ] == false &&  
					m_neighbor_faulty[ FDWEST ] == true )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
			}
			else if ( m_neighbor_faulty[ FDNORTH ] == true &&  
						m_neighbor_faulty[ FDWEST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
			else 
			{
				if ( creditCounter( PWEST ) >= creditCounter( PNORTH2 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
				}
			}
		}
	}
	/* Elsif D=SouthEast then */
	else if ( m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X] )
	{
		/* If (Delta(X)=1 and Delta(Y)>1) then */
		if ( ( des_t[AX_Y] - m_router_addr[AX_Y] > 1 ) && 
				( des_t[AX_X] - m_router_addr[AX_X] == 1 ) )
		{
			/* If Neighbor(S)=available then Select<=S(VC1); 
				* else Select<=E; */
			if ( m_neighbor_faulty[ FDSOUTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
		}
		/* Elsif (Delta(X)>=1 and Delta(Y)=1) then */
		else if ( ( des_t[AX_Y] - m_router_addr[AX_Y] == 1 ) && 
					( des_t[AX_X] - m_router_addr[AX_X] >= 1 ) )
		{
			/* If Neighbor(E)=available then Select<=E; 
				* else Select<=S(VC1); */
			if ( m_neighbor_faulty[ FDEAST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
			}
		}
		/* Else	-- either E or S can be selected, maybe based on some 
			* congestion value but we can code like
			* If Neighbor(S) = available then Select<=S(VC1); else Select<=E 
			*/
		else
		{
			if ( m_neighbor_faulty[ FDSOUTH ] == false && 
					m_neighbor_faulty[ FDEAST ] == true )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
			}
			else if ( m_neighbor_faulty[ FDSOUTH ] == true &&  
						m_neighbor_faulty[ FDEAST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
			else 
			{
				if (creditCounter( PSOUTH1 ) >= creditCounter( PEAST ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
				}
			}
		}
	}
	/* Elsif D=SouthWest then */
	else if ( m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X] )
	{
		/* If (Delta(X)=1 and Delta(Y)>1) then */
		if ( ( des_t[AX_Y] - m_router_addr[AX_Y] > 1 ) &&
				( m_router_addr[AX_X] - des_t[AX_X] == 1 ) )
		{
			/* If Neighbor(S)=available then Select<=S(VC1); 
				* else Select<=E; */
			if ( m_neighbor_faulty[ FDSOUTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
		}
		/* Elsif (Delta(X)>=1 and Delta(Y)=1) then */
		else if ( ( des_t[AX_Y] - m_router_addr[AX_Y] == 1 ) && 
					( m_router_addr[AX_X] - des_t[AX_X] >= 1 ) )
		{
			/* If Neighbor(E)=available then Select<=E; 
				* else Select<=S(VC1); */
			if ( m_neighbor_faulty[ FDWEST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
			}
		}
		/* Else */
		else
		{
			/* If Neighbor(W)=available then Select<=W; 
				* else Select<=S(VC2); */
			if ( m_neighbor_faulty[ FDWEST ] == false && 
					m_neighbor_faulty[ FDSOUTH ] == true )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
			else if ( m_neighbor_faulty[ FDWEST ] == true && 
						m_neighbor_faulty[ FDSOUTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
			}
			else
			{
				if ( creditCounter( PSOUTH2 ) >= creditCounter( PWEST ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
				}
			}
		}
	}
}

void EsynetRouter::algorithmERescuer(
	long des, long src, long s_ph, long s_vc )
{
	AddrType des_t = m_network_cfg->seq2Coord( des );
	AddrType sor_t = m_network_cfg->seq2Coord( src );

	/* IF D=Local then Select <= Local; */
	if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
			m_router_addr[AX_X] == des_t[AX_X] )
	{
		m_input_port[ s_ph ][ s_vc ].addRouting( PLOCAL );
	}
	/* Elsif D=East then */
	else if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X] )
	{
		/* If Neighbor(E)=Healthy then Select<=E; */
		if ( m_neighbor_faulty[ FDEAST ] == false )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
		}
		/* Else */
		else
		{
			/* If Neighbor(E)=D then */
			if ( m_router_addr[AX_X] + 1 == des_t[AX_X] )
			{
				/* If Neighbor(N)=avaiable then Select<=N(VC1); */
				if ( m_neighbor_faulty[ FDNORTH ] == false )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
				}
				/* Else Select<=S(VC1); */
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
				}
			}
			/* Else Select<=E; */
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
		}
	}
	/* Elsif D=West then */
	else if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X] )
	{
		/* If Neighbor(W)=Healthy then Select<=W; */
		if ( m_neighbor_faulty[ FDWEST ] == false )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
		}
		/* Else */
		else
		{
			/* If Neighbor(W)=D then */
			if ( m_router_addr[AX_X] - 1 == des_t[AX_X] )
			{
				/* If Neighbor(N)=available then Select<=N(VC2); */
				if ( m_neighbor_faulty[ FDNORTH ] == false )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
				}
				/* Else Select<=S(VC2); */
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
				}
			}
			/* Else Select<=W */
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
		}
	}
	/* Elsif D=North then */
	else if ( m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] == des_t[AX_X] )
	{
		if ( m_neighbor_faulty[ FDNORTH ] == true )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
		}
		else if ( s_ph == 2 )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
		}
		else if ( des_t[ AX_X ] > sor_t[ AX_X ] )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
		}  
		else
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
		}
	}
	/* Elsif D=South then */
	else if ( m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] == des_t[AX_X] )
	{
		/* If (Neighbor(S)=Faulty and Neighbor(S) = D) 
			* then Select<=S(VC1); */
		if ( m_neighbor_faulty[ FDSOUTH ] == true )
		{
			if ( des_t[AX_Y] == (m_router_addr[AX_Y] + 1) )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
			}
		}
		else if ( s_ph == 1 )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
		}
		else if ( sor_t[ AX_X ] > des_t[ AX_X ] )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
		}  
		else
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
		}  
	}
	/* Elsif D=NorthEast then */
	else if ( m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X] )
	{
		if ( ( des == m_router_id - m_network_cfg->size( AX_X ) + 1 ) && 
				( m_corner_faulty[ FDNORTHEAST ] == true ) )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
		}
		else
		{
			if ( m_neighbor_faulty[ FDNORTH ] == true && 
						m_neighbor_faulty[ FDEAST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
			else if ( m_neighbor_faulty[ FDEAST ] == true && 
						m_neighbor_faulty[ FDNORTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1);
			}
			else
			{
				if ( creditCounter( PEAST ) >= creditCounter( PNORTH1 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
				}
			}
		}
	}
	/* Elsif D=NorthWest then */
	else if ( m_router_addr[AX_Y] > des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X] )
	{
		if ( ( des == m_router_id - m_network_cfg->size( AX_X ) - 1 ) && 
				( m_corner_faulty[ FDNORTHWEST ] == true ) )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
		}
		else
		{
			if ( m_neighbor_faulty[ FDNORTH ] == true  &&  
						m_neighbor_faulty[ FDWEST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
			else if ( m_neighbor_faulty[ FDWEST ] == true  &&  
						m_neighbor_faulty[ FDNORTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
			}
			else
			{
				if ( creditCounter( PWEST ) >= creditCounter( PNORTH2 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
				}
			}
		}
	}
	/* Elsif D=SouthEast then */
	else if ( m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] < des_t[AX_X] )
	{
		if ( ( des == m_router_id + m_network_cfg->size( AX_X ) + 1 ) && 
				( m_corner_faulty[ FDSOUTHEAST ] == true ) )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
		}
		else
		{
			if ( m_neighbor_faulty[ FDSOUTH ] == true &&  
				m_neighbor_faulty[ FDEAST ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
			}
			else if ( m_neighbor_faulty[ FDEAST ] == true &&  
				m_neighbor_faulty[ FDSOUTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
			}
			else
			{
				if ( creditCounter( PEAST ) >= creditCounter( PSOUTH1 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
				}
			}
		}
	}
	/* Elsif D=SouthWest then */
	else if ( m_router_addr[AX_Y] < des_t[AX_Y] && 
				m_router_addr[AX_X] > des_t[AX_X] )
	{
		if ( ( des == m_router_id + m_network_cfg->size( AX_X ) - 1 ) && 
				( m_corner_faulty[ FDSOUTHWEST ] == true ) )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
		}
		else
		{
			if ( m_neighbor_faulty[ FDWEST ] == false && 
					m_neighbor_faulty[ FDSOUTH ] == true )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
			}
			else if ( m_neighbor_faulty[ FDWEST ] == true && 
				m_neighbor_faulty[ FDSOUTH ] == false )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
			}
			else
			{
				if ( creditCounter( PWEST ) >= creditCounter( PSOUTH2 ) )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
				}
			}
		}
	}
}

void EsynetRouter::algorithmAlterTest(
    long des, long src, long s_ph, long s_vc )
{
    AddrType des_t = m_network_cfg->seq2Coord( des );
    AddrType sor_t = m_network_cfg->seq2Coord( src );
    
	VCType x_dir, y_dir;
	long x_dir_port, y_dir_port;
	long delta_x, delta_y;
	long vc;
	
	delta_x = abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) );
	delta_y = abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) );
	vc = ( des_t[AX_X] > sor_t[AX_X] )?0:1;
	x_dir = ( des_t[AX_X] > m_router_addr[AX_X] )?PEAST:PWEST;
	x_dir_port = x_dir.first;
	
	if ( vc == 0 )
	{
		y_dir = ( des_t[AX_Y] > m_router_addr[AX_Y] )?PSOUTH1:PNORTH1;
	}
	else
	{
		y_dir = ( des_t[AX_Y] > m_router_addr[AX_Y] )?PSOUTH2:PNORTH2;
	}
	y_dir_port = y_dir.first;

	bool near_faulty_dst = false;
	if ( delta_x <= 1 && delta_y <= 1 && !(delta_x == 0 && delta_y == 0) )
	{
		if ( des_t[AX_Y] == m_router_addr[AX_Y] )
		{
			near_faulty_dst = m_neighbor_faulty[ x_dir_port ];
		}
		else if ( des_t[AX_X] == m_router_addr[AX_X] )
		{
			near_faulty_dst = m_neighbor_faulty[ y_dir_port ];
		}
		else
		{
			if ( des_t[AX_X] > m_router_addr[AX_X] && 
				des_t[AX_Y] > m_router_addr[AX_Y] )
			{
				near_faulty_dst = m_corner_faulty[ y_dir_port ];
			}
			else if ( des_t[AX_X] > m_router_addr[AX_X] && 
				des_t[AX_Y] < m_router_addr[AX_Y] )
			{
				near_faulty_dst = m_corner_faulty[ x_dir_port ];
			}
			else if ( des_t[AX_X] < m_router_addr[AX_X] && 
				des_t[AX_Y] > m_router_addr[AX_Y] )
			{
				near_faulty_dst = m_corner_faulty[ x_dir_port ];
			}
			else if ( des_t[AX_X] < m_router_addr[AX_X] && 
				des_t[AX_Y] < m_router_addr[AX_Y] )
			{
				near_faulty_dst = m_corner_faulty[ y_dir_port ];
			}
		}
	}
			
	if ( m_router_addr[AX_Y] == des_t[AX_Y] && 
		m_router_addr[AX_X] == des_t[AX_X] )
	{
		m_input_port[ s_ph ][ s_vc ].addRouting( PLOCAL );
	}
	else if ( near_faulty_dst )
	{
		if ( des_t[AX_X] != m_router_addr[AX_X] )
		{
			if ( des_t[AX_Y] == m_router_addr[AX_Y])
			{
				if ( des_t[AX_Y] == 0 )
				{
					if ( vc == 0 )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
					}
				}
				else
				{
					if ( vc == 0 )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
					}
				}
			}
			else if ( des_t[AX_Y] < m_router_addr[AX_Y])
			{
				if ( des_t[AX_Y] == 0 )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
				}
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
		}
		else if ( des_t[AX_Y] < m_router_addr[AX_Y] )
		{
			if ( des_t[AX_Y] == 0 )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
			}
			else
			{
				if ( m_router_addr[ AX_X ] != 0 )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
				}                    
			}
		}
		else
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
		}
	}
	else if ( (des_t[AX_X] != sor_t[AX_X]) && (des_t[AX_Y] != sor_t[AX_Y]) )
	{
		if ( delta_x >= 1 && delta_y == 0 )
		{
			if ( m_neighbor_faulty[ x_dir_port ] )
			{
				if ( vc == 0 )
				{
					if ( des_t[AX_Y] < sor_t[AX_Y] )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
					}
				}
				else
				{
					if ( des_t[AX_Y] < sor_t[AX_Y] )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
					}
					else
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
					}
				}
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
		}
		else if ( delta_x == 0 && delta_y >= 1 )
		{
			m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
		}
		else if ( delta_x == 1 && delta_y >= 1 )
		{                
			if ( m_neighbor_faulty[ y_dir_port ] )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
			}
		}
		else if ( delta_x >= 1 && delta_y == 1 )
		{                
			if ( m_neighbor_faulty[ x_dir_port ] )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
		}
		else
		{
			if ( m_neighbor_faulty[ x_dir_port ] )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
			}
			else if ( m_neighbor_faulty[ y_dir_port ] )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
			else
			{
				if ( creditCounter( x_dir ) >= creditCounter( y_dir ) )
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
	else if ( des_t[AX_Y] == sor_t[AX_Y] )
	{
		if ( delta_y == 0 )
		{
			if ( m_neighbor_faulty[ x_dir_port ] )
			{
				if ( vc == 0 )
				{
					if ( m_neighbor_faulty[ PNORTH1PORT ] )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
					}
					else if ( m_neighbor_faulty[ PSOUTH1PORT ] )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
					}
					else
					{
						if ( creditCounter( PNORTH1 ) >= 
							creditCounter( PSOUTH1 ) )
						{
							m_input_port[s_ph][s_vc].addRouting( PNORTH1 );
						}
						else
						{
							m_input_port[s_ph][s_vc].addRouting( PSOUTH1 );
						}
					}
				}
				else
				{
					if ( m_neighbor_faulty[ PNORTH2PORT ] )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
					}
					else if ( m_neighbor_faulty[ PSOUTH2PORT ] )
					{
						m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
					}
					else
					{
						if ( creditCounter( PNORTH2 ) >= 
							creditCounter( PSOUTH2 ) )
						{
							m_input_port[s_ph][s_vc].addRouting( PNORTH2 );
						}
						else
						{
							m_input_port[s_ph][s_vc].addRouting( PSOUTH2 );
						}
					}
				}
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
		}
		else
		{
			if ( s_ph != PNORTH1PORT && s_ph != PNORTH2PORT &&
				s_ph != PSOUTH1PORT && s_ph != PSOUTH2PORT &&
				(!m_neighbor_faulty[ y_dir_port ]) )
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( x_dir );
			}
		}
	}
	else if ( des_t[AX_X] == sor_t[AX_X] )
	{
		if ( delta_x == 0 )
		{
			if ( m_neighbor_faulty[ y_dir_port ] )
			{
				if ( m_router_addr[ AX_X ] != 0 )
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
				}
				else
				{
					m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
				}
			}
			else
			{
				m_input_port[ s_ph ][ s_vc ].addRouting( y_dir );
			}
		}
		else
		{
			if ( m_output_port[ x_dir_port ].outputNeighborAddrId() == 
				des || m_neighbor_faulty[ y_dir_port ] )
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

#undef PLOCALPORT
#undef PNORTH1PORT
#undef PSOUTH1PORT
#undef PWESTPORT
#undef PEASTPORT
#undef PNORTH2PORT
#undef PSOUTH2PORT

#undef PLOCAL
#undef PNORTH1
#undef PSOUTH1
#undef PWEST
#undef PEAST
#undef PNORTH2
#undef PSOUTH2

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



