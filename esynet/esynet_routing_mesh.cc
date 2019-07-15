/*
 * File name : esynet_routing_mesh.cc
 * Function : Routing algorithm XY, TXY, and DyXY.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>
 */

/**
 * @ingroup ESYNET_ROUTER
 * @file esynet_routing_mesh.cc 
 * @brief Routing algorithm for mesh/torus topology.
 */

#include "esynet_router_unit.h"

#define AX_X EsyNetworkCfg::AX_X
#define AX_Y EsyNetworkCfg::AX_Y

#define VCLOCAL esynet::EsynetVC(0,0)
#define VCNORTH esynet::EsynetVC(1,0)
#define VCSOUTH esynet::EsynetVC(2,0)
#define VCWEST  esynet::EsynetVC(3,0)
#define VCEAST  esynet::EsynetVC(4,0)

void EsynetRouter::algorithmTXY(long des, long src, long s_ph, long s_vc)
{
    esynet::EsynetAddr des_t = m_network_cfg->seq2Coord( des );
    esynet::EsynetAddr sor_t = m_network_cfg->seq2Coord( src );
    long xoffset = des_t[ AX_X ] - m_router_addr[ AX_X ];
    long yoffset = des_t[ AX_Y ] - m_router_addr[ AX_Y ];
    bool xdirection = ( abs( static_cast<int>( xoffset ) ) * 2 <= m_network_cfg->size( AX_X ) ) ? true: false;
    bool ydirection = ( abs( static_cast<int>( yoffset ) ) * 2 <= m_network_cfg->size( AX_Y ) ) ? true: false;

    if ( xoffset == 0 && yoffset == 0 )
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( VCLOCAL );
    }
    if ( xdirection )
    {
        if ( xoffset < 0 )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCWEST );
        }
        else if ( xoffset > 0 )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCEAST );
        }
        else
        {
            if ( ydirection )
            {
                if ( yoffset < 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH );
                }
                else if ( yoffset > 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH );
                }
            }
            else
            {
                if ( yoffset < 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH );
                }
                else if ( yoffset > 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH );
                }
            }
        }
    }
    else
    {
        if ( xoffset < 0 )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCEAST );
        }
        else if ( xoffset > 0 )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCWEST );
        }
        else
        {
            if ( ydirection )
            {
                if ( yoffset < 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH );
                }
                else if ( yoffset > 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH );
                }
            }
            else
            {
                if ( yoffset < 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH );
                }
                else if ( yoffset > 0 )
                {
                    m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH );
                }
            }
        }
    }
}
#undef VCLOCAL
#undef VCNORTH
#undef VCSOUTH
#undef VCWEST
#undef VCEAST

#define PLOCAL  0
#define PNORTH  1
#define PSOUTH  2
#define PWEST   3
#define PEAST   4

void EsynetRouter::algorithmXY(long des, long src, long s_ph, long s_vc)
{
    esynet::EsynetAddr des_t = m_network_cfg->seq2Coord( des );
    esynet::EsynetAddr sor_t = m_network_cfg->seq2Coord( src );

    long dir = PLOCAL;

    if(m_router_addr[AX_X] > des_t[AX_X])
    {
        dir = PWEST;
    }
    else if(m_router_addr[AX_X] < des_t[AX_X])
    {
        dir = PEAST;
    }
    else
    {
        if(m_router_addr[AX_Y] > des_t[AX_Y])
        {
            dir = PNORTH;
        }
        else if (m_router_addr[AX_Y] < des_t[AX_Y])
        {
            dir = PSOUTH;
        }
        else
        {
            dir = PLOCAL;
        }
    }

    for (int i = 0; i < (int)m_input_port[ dir ].size(); i ++)
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( esynet::EsynetVC(dir, i) );
    }
}
#undef PLOCAL
#undef PNORTH
#undef PSOUTH
#undef PWEST
#undef PEAST

