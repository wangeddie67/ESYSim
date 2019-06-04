/*
 * File name : esynet_routing_dia.cc
 * Function : Routing algorithm for diamension mesh or toruns.
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

void EsynetRouter::algorithmDiaTorus(long des, long src, long s_ph, long s_vc)
{
    esynet::EsynetAddr des_t = m_network_cfg->seq2Coord( des );
    esynet::EsynetAddr sor_t = m_network_cfg->seq2Coord( src );

    long phy = 0;
    for ( int dia = 0; dia < m_network_cfg->dim(); dia ++ )
    {
        long offset = des_t[ dia ] - m_router_addr[ dia ];
        bool direction = ( abs( static_cast<int>( offset ) ) * 2 <= m_network_cfg->size( dia ) ) ? true : false;
        // hand to direction with down direction
        if ( offset < 0 )
        {
            phy = direction ? ( 2 * ( m_network_cfg->dim() - dia - 1 ) + 1 ) : ( 2 * ( m_network_cfg->dim() - dia - 1 ) + 2 );
            break;
        }
        // hand to direction with up direction
        else if ( offset > 0 )
        {
            phy = direction ? ( 2 * ( m_network_cfg->dim() - dia - 1 ) + 2 ) : ( 2 * ( m_network_cfg->dim() - dia - 1 ) + 1 );
            break;
        }
        // if one the same position, next diamension.
        else
        {
            continue;
        }
    }

    for (int i = 0; i < (int)m_input_port[ phy ].size(); i ++)
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( esynet::EsynetVC(phy, i) );
    }
}

void EsynetRouter::algorithmDiaMesh(long des, long src, long s_ph, long s_vc)
{
    esynet::EsynetAddr des_t = m_network_cfg->seq2Coord( des );
    esynet::EsynetAddr sor_t = m_network_cfg->seq2Coord( src );

    long phy = 0;
    for ( int dia = 0; dia < m_network_cfg->dim(); dia ++ )
    {
        // hand to port with down direction
        if ( des_t[ dia ] < m_router_addr[ dia ] )
        {
            phy = 2 * ( m_network_cfg->dim() - dia - 1 ) + 1;
            break;
        }
        // hand to port with up direction
        else if ( des_t[ dia ] > m_router_addr[ dia ] )
        {
            phy = 2 * ( m_network_cfg->dim() - dia - 1 ) + 2;
            break;
        }
        // if one the same position, next diamension.
        else
        {
            continue;
        }
    }

    for (int i = 0; i < (int)m_input_port[ phy ].size(); i ++)
    {
        m_input_port[ s_ph ][ s_vc ].addRouting( esynet::EsynetVC(phy, i) );
    }
}
