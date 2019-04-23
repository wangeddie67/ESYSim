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

/* position <= {NE,NW,SE,SW,E,W,N,or S} according to the source and destination positions
 * delta(x) <= Xd-Xc when Xd>Xc else Xc-Xd;   (Current and Dest)
 * delta(y) <= Yd-Yc when Yd>Yc else Yc-Yd;	 (Current and Dest)
 * General rule:
 * vc <= vc1 when position={E,NE,SE} else vc2 when position={W,N,S,NW,SW};
 */
// version 3.0 

#define AX_X EsyNetworkCfg::AX_X
#define AX_Y EsyNetworkCfg::AX_Y

#define PLOCAL		VCType(0,0)
#define PNORTH1		VCType(1,0)
#define PSOUTH1		VCType(2,0)
#define PWEST		VCType(3,0)
#define PEAST		VCType(4,0)
#define PNORTH2		VCType(1,1)
#define PSOUTH2		VCType(2,1)
#define DNORTH1		1
#define DSOUTH1		2
#define DWEST		3
#define DEAST		4
#define DNORTH2     1
#define DSOUTH2     2

void EsynetRouter::algorithmHiPFaR(long des, long src, long s_ph, long s_vc)
{
    AddrType des_t = m_network_cfg->seq2Coord( des );
    AddrType sor_t = m_network_cfg->seq2Coord( src );
    // IF (Xc=Xd and Yc=Yd) then Select <= Local;
    if ( ( m_router_addr[AX_X] == des_t[AX_X] ) && 
         ( m_router_addr[AX_Y] == des_t[AX_Y] ) )
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( VCType( PLOCAL ) );
    }
    // ***
    // Elsif (D=East) then
    else if ( ( sor_t[AX_Y] == des_t[AX_Y] ) && ( sor_t[AX_X] < des_t[AX_X] ) )
    {
        // if delta(y)=0 then
        if (des_t[AX_Y] == m_router_addr[AX_Y])
        {
            // if neighbor(E)=faulty then select <= N(vc1) or S(vc1) based on 
            // their availability
            if ( m_neighbor_faulty[DEAST] == true)
            {
                if ( (m_neighbor_faulty[DNORTH1] == false) && 
                     (m_neighbor_faulty[DSOUTH1] == true) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
                }
                else if ( (m_neighbor_faulty[DNORTH1] == true) && 
                          (m_neighbor_faulty[DSOUTH1] == false) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
                }
                else
                {
                    if ( m_router_addr[AX_Y] == 0 )
                    {
                        m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
                    }
                    else if ( m_router_addr[AX_Y] == 
                              m_network_cfg->size( AX_Y ) - 1 )
                    {
                        m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
                    }
                    else
                    {
						if (EsynetSRGenFlatLong(0, 2) == 0)
                        {
                            m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
                        }
                        else
                        {
                            m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
                        }
                    }
                }
            }
            //Else Select<=E;
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
            }
        }
        // Else
        else
        {
            // if (neighbor(N)=destination) then select <= N(vc1);
            if ( (des_t[AX_Y] == (m_router_addr[AX_Y] - 1) ) && 
                 (des_t[AX_X] == m_router_addr[AX_X]) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
            }
            // elsif (neighbor(S)=destination) then select <= S(vc1);
            else if ( (des_t[AX_Y] == (m_router_addr[AX_Y] + 1) )  && 
                      (des_t[AX_X] == m_router_addr[AX_X]) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
            }
            // Else Select<=E;
            else
            {
                if ( (m_neighbor_faulty[DEAST] == true) && 
                    (des_t[AX_Y] < m_router_addr[AX_Y]) && (s_ph != DNORTH1) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
                }
                else if ( (m_neighbor_faulty[DEAST] == true) && 
                    (des_t[AX_Y] > m_router_addr[AX_Y]) && (s_ph != DSOUTH1) )
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
    // ***
    // Elsif (D=West) then 
    else if ( (sor_t[AX_Y] == des_t[AX_Y]) && (sor_t[AX_X] > des_t[AX_X]) )
    {
        // if delta(Y)=0 then
        if (des_t[AX_Y] == m_router_addr[AX_Y])
        {
            // if neighbor(W)=faulty then 
            if (m_neighbor_faulty[DWEST] == true)
            {
                // If Neighbor(N)=available then Select<=N(VC2);
                if ( (m_neighbor_faulty[DNORTH2] == false) && 
                     (m_neighbor_faulty[DSOUTH2] == true) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
                }
                // Else Select<=S(VC2);
                else if ( (m_neighbor_faulty[DNORTH2] == true) && 
                          (m_neighbor_faulty[DSOUTH2] == false) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
                }
                else
                {
                    if ( m_router_addr[AX_Y] == 0 )
                    {
                        m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
                    }
                    else if ( m_router_addr[AX_Y] == 
                        m_network_cfg->size( AX_Y ) - 1 )
                    {
                        m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
                    }
                    else
                    {
						if (EsynetSRGenFlatLong(0, 2) == 0)
                        {
                            m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
                        }
                        else
                        {
                            m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
                        }
                    }
                }
            }
            // Else Select<=W;
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
            }
        }
        // Else
        else
        {
            // if (neighbor(N)=destination) then select <= N(vc2);
            if ( (des_t[AX_Y] == (m_router_addr[AX_Y] - 1) ) && 
                 (des_t[AX_X] == m_router_addr[AX_X]) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
            }
            // elsif (neighbor(S)=destination) then select <= S(vc2);
            else if ( (des_t[AX_Y] == (m_router_addr[AX_Y] + 1) )  && 
                      (des_t[AX_X] == m_router_addr[AX_X]) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
            }
            // Else Select<=W;
            else
            {
                if ( (m_neighbor_faulty[DWEST] == true) && 
                    (des_t[AX_Y] < m_router_addr[AX_Y]) && (s_ph != DNORTH2) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
                }
                else if ( (m_neighbor_faulty[DWEST] == true) && 
                    (des_t[AX_Y] > m_router_addr[AX_Y]) && (s_ph != DSOUTH2) )
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
    // ***
    // Elsif (D=North) then 
    else if ( (sor_t[AX_Y] > des_t[AX_Y]) && (sor_t[AX_X] == des_t[AX_X]) )
    {
        // if delta(X)=0 then
        if (des_t[AX_X] == m_router_addr[AX_X])
        {
            // if neighbor(N)=faulty then
            if (m_neighbor_faulty[DNORTH2] == true)
            {
                // if Xc/=0 then select <= W; 
                if (m_router_addr[AX_X] != 0)
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                // else select <= E;
                else
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
            }
            // else select <= N(vc2);
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
            }
        }
        // Else
        else
        {
            // if (Xd>Xc) then
            if (des_t[AX_X] > m_router_addr[AX_X])
            {
                // If (inPort/={E} and neighbor(E)=non-faulty) then Select<=E;
                if ( (s_ph != DEAST) && (m_neighbor_faulty[DEAST] == false) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
                // else select <= N(vc2);  
                else if ( (des_t[AX_Y] == m_router_addr[AX_Y]) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
                else
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
                }
            }
            // else
            else
            {
                // If (inPort/={W} and neighbor(W)=non-faulty) then Select<=W;
                if ( (s_ph != DWEST) && (m_neighbor_faulty[DWEST] == false) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                else if ( (des_t[AX_Y] == m_router_addr[AX_Y]) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                // else select <= N(vc2);  
                else
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
                }
            }
        }
    }
    // ***
    // Elsif (D=South) then 
    else if ( (sor_t[AX_Y] < des_t[AX_Y]) && (sor_t[AX_X] == des_t[AX_X]) )
    {
        // if delta(X)=0 then
        if (des_t[AX_X] == m_router_addr[AX_X])
        {
            // if neighbor(S)=faulty then
            if (m_neighbor_faulty[DSOUTH2] == true)
            {
                // if Xc/=0 then select <= W; 
                if (m_router_addr[AX_X] != 0)
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                // else select <= E;
                else
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
            }
            // else select <= S(vc2);	 
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
            }
        }
        // Else
        else
        {
            // if (Xd>Xc) then
            if (des_t[AX_X] > m_router_addr[AX_X])
            {
                // If (inPort/={E} and neighbor(E)=non-faulty) then Select<=E;
                if ( (s_ph != DEAST) && (m_neighbor_faulty[DEAST] == false) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
                else if ( (des_t[AX_Y] == m_router_addr[AX_Y]) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
                // else select <= S(vc2);  
                else
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
                }
            }
            // else
            else
            {
                // If (inPort/={W} and neighbor(W)=non-faulty) then Select<=W;
                if ( (s_ph != DWEST) && (m_neighbor_faulty[DWEST] == false) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                else if ( (des_t[AX_Y] == m_router_addr[AX_Y]) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                // else select <= S(vc2);  
                else
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
                }
            }
        }
    }
    // ***
    // Elsif (D=Northeast) then
    else if ( (sor_t[AX_Y] > des_t[AX_Y]) && (sor_t[AX_X]  < des_t[AX_X]) )
    {
        // if (delta(X)>=1 and delta(Y)=0) then select<=E;
        if ( (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) >= 1) 
            && (des_t[AX_Y] == m_router_addr[AX_Y]) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
        }
        // elsif (delta(X)=0 and delta(Y)>=1) then select<=N(vc1);
        else if ( (des_t[AX_X] == m_router_addr[AX_X]) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y] ) ) >= 1) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
        }
        // elsif (delta(X)=1 and delta(Y)>=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) == 1) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y] ) ) >= 1) )
        {
            // if neighbor(N)=faulty then select<=E; 
            if (m_neighbor_faulty[DNORTH1] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
            }
            // else  select<=N(vc1);     
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
            }
        }
        // elsif (delta(X)>1 and delta(Y)=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) > 1) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) == 1) )
        {
            // if neighbor(E)=faulty then select<=N(vc1); 
            if (m_neighbor_faulty[DEAST] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
            }
            // else select<=E;    
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
            }
        }
        // else
        else
        {
            // if neighbor(E)=faulty then select<=N(vc1);
            if ( (m_neighbor_faulty[DEAST] == true) && 
                 (m_neighbor_faulty[DNORTH1] == false) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
            }
            // elsif neighbor(N)=faulty then select<=E;
            else if ( (m_neighbor_faulty[DEAST] == false) && 
                      (m_neighbor_faulty[DNORTH1] == true) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
            }
            // else select<= E or N(vc1); 
            else
            {
                if ( creditCounter(PEAST) > creditCounter(PNORTH1) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
                else if (creditCounter(PEAST) < creditCounter(PNORTH1) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH1 );
                }
                else
                {
					if (EsynetSRGenFlatLong(0, 2) == 0)
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
    }
    // end if;
    // ***
    // Elsif (D=Northwest) then	
    else if ( (sor_t[AX_Y] > des_t[AX_Y]) && (sor_t[AX_X] > des_t[AX_X]) )
    {
        // if (delta(X)>=1 and#include "farthest_neighbor.h" delta(Y)=0) then 
//select<=W;
        if ( (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) >= 1) 
&&               (des_t[AX_Y] == m_router_addr[AX_Y]) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
        }
        // elsif (delta(X)=0 and delta(Y)>=1) then select<=N(vc2);
        else if ( (des_t[AX_X] == m_router_addr[AX_X]) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) >= 1) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
        }
        // elsif (delta(X)=1 and delta(Y)>=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) == 1) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) >= 1) )
        {
            // if neighbor(N)=faulty then select<=W; 
            if (m_neighbor_faulty[DNORTH2] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
            }
            // else 	select<=N(vc2);     
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
            }
        }
        // elsif (delta(X)>1 and delta(Y)=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) > 1) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) == 1) )
        {
            // if neighbor(W)=faulSRGenty then select<=N(vc2); 
            if (m_neighbor_faulty[DWEST] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
            }
            // else select<=W;    
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
            }
        }
        // else
        else
        {
            // if neighbor(W)=faulty then select<=N(vc2);
            if ( (m_neighbor_faulty[DWEST] == true) && 
                 (m_neighbor_faulty[DNORTH2] == false) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
            }
            // elsif neighbor(N)=faulty then select<=W;
            else if ( (m_neighbor_faulty[DWEST] == false) && 
                      (m_neighbor_faulty[DNORTH2] == true) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
            }
            // else select<= W or N(vc2); 
            else
            {
                if ( creditCounter(PWEST) > creditCounter(PNORTH2) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                else if ( creditCounter(PWEST) < creditCounter(PNORTH2) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PNORTH2 );
                }
                else
                {
					if (EsynetSRGenFlatLong(0, 2) == 0)
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
    }
    // end if;
    // ***
    // Elsif (D=Southeast) then	
    else if ( (sor_t[AX_Y] < des_t[AX_Y]) && (sor_t[AX_X] < des_t[AX_X]) )
    {
        // if (delta(X)>=1 and delta(Y)=0) then select<=E;
        if ( (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) >= 1)   
            && (des_t[AX_Y] == m_router_addr[AX_Y]) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
        }
        // elsif (delta(X)=0 and delta(Y)>=1) then select<=S(vc1);
        else if ( (des_t[AX_X] == m_router_addr[AX_X]) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) >= 1) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
        }
        // elsif (delta(X)=1 and delta(Y)>=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) == 1) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) >= 1) )
        {
            // if neighbor(S)=faulty then select<=E; 
            if (m_neighbor_faulty[DSOUTH1] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
            }
            // else 	select<=S(vc1);     
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
            }
        }
        // elsif (delta(X)>1 and delta(Y)=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) > 1) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) == 1) )
        {
            // if neighbor(E)=faulty then select<=S(vc1); 
            if (m_neighbor_faulty[DEAST] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
            }
            // else select<=E;    
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
            }
        }
        // else
        else
        {
            // if neighbor(E)=faulty then select<=S(vc1);
            if ( (m_neighbor_faulty[DEAST] == true) && 
                 (m_neighbor_faulty[DSOUTH1] == false) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
            }
            // elsif neighbor(S)=faulty then select<=E;
            else if ( (m_neighbor_faulty[DEAST] == false) && 
                      (m_neighbor_faulty[DSOUTH1] == true) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
            }
            // else select<= E or S(vc1); 
            else
            {
                if ( creditCounter(PEAST) > creditCounter(PSOUTH1) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PEAST );
                }
                else if ( creditCounter(PEAST) < creditCounter(PSOUTH1) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH1 );
                }
                else
                {
					if (EsynetSRGenFlatLong(0, 2) == 0)
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
    }
    // end if;
    // ***
    // Elsif (D=Southwest) then
    else
    {
        // if (delta(X)>=1 and delta(Y)=0) then select<=W;
        if ( (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) >= 1) 
            && (des_t[AX_Y] == m_router_addr[AX_Y]) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
        }
        // elsif (delta(X)=0 and delta(Y)>=1) then select<=S(vc2);
        else if ( (des_t[AX_X] == m_router_addr[AX_X]) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) >= 1) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
        }
        // elsif (delta(X)=1 and delta(Y)>=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X ]) ) == 1) 
