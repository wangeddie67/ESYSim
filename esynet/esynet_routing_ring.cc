/*
 * File name : esynet_routing_ring.cc
 * Function : Routing algorithm for ring.
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

#include "esynet_router_unit.h"

#define PORT_LOCAL  0
#define PORT_DOWN   1
#define PORT_UP     2

void EsynetRouter::algorithmSingleRing( long des, long src, long s_ph, long s_vc )
{
    long phy = PORT_LOCAL;
    // if destination == current id, turn to local
    if ( des == m_router_id )
    {
        phy = PORT_LOCAL;
    }
    // otherwise, turn to up.
    else
    {
        phy = PORT_UP;
    }

    // assign to all virtual channl.
    for ( int vc = 0; vc < m_input_port[ phy ].size(); vc ++ )
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( esynet::EsynetVC( phy, vc ) );
    }
}

void EsynetRouter::algorithmDoubleRing( long des, long src, long s_ph, long s_vc )
{
    // routing direction, two direction.
    long offset = des - m_router_id;
    bool direction = ( abs( static_cast<int>( offset ) ) * 2 <= m_network_cfg->routerCount() ) ? true: false;

    long phy = PORT_LOCAL;
    // if destination == current id, turn to local
    if ( offset == 0 )
    {
        phy = PORT_LOCAL;
    }
    // if destination > current id, turn to up.
    else if ( offset > 0 )
    {
        // if distance between destination and current id is longer than 1/2 router number, reversal direction.
        phy = direction ? PORT_UP : PORT_DOWN;
    }
    // if destination < current id, turn to down
    else
    {
        // if distance between destination and current id is longer than 1/2 router number, reversal direction. 
        phy = direction ? PORT_DOWN : PORT_UP;
    }

    // assign to all virtual channel.
    for ( int vc = 0; vc < m_input_port[ phy ].size(); vc ++ )
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( esynet::EsynetVC( phy, vc ) );
    }
}

#undef PORT_LOCAL
#undef PORT_DOWN
#undef PORT_UP
