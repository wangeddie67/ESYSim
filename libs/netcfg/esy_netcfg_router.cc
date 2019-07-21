/*
 * File name : esy_netcfg_router.cc
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
#include "esy_netcfg_router.h"

#define NET_ARGU_ROUTER "router_cfg"
#define NET_ARGU_ROUTER_ID  "id"
#define NET_ARGU_ROUTER_POS "position"
#define NET_ARGU_ROUTER_POS_X   "x"
#define NET_ARGU_ROUTER_POS_Y   "y"
#define NET_ARGU_ROUTER_PIPE    "pipe_cycle"
#define NET_ARGU_ROUTER_PORT    "port_cfg"

#define NET_ARGU_DATA   "data"
#define NET_ARGU_SIZE   "size"
#define NET_ARGU_INDEX  "index"

std::string EsyNetCfgRouter::portName( long i ) const
{
    std::string t_str;
    if ( m_port[ i ].networkInterface() )
    {
        t_str = std::string( "L." ) + (std::string)EsyConvert( i );
    }
    else
    {
        t_str = m_port[ i ].portDirectionStrShort() + "." + (std::string)EsyConvert( i );
    }
    return t_str;
}

void EsyNetCfgRouter::readXml( TiXmlElement * root )
{
    // Loop all item behind root.
    for ( TiXmlNode * p_child = root->FirstChild(); p_child != NULL; p_child = p_child->NextSibling() )
    {
        TiXmlElement * t_element = p_child->ToElement();

        // Router id element.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_ID ) )
        {
            m_id = EsyConvert( t_element->GetText() );
        }

        // Position in GUI.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_POS ) )
        {
            for ( TiXmlNode * p_pos_child = t_element->FirstChild(); p_pos_child != NULL; p_pos_child = p_pos_child->NextSibling() )
            {
                // Position in one axis in GUI.
                TiXmlElement * t_pos_element = p_pos_child->ToElement();
                if ( t_pos_element->Value() == std::string( NET_ARGU_ROUTER_POS_X ) )
                {
                     m_pos.first = EsyConvert( t_pos_element->GetText() );
                }
                if ( t_pos_element->Value() == std::string( NET_ARGU_ROUTER_POS_Y ) )
                {
                    m_pos.second = EsyConvert( t_pos_element->GetText() );
                }
            }
        }

        // Ports.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT ) )
        {
            // Size of port.
            m_port.resize( EsyConvert( t_element->Attribute( NET_ARGU_SIZE ) ) );

            for ( TiXmlNode * p_port_child = t_element->FirstChild(); p_port_child != NULL; p_port_child = p_port_child->NextSibling() )
            {
                // Port configuration.
                TiXmlElement * t_port_element = p_port_child->ToElement();
                m_port[ EsyConvert( t_port_element->Attribute( NET_ARGU_INDEX ) ) ].readXml( t_port_element );
            }
        }

        // Pipeline cycle.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PIPE ) )
        {
            m_pipe_cycle = EsyConvert( t_element->GetText() );
        }
    }
}

void EsyNetCfgRouter::writeXml( TiXmlElement * root )
{
    TiXmlElement * t_item;

    // Router id.
    t_item = new TiXmlElement( NET_ARGU_ROUTER_ID );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_id ).c_str() ) );
    root->LinkEndChild( t_item );

    // Position in GUI.
    TiXmlElement * t_pos_item = new TiXmlElement( NET_ARGU_ROUTER_POS );
    // Position in x-axis in GUI.
    TiXmlElement * t_pos_x_item = new TiXmlElement( NET_ARGU_ROUTER_POS_X );
    t_pos_x_item->LinkEndChild( new TiXmlText( EsyConvert( m_pos.first ).c_str() ) );
    t_pos_item->LinkEndChild( t_pos_x_item );
    // Position in y-axis in GUI.
    TiXmlElement * t_pos_y_item = new TiXmlElement( NET_ARGU_ROUTER_POS_Y );
    t_pos_y_item->LinkEndChild( new TiXmlText( EsyConvert( m_pos.second ).c_str() ) );
    t_pos_item->LinkEndChild( t_pos_y_item );
    root->LinkEndChild( t_pos_item );

    // Ports
    TiXmlElement * t_port_list_item = new TiXmlElement( NET_ARGU_ROUTER_PORT );
    t_port_list_item->SetAttribute( NET_ARGU_SIZE, (int)m_port .size() );
    for ( std::size_t i = 0; i < m_port .size(); i ++ )
    {
        // Port configuration.
        TiXmlElement * t_port_item = new TiXmlElement( NET_ARGU_DATA );
        t_port_item->SetAttribute( NET_ARGU_INDEX, (int)i );
        m_port[ i ].writeXml( t_port_item );
        t_port_list_item->LinkEndChild( t_port_item );
    }
    root->LinkEndChild( t_port_list_item );

    // Pipeline cycle
    t_item = new TiXmlElement( NET_ARGU_ROUTER_PIPE );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_pipe_cycle ).c_str() ) );
    root->LinkEndChild( t_item );
}

long int EsyNetCfgRouter::maxVcNum() const
{
    return ( maxInputVcNum() > maxOutputVcNum() ) ? maxInputVcNum() : maxOutputVcNum();
}

long int EsyNetCfgRouter::maxInputVcNum() const
{
    long vcnum = 1;
    for ( long i = 0; i < portNum(); i ++ )
    {
        if ( m_port[ i ].inputVcNumber() > vcnum )
        {
            vcnum = m_port[ i ].inputVcNumber();
        }
    }
    return vcnum;
}

long int EsyNetCfgRouter::maxOutputVcNum() const
{
    long vcnum = 1;
    for ( long i = 0; i < portNum(); i ++ )
    {
        if ( m_port[ i ].outputVcNumber() > vcnum )
        {
            vcnum = m_port[ i ].outputVcNumber();
        }
    }
    return vcnum;
}

long int EsyNetCfgRouter::maxInputBuffer() const
{
    long inputbuffer = 1;
    for ( long i = 0; i < portNum(); i ++ )
    {
        if ( m_port[ i ].inputBuffer() > inputbuffer )
        {
            inputbuffer = m_port[ i ].inputBuffer();
        }
    }
    return inputbuffer;
}

long int EsyNetCfgRouter::maxOutputBuffer() const
{
    long outputbuffer = 1;
    for ( long i = 0; i < portNum(); i ++ )
    {
        if ( m_port[ i ].outputBuffer() > outputbuffer )
        {
            outputbuffer = m_port[ i ].outputBuffer();
        }
    }
    return outputbuffer;
}

long int EsyNetCfgRouter::totalInputVc() const
{
    long inputvc = 0;
    for ( long i = 0; i < portNum(); i ++ )
    {
        inputvc += m_port[ i ].inputVcNumber();
    }
    return inputvc;
}

long int EsyNetCfgRouter::totalOutputVc() const
{
    long outputvc = 0;
    for ( long i = 0; i < portNum(); i ++ )
    {
        outputvc += m_port[ i ].outputVcNumber();
    }
    return outputvc;
}

std::ostream& operator<<( std::ostream & os, const EsyNetCfgRouter & router_cfg )
{
    os << "Router(" << router_cfg.m_id << ") ";
    os << "pipe_cycle=" << router_cfg.m_pipe_cycle << " ";
    for ( int i = 0; i < router_cfg.portNum(); i ++ )
    {
        os << std::endl;
        os << "\t" << "port=" << i << " " << router_cfg.port( i );
    }

    return os;
}