#define VCLOCAL     esynet::EsynetVC(0,0)
#define VCNORTH1    esynet::EsynetVC(1,0)
#define VCSOUTH1    esynet::EsynetVC(2,0)
#define VCWEST      esynet::EsynetVC(3,0)
#define VCEAST      esynet::EsynetVC(4,0)
#define VCNORTH2    esynet::EsynetVC(1,1)
#define VCSOUTH2    esynet::EsynetVC(2,1)

void EsynetRouter::algorithmDyXY(long des, long src, long s_ph, long s_vc)
{
    esynet::EsynetAddr des_t = m_network_cfg->seq2Coord( des );
    esynet::EsynetAddr sor_t = m_network_cfg->seq2Coord( src );
    // IF D=Local then Select <= Local;
    if ( m_router_addr[ AX_Y ] == des_t[ AX_Y ] && m_router_addr[ AX_X ] == des_t[ AX_X ] )
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( VCLOCAL );
    }
    // ***
    // Elsif D=East then Select <= E;
    else if ( m_router_addr[ AX_Y ] == des_t[ AX_Y ] && m_router_addr[ AX_X ] < des_t[ AX_X ] )
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( VCEAST );
    }
    // Elsif D=West then Select <= W;
    else if ( m_router_addr[ AX_Y ] == des_t[ AX_Y ] && m_router_addr[ AX_X ] > des_t[ AX_X ] )
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( VCWEST );
    }
    // Elsif D=North then Select <= N(VC1);
    else if ( m_router_addr[ AX_Y ] > des_t[ AX_Y ] && m_router_addr[ AX_X ] == des_t[ AX_X ] )
    {
        // If Packet=eastward then Select<=N(VC1);
        if (des_t[ AX_X ] > sor_t[ AX_X ])
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH1 );
        }
        // Else Select<=N(VC2)
        else
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH2 );
        }
    }
    // Elsif D=South then Select <= S(VC2);
    else if ( m_router_addr[ AX_Y ] < des_t[ AX_Y ] && m_router_addr[ AX_X ] == des_t[ AX_X ] )
    {
        // If Packet=westward then Select<=S(VC2)
        if (des_t[ AX_X ] < sor_t[ AX_X ])
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH2 );
        }
        // Else Select<=S(VC1); 
        else
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH1 );
        }
    }
    // Elsif D=NorthEast then
    else if ( m_router_addr[ AX_Y ] > des_t[ AX_Y ] && m_router_addr[ AX_X ] < des_t[ AX_X ] )
    {
        // If E=free then Select <= E;
        // Else Select <= N(VC1);
        if ( creditCounter( VCEAST ) >= creditCounter( VCNORTH1 ) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCEAST );
        }
        else
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH1 );
        }
    }
    // Elsif D=NorthWest then
    else if ( m_router_addr[ AX_Y ] > des_t[ AX_Y ] && m_router_addr[ AX_X ] > des_t[ AX_X ] )
    {
        // If W=free then Select <= W;
        // Else Select <= N(VC2);
        if ( creditCounter( VCWEST ) >= creditCounter( VCNORTH2 ) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCWEST );
        }
        else
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCNORTH2 );
        }
    }
    // Elsif D=SouthEast then
    else if (m_router_addr[ AX_Y ] < des_t[ AX_Y ] && m_router_addr[ AX_X ] < des_t[ AX_X ])
    {
        // If S(VC1) = free then Select <= S(VC1);
        // Else Select <= E;
        if ( creditCounter( VCSOUTH1 ) >= creditCounter( VCEAST ) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH1 );
        }
        else
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCEAST );
        }
    }
    // Elsif D=SouthWest then
    else
    {
        // If S(VC2) = free then Select <= S(VC2);
        // Else Select <= W;
        if ( creditCounter( VCSOUTH2 ) >= creditCounter( VCWEST ) )
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCSOUTH2 );
        }
        else
        {
            m_input_port[ s_ph ][ s_vc ].addRouting( VCWEST );
        }
    }
}
#undef VCLOCAL
#undef VCNORTH1
#undef VCSOUTH1
#undef VCWEST
#undef VCEAST
#undef VCNORTH2
#undef VCSOUTH2

#undef AX_X
#undef AX_Y
