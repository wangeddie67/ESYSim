/*
 * File name : esy_netcfg_port.cc
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

#include "esy_conv.h"
#include "esy_netcfg_port.h"

#define NET_ARGU_ROUTER_PORT_INPUT_VC   "input_vc"
#define NET_ARGU_ROUTER_PORT_OUTPUT_VC  "output_vc"
#define NET_ARGU_ROUTER_PORT_DIRECTION  "port_dir"
#define NET_ARGU_ROUTER_PORT_AXIS   "port_axis"
#define NET_ARGU_ROUTER_PORT_AXIS_DIRECTION "port_axis_dir"
#define NET_ARGU_ROUTER_PORT_NEIGHBOR_ID    "neighbor_id"
#define NET_ARGU_ROUTER_PORT_NEIGHBOR_PORT  "neighbor_port"
#define NET_ARGU_ROUTER_PORT_INPUT_BUFFER   "input_buffer"
#define NET_ARGU_ROUTER_PORT_OUTPUT_BUFFER  "output_buffer"
#define NET_ARGU_ROUTER_PORT_NETWORK_INTERFACE  "ni"

std::string EsyNetworkCfgPort::const_port_direction[ ROUTER_PORT_NUM ] =
    { "North", "South", "West", "East", "NorthWest", "NorthEast", "SouthWest", "SouthEast" };

std::string EsyNetworkCfgPort::const_port_direction_short[ ROUTER_PORT_NUM ] =
    { "N", "S", "W", "E", "NW", "NE", "SW", "SE" };

std::string EsyNetworkCfgPort::const_port_axis_direction[ AXDIR_NUM ] =
    { "Upward", "Downward" };

void EsyNetworkCfgPort::readXml( TiXmlElement * root )
{
    // Loop all item behind root.
    for ( TiXmlNode * p_child = root->FirstChild(); p_child != NULL; p_child = p_child->NextSibling() )
    {
        TiXmlElement * t_element = p_child->ToElement();

        // Number of input virtual channel.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_INPUT_VC ) )
        {
            m_input_vc_num = EsyConvert( t_element->GetText() );
        }

        // Number of output virtual channel.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_OUTPUT_VC ) )
        {
            m_output_vc_num = EsyConvert( t_element->GetText() );
        }

        // Port direction in GUI.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_DIRECTION ) )
        {
            long t = EsyConvert( t_element->GetText() );
            m_port_dir = ( PortDirection )t;
        }

        // Port axis.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_AXIS ) )
        {
            m_port_axis = EsyConvert( t_element->GetText() );
        }

        // Direction on port axis.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_AXIS_DIRECTION ) )
        {
            long t = EsyConvert( t_element->GetText() );
            m_port_axis_dir = ( PortAxisDirection )t;
        }

        // Neighbor router id.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_NEIGHBOR_ID ) )
        {
            m_neighbor_router = EsyConvert( t_element->GetText() );
        }

        // Neighbor router port.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_NEIGHBOR_PORT ) )
        {
            m_neighbor_port = EsyConvert( t_element->GetText() );
        }

        // Size of input buffer.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_INPUT_BUFFER ) )
        {
            m_input_buffer = EsyConvert( t_element->GetText() );
        }

        // Size of output buffer.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_OUTPUT_BUFFER ) )
        {
            m_output_buffer = EsyConvert( t_element->GetText() );
        }

        // Network interface.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT_NETWORK_INTERFACE ) )
        {
            m_network_interface = EsyConvert( t_element->GetText() );
        }
    }
}

void EsyNetworkCfgPort::writeXml( TiXmlElement * root )
{
    TiXmlElement * t_item;

    // Number of input virtual channel.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_INPUT_VC );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_input_vc_num ).c_str() ) );
    root->LinkEndChild( t_item );

    // Number of output virtual channel.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_OUTPUT_VC );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_output_vc_num ).c_str() ) );
    root->LinkEndChild( t_item );

    // Port direction in GUI.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_DIRECTION );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_port_dir ).c_str() ) );
    root->LinkEndChild( t_item );

    // Axis of port.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_AXIS );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_port_axis ).c_str() ) );
    root->LinkEndChild( t_item );

    // Direction on axis.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_AXIS_DIRECTION );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_port_axis_dir ).c_str() ) );
    root->LinkEndChild( t_item );

    // Neighbor router id.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_NEIGHBOR_ID );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_neighbor_router ).c_str() ) );
    root->LinkEndChild( t_item );

    // Neighbor router port.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_NEIGHBOR_PORT );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_neighbor_port ).c_str() ) );
    root->LinkEndChild( t_item );

    // Size of input buffer.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_INPUT_BUFFER );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_input_buffer ).c_str() ) );
    root->LinkEndChild( t_item );

    // Size of output buffer.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_OUTPUT_BUFFER );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_output_buffer ).c_str() ) );
    root->LinkEndChild( t_item );

    // Network interface.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PORT_NETWORK_INTERFACE );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_network_interface ).c_str() ) );
    root->LinkEndChild( t_item );
}

std::ostream& operator<<( std::ostream & os, const EsyNetworkCfgPort & port_cfg )
{
    os << "input_vc=" << port_cfg.m_input_vc_num << " ";
    os << "input_buffer=" << port_cfg.m_input_buffer << " ";
    os << "output_vc=" << port_cfg.m_output_vc_num << " ";
    os << "output_buffer=" << port_cfg.m_output_buffer << " ";
    if ( port_cfg.m_network_interface )
    {
        os << "axis=NI ";
    }
    else
    {
        os << "axis=" << port_cfg.m_port_axis << " ";
        os << "direction=" << port_cfg.portAxisDirectionStr() << " ";
    }
    os << "neighbor=(" << port_cfg.m_neighbor_router << "," << port_cfg.m_neighbor_port << ") ";

    return os;
}

