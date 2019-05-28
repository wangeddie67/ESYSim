/*
 * File name : esynet_routing_table.cc
 * Function : Table based routing algorithm.
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

#define AX_X EsyNetworkCfg::AX_X
#define AX_Y EsyNetworkCfg::AX_Y

bool EsynetRouter::routingTableCheck()
{
    long t_router_count = m_network_cfg->routerCount();

    for ( std::size_t dst = 0; dst < m_routing_table.size(); dst ++ )
    {
        // If there is one destination without item, return false.
        if ( m_routing_table[ dst ].size() == 0 )
        {
            return false;
        }
        long item_count = 0;
        // Check if there is one miss between source and destination. 
        for ( std::size_t i = 0; i < m_routing_table[ dst ].size(); i ++ )
        {
            if ( m_routing_table[ dst ][ i ].first == -1 )
            {
                item_count = t_router_count;
            }
            else
            {
                item_count += 1;
            }
        }
        if ( item_count != t_router_count )
        {
            return false;
        }
    }
    return true;
}

void EsynetRouter::algorithmTable(long des, long src, long s_ph, long s_vc)
{
    // Search table for routing destination.
    esynet::EsynetVC routing_vc = esynet::VC_NULL;
    for ( std::size_t i = 0; i < m_routing_table[ des ].size(); i ++ )
    {
        long src_in_table = m_routing_table[ des ][ i ].first;
        if ( src_in_table == -1 || src_in_table == src )
        {
            routing_vc = m_routing_table[ des ][ i ].second;
        }
    }

    // asser destination
    assert( routing_vc != esynet::VC_NULL );

    // If not specified vc, assign to all vc.
    if ( routing_vc.second == -1 )
    {
        for ( int vc = 0; vc < m_input_port[ routing_vc.first ].size(); vc ++ )
        {
            esynet::EsynetVC routing_vc_t = routing_vc;
            routing_vc_t.second = vc;
            m_input_port[ s_ph ][ s_vc ].addRouting( routing_vc_t );
        }
    }
    // Otherwise, assign to specified vc.
    else
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( routing_vc );
    }
}
