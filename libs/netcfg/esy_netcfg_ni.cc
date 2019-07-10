/*
 * File name : esy_netcfg_ni.cc
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
#include "esy_netcfg_ni.h"

#define NET_ARGU_NI_ID              "id"
#define NET_ARGU_NI_CONNECT_ROUTER  "connect_router"
#define NET_ARGU_NI_CONNECT_PORT    "connect_port"
#define NET_ARGU_NI_PIPE_CYCLE      "pipe_cycle"
#define NET_ARGU_NI_BUFFER_SIZE     "buffer_size"
#define NET_ARGU_NI_INTERRUPT_DELAY "interrupt_delay"

void EsyNetworkCfgNI::readXml( TiXmlElement * root )
{
    // Loop all item behind root.
    for ( TiXmlNode * p_child = root->FirstChild(); p_child != NULL; p_child = p_child->NextSibling() )
    {
        TiXmlElement * t_element = p_child->ToElement();

        // NI id.
        if ( t_element->Value() == std::string( NET_ARGU_NI_ID ) )
        {
            m_id = EsyConvert( t_element->GetText() );
        }

        // Connect router.
        if ( t_element->Value() == std::string( NET_ARGU_NI_CONNECT_ROUTER ) )
        {
            m_connect_router = EsyConvert( t_element->GetText() );
        }

        // Connect port.
        if ( t_element->Value() == std::string( NET_ARGU_NI_CONNECT_PORT ) )
        {
            m_connect_port = EsyConvert( t_element->GetText() );
        }

        // Pipe cycle.
        if ( t_element->Value() == std::string( NET_ARGU_NI_PIPE_CYCLE ) )
        {
            m_pipe_cycle = EsyConvert( t_element->GetText() );
        }

        // Size of ejection size.
        if ( t_element->Value() == std::string( NET_ARGU_NI_BUFFER_SIZE ) )
        {
            m_buffer_size = EsyConvert( t_element->GetText() );
        }

        // Delay of interrupt response.
        if ( t_element->Value() == std::string( NET_ARGU_NI_INTERRUPT_DELAY ) )
        {
            m_interrupt_delay = EsyConvert( t_element->GetText() );
        }
    }
}
#define writeItem( root, value, var ) \
TiXmlElement * t_##var##_item = new TiXmlElement( value );\
t_##var##_item->LinkEndChild( \
    new TiXmlText( EsyConvert( var ).c_str() ) ); \
root->LinkEndChild( t_##var##_item ); \

void EsyNetworkCfgNI::writeXml( TiXmlElement * root )
{
    TiXmlElement * t_item;

    // NI id.
    t_item = new TiXmlElement( NET_ARGU_NI_ID );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_id ).c_str() ) );
    root->LinkEndChild( t_item );

    // Connect router.
    t_item = new TiXmlElement( NET_ARGU_NI_CONNECT_ROUTER );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_connect_router ).c_str() ) );
    root->LinkEndChild( t_item );

    // Connect port.
    t_item = new TiXmlElement( NET_ARGU_NI_CONNECT_PORT );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_connect_port ).c_str() ) );
    root->LinkEndChild( t_item );

    // Pipe cycle.
    t_item = new TiXmlElement( NET_ARGU_NI_PIPE_CYCLE );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_pipe_cycle ).c_str() ) );
    root->LinkEndChild( t_item );

    // Size of ejection size.
    t_item = new TiXmlElement( NET_ARGU_NI_BUFFER_SIZE );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_buffer_size ).c_str() ) );
    root->LinkEndChild( t_item );

    // Delay of interrupt response.
    t_item = new TiXmlElement( NET_ARGU_NI_INTERRUPT_DELAY );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_interrupt_delay ).c_str() ) );
    root->LinkEndChild( t_item );
}

std::ostream& operator<<( std::ostream & os, const EsyNetworkCfgNI & ni_cfg )
{
    os << "NI(" << ni_cfg.m_id << ") ";
    os << "connecion=(" << ni_cfg.m_connect_router << "," << ni_cfg.m_connect_port << ") ";
    os << "pipe_cycle=" << ni_cfg.m_pipe_cycle << " ";
    os << "eject_buffer_size=" << ni_cfg.m_buffer_size << " ";
    os << "interrupt_delay=" << ni_cfg.m_interrupt_delay;

    return os;
}
