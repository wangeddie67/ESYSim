/*
 * File name : esy_netcfg.cc
 * Function : Implement network configuration structure.
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

#include <cassert>
#include <iomanip>

#include "esy_netcfg.h"

EsyNetCfg::EsyNetCfg( NoCTopology topology,
               const std::vector< long >& axis_size,
               long node_num,
               long ni_num,
               const std::vector< EsyNetCfgLink >& links )
    : m_topology( topology )
    , m_size( axis_size )
    , m_node_num( node_num )
    , m_ni_num( ni_num )
    , m_links( links )
{}

EsyNetCfg::EsyNetCfg( NoCTopology topology,
               const std::vector< long >& axis_size,
               const std::vector<EsyNetCfgLink>& temp_links )
    : m_topology( topology )
    , m_size( axis_size )
{
    // assert topology and network size
    std::size_t t_dim = m_size.size();
    assert( ( topology != NT_IRREGULAR ) );
    if ( topology == NT_MESH_2D || topology == NT_TORUS_2D )
    {
        assert( t_dim == 2 );
    }
    else if ( topology == NT_MESH_DIA || topology == NT_TORUS_DIA )
    {
        assert( t_dim > 0 );
    }
    else if ( topology == NT_RING )
    {
        assert( t_dim == 1 );
    }

    // the topology is based on ring or bus.
    bool t_ring = ( topology == NT_TORUS_2D )
               || ( topology == NT_TORUS_DIA )
               || ( topology == NT_RING );

    // calcluate node count
    m_node_num = 1;
    for ( std::size_t axis_id = 0; axis_id < t_dim; axis_id ++ )
    {
        m_node_num *= m_size[ axis_id ];
    }

    long port_num = temp_links.size();
    m_ni_num = 0;
    // loop node id to create and connect links
    for ( long node_id = 0; node_id < m_node_num; node_id ++ )
    {
        std::vector< long > t_coord = seq2Coord( node_id );
        for ( long port_id = 0; port_id < port_num; port_id ++ )
        {
            const EsyNetCfgLink& temp_link = temp_links[ port_id ];
            // port connects to NI.
            if ( temp_link.inputPort().isNi() )
            {
                // link from port 0 to NI
                m_links.push_back( EsyNetCfgLink(
                    EsyNetCfgLink::LINKDIR_SW_NE, -1,
                    EsyNetCfgLink::LINKAXISDIR_DOWN,
                    EsyNetCfgPort( node_id, port_id, false,
                        temp_link.outputPort().bufs(),
                        temp_link.outputPort().outPeriod() ),
                    EsyNetCfgPort( m_ni_num, 0, true, 
                        temp_link.inputPort().bufs(),
                        temp_link.inputPort().outPeriod() ),
                    temp_link.niIntDelay() ) );
                // link from NI to port 0
                m_links.push_back( EsyNetCfgLink(
                    EsyNetCfgLink::LINKDIR_NE_SW, -1,
                    EsyNetCfgLink::LINKAXISDIR_UP,
                    EsyNetCfgPort( m_ni_num, 0, true,
                        temp_link.outputPort().bufs(),
                        temp_link.outputPort().outPeriod() ),
                    EsyNetCfgPort( node_id, port_id, false, 
                        temp_link.inputPort().bufs(),
                        temp_link.inputPort().outPeriod() ),
                    temp_link.niIntDelay() ) );
                m_ni_num += 1;
            }
            // port connects to another node
            else
            {
                // axis and axis dir
                long t_axis = temp_link.linkAxis();
                EsyNetCfgLink::LinkAxisDir t_axis_dir = temp_link.linkAxisDir();
                assert( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_DOWN ||
                        t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP );

                // link dir
                EsyNetCfgLink::LinkDir t_dir = EsyNetCfgLink::LINKDIR_N_S;
                if ( t_axis == 0 )
                {
                    t_dir = ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                        ? EsyNetCfgLink::LINKDIR_W_E
                        : EsyNetCfgLink::LINKDIR_E_W;
                }
                else if ( t_axis == 1 )
                {
                    t_dir = ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                        ? EsyNetCfgLink::LINKDIR_N_S 
                        : EsyNetCfgLink::LINKDIR_S_N;
                }
                else if ( t_axis == 2 )
                {
                    t_dir = ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                        ? EsyNetCfgLink::LINKDIR_NW_SE
                        : EsyNetCfgLink::LINKDIR_SE_NW;
                }
                else
                {
                    t_dir = ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                        ? EsyNetCfgLink::LINKDIR_NE_SW
                        : EsyNetCfgLink::LINKDIR_SW_NE;
                }

                // neighbor node id, node port
                std::vector< long > t_coord_1 = t_coord;
                if ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                {
                    if ( t_coord_1[ t_axis ] >= m_size[ t_axis ] - 1 )
                    {
                        t_coord_1[ t_axis ] = 0;
                    }
                    else
                    {
                        t_coord_1[ t_axis ] += 1;
                    }
                }
                else 
                {
                    if ( t_coord_1[ t_axis ] <= 0 )
                    {
                        t_coord_1[ t_axis ] = m_size[ t_axis ] - 1;
                    }
                    else
                    {
                        t_coord_1[ t_axis ] -= 1;
                    }
                }
                long t_node_id = coord2Seq( t_coord_1 );
                long t_port_id =
                    ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                        ? port_id - 1 : port_id + 1;
                if ( !t_ring )
                {
                    if ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                    {
                        if ( t_coord[ t_axis ] >= m_size[ t_axis ] - 1 )
                        {
                            t_node_id = -1;
                            t_port_id = -1;
                        }
                    }
                    if ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_DOWN )
                    {
                        if ( t_coord[ t_axis ] == 0 )
                        {
                            t_node_id = -1;
                            t_port_id = -1;
                        }
                    }
                }

                if ( t_node_id >= 0 && t_port_id >= 0 )
                {
                    // link from this node to next node.
                    m_links.push_back( EsyNetCfgLink(
                        t_dir, t_axis, t_axis_dir,
                        EsyNetCfgPort( node_id, port_id, false,
                            temp_link.outputPort().bufs(),
                            temp_link.outputPort().outPeriod() ),
                        EsyNetCfgPort( t_node_id, t_port_id, false, 
                            temp_link.inputPort().bufs(),
                            temp_link.inputPort().outPeriod() ),
                        temp_link.niIntDelay() ) );
                }
            }
        }
    }
}

long EsyNetCfg::coord2Seq( const std::vector< long > & coord ) const
{
    long seq_t = coord[ m_size.size() - 1 ];
    for ( std::size_t i = 1; i < m_size.size(); i ++ )
    {
        seq_t = ( seq_t * m_size[ m_size.size() - i - 1 ] ) +
            coord[ m_size.size() - i - 1 ];
    }
    return seq_t;
}

std::vector< long > EsyNetCfg::seq2Coord( long seq ) const
{
    long seq_t = seq;
    std::vector< long > coord;
    for ( std::size_t i = 0; i < m_size.size(); i ++ )
    {
        coord.push_back( seq_t % m_size[ i ] );
        seq_t = seq_t / m_size[ i ];
    }
    return coord;
}

int EsyNetCfg::coordDistance( long p1, long p2 ) const
{
    std::vector< long > t_p1 = seq2Coord( p1 );
    std::vector< long > t_p2 = seq2Coord( p2 );
    int dist = 0;
    for ( std::size_t i = 0; i < m_size.size(); i ++ )
    {
        dist = dist + std::abs( t_p1[ i ] - t_p2[ i ] );
    }
    return dist;
}