&&             (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) >= 1) 
)
        {
            // if neighbor(S)=faulty then select<=W; 
            if (m_neighbor_faulty[DSOUTH2] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
            }
            // else 	select<=S(vc2);     
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
            }
        }
        // elsif (delta(X)>1 and delta(Y)=1) then
        else if ( 
            (abs(static_cast<int>(des_t[AX_X] - m_router_addr[AX_X]) ) > 1) && 
            (abs(static_cast<int>(des_t[AX_Y] - m_router_addr[AX_Y]) ) == 1) )
        {
            // if neighbor(W)=faulty then select<=S(vc2); 
            if (m_neighbor_faulty[DWEST] == true)
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
            }
            // else select<=W;    
            else
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
            }
        }
        // else
        else
        {
            // if neighbor(W)=faulty then select<=S(vc2);
            if ( (m_neighbor_faulty[DWEST] == true) && 
                 (m_neighbor_faulty[DSOUTH2] == false) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
            }
            // elsif neighbor(S)=faulty then select<=W;
            else if ( (m_neighbor_faulty[DWEST] == false) && 
                      (m_neighbor_faulty[DSOUTH2] == true) )
            {
                m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
            }
            // else select<= W or S(vc2); 
            else
            {
                if ( creditCounter(PWEST) > creditCounter(PSOUTH2) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PWEST );
                }
                else if ( creditCounter(PWEST) < creditCounter(PSOUTH2) )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( PSOUTH2 );
                }
                else
                {
					if (EsynetSRGenFlatLong(0, 2) == 0)
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
        // end if;*/
}

#undef PLOCAL
#undef PNORTH1
#undef PSOUTH1
#undef PWEST
#undef PEAST
#undef PNORTH2
#undef PSOUTH2
#undef DNORTH1
#undef DSOUTH1
#undef DWEST
#undef DEAST
#undef DNORTH2
#undef DSOUTH2

#undef AX_X
#undef AX_Y

