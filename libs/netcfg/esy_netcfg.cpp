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
               const std::vector<EsyNetCfgLink>& temp_links )
    : m_topology( topology )
    , m_size( axis_size )
{
    // assert topology and network size
    std::size_t t_dim = m_size.size();
    assert( ( topology == NT_MESH_2D ) && ( t_dim == 2 ) );
    assert( ( topology == NT_TORUS_2D ) && ( t_dim == 2 ) );
    assert( ( topology == NT_MESH_DIA ) && ( t_dim > 0 ) );
    assert( ( topology == NT_TORUS_DIA ) && ( t_dim > 0 ) );
    assert( ( topology == NT_RING ) && ( t_dim == 1 ) );
    assert( ( topology != NT_IRREGULAR ) );

    // the topology is based on ring or bus.
    bool t_ring = ( topology == NT_TORUS_2D )
               || ( topology == NT_TORUS_DIA )
               || ( topology == NT_RING );

    long port_num = temp_links.size();
    long m_ni_num = 0;
    // loop router id to create and connect links
    for ( long router_id = 0; router_id < m_router_num; router_id ++ )
    {
        std::vector< long > t_coord = seq2Coord( router_id );
        for ( long port_id = 0; port_id < port_num; port_id ++ )
        {
            const EsyNetCfgLink& temp_link = temp_links[ port_id ];
            // port 0 connects to NI.
            if ( temp_link.outputPort().isNi() )
            {
                // link from port 0 to NI
                m_links.push_back( EsyNetCfgLink(
                    EsyNetCfgLink::LINKDIR_SW_NE, -1,
                    EsyNetCfgLink::LINKAXISDIR_NUM,
                    EsyNetCfgPort( router_id, port_id, false,
                        temp_link.outputPort().bufs(),
                        temp_link.outputPort().outPeriod() ),
                    EsyNetCfgPort( m_ni_num, 0, true, 
                        temp_link.inputPort().bufs(),
                        temp_link.inputPort().outPeriod() ),
                    temp_link.niIntDelay() ) );
                // link from NI to port 0
                m_links.push_back( EsyNetCfgLink(
                    EsyNetCfgLink::LINKDIR_NE_SW, -1,
                    EsyNetCfgLink::LINKAXISDIR_NUM,
                    EsyNetCfgPort( m_ni_num, 0, true,
                        temp_link.outputPort().bufs(),
                        temp_link.outputPort().outPeriod() ),
                    EsyNetCfgPort( router_id, port_id, false, 
                        temp_link.inputPort().bufs(),
                        temp_link.inputPort().outPeriod() ),
                    temp_link.niIntDelay() ) );
                m_ni_num ++;
            }
            else
            {
                // axis and axis dir
                long t_axis = temp_link.linkDir();
                EsyNetCfgLink::LinkAxisDir t_axis_dir = temp_link.linkAxisDir();
                assert( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_DOWN ||
                        t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP );

                // link dir
                EsyNetCfgLink::LinkDir t_dir = EsyNetCfgLink::LINKDIR_N_S;
                if ( t_axis == 0 )
                {
                    t_dir = (t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP)
                        ? EsyNetCfgLink::LINKDIR_W_E
                        : EsyNetCfgLink::LINKDIR_E_W;
                }
                else if ( t_axis == 1 )
                {
                    t_dir = (t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP)
                        ? EsyNetCfgLink::LINKDIR_N_S 
                        : EsyNetCfgLink::LINKDIR_S_N;
                }
                else if ( t_axis == 2 )
                {
                    t_dir = (t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP)
                        ? EsyNetCfgLink::LINKDIR_NW_SE
                        : EsyNetCfgLink::LINKDIR_SE_NW;
                }
                else
                {
                    t_dir = (t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP)
                        ? EsyNetCfgLink::LINKDIR_NE_SW
                        : EsyNetCfgLink::LINKDIR_SW_NE;
                }

                // neighbor router id, router port
                long t_router_id = (t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP)
                        ? router_id + 1 : router_id - 1;
                long t_port_id = (t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP)
                        ? port_id + 1 : port_id - 1;
                if ( !t_ring )
                {
                    if ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_UP )
                    {
                        if ( t_coord[ t_axis ] >= m_size[ t_axis ] - 1 )
                        {
                            t_router_id = -1;
                            t_port_id = -1;
                        }
                    }
                    if ( t_axis_dir == EsyNetCfgLink::LINKAXISDIR_DOWN )
                    {
                        if ( t_coord[ t_axis ] == 0 )
                        {
                            t_router_id = -1;
                            t_port_id = -1;
                        }
                    }
                }

                if ( t_router_id >= 0 && t_port_id >= 0 )
                {
                    // link from this router to next router.
                    m_links.push_back( EsyNetCfgLink(
                        t_dir, t_axis, t_axis_dir,
                        EsyNetCfgPort( router_id, port_id, false,
                            temp_link.outputPort().bufs(),
                            temp_link.outputPort().outPeriod() ),
                        EsyNetCfgPort( t_router_id, t_port_id, false, 
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

std::ostream& operator<<( std::ostream & os, const EsyNetCfg & net_cfg )
{
    os << "Topology=";
    switch( net_cfg.topology() )
    {
        case EsyNetCfg::NT_SWITCH : os << "Switch"; break;
        case EsyNetCfg::NT_RING: os << "Ring (1D, ring)"; break;
        case EsyNetCfg::NT_MESH_2D: os << "2D-Mesh (2D, bus)"; break;
        case EsyNetCfg::NT_TORUS_2D: os << "2D-Torus (2D, ring)"; break;
        case EsyNetCfg::NT_MESH_DIA: os << "Dia-Mesh (Dia, bus)"; break;
        case EsyNetCfg::NT_TORUS_DIA: os << "Dia-Torus (Dia, ring)"; break;
        case EsyNetCfg::NT_IRREGULAR: os << "Irregular"; break;
        default : os << "Unknown topology"; break;
    }
    os << std::endl;
    os << "Size={" << net_cfg.size()[ 0 ];
    for ( std::size_t i = 1; i < net_cfg.size().size(); i ++ )
    {
        os << "," << net_cfg.size()[ i ];
    }
    os << "}" << std::endl;
    os << std::endl;

    os << "Link list :" << std::endl;

    for ( std::size_t link_id = 0; link_id < net_cfg.links().size(); link_id ++ )
    {
        const EsyNetCfgLink& link_cfg = net_cfg.links()[ link_id ];
        if ( link_cfg.outputPort().isNi() )
        {
            os << std::setw(4) << link_cfg.outputPort().id();
            os << std::setw(4) << link_cfg.outputPort().port();
            os << std::setw(4) << link_cfg.outputPort().vc();
            os << std::setw(8) << link_cfg.outputPort().outPeriod();
        }
        else
        {
            os << std::setw(4) << link_cfg.outputPort().id();
            os << "(NI)";
            os << "    ";
            os << "        ";
        }
        os << " -- ";
        os << link_cfg.linkDir();
        os << std::setw(4) << link_cfg.linkAxis();
        os << link_cfg.linkAxisDir();
        os << " --> ";
        if ( link_cfg.inputPort().isNi() )
        {
            os << std::setw(4) << link_cfg.inputPort().id();
            os << std::setw(4) << link_cfg.inputPort().port();
            os << std::setw(4) << link_cfg.outputPort().vc();
            os << std::setw(8) << link_cfg.inputPort().outPeriod();
        }
        else
        {
            os << std::setw(4) << link_cfg.inputPort().id();
            os << "(NI)";
            os << "    ";
            os << std::setw(8) << link_cfg.niIntDelay();
        }
    }

    return os;
}
