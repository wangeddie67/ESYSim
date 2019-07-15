/*
 * File name : esy_netcfg_port.h
 * Function : Declare network configuration structure for ports.
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

#ifndef INTERFACE_NETCFG_PORT_H
#define INTERFACE_NETCFG_PORT_H

#include <iostream>

#include "tinyxml.h"

/**
 * @ingroup networkcfggroup
 * @brief Structure of port configuration item.
 */
class EsyNetworkCfgPort
{
public:
    /**
     * @brief Port direction in GUI.
     *
     * In Graphics User Interface, a router has eight edges and one port can be placed at one of them.
     */
    enum PortDirection
    {
        ROUTER_PORT_NORTH,      /*!< @brief North edge */
        ROUTER_PORT_SOUTH,      /*!< @brief South edge */
        ROUTER_PORT_WEST,       /*!< @brief West edge */
        ROUTER_PORT_EAST,       /*!< @brief East edge */
        ROUTER_PORT_NORTHWEST,  /*!< @brief North-West edge */
        ROUTER_PORT_NORTHEAST,  /*!< @brief North-East edge */
        ROUTER_PORT_SOUTHWEST,  /*!< @brief South-West edge */
        ROUTER_PORT_SOUTHEAST,  /*!< @brief South-East edge */
        ROUTER_PORT_NUM         /*!< @brief Count of edges */
    };

    /**
     * @brief Port direction in axis.
     */
    enum PortAxisDirection
    {
        AXDIR_UP,  /*!< @brief Increase id on axis. */
        AXDIR_DOWN,  /*!< @brief Decrease id on axis. */
        AXDIR_NUM,  /*!< @brief Count of axis direction. */
    };

protected:
    static std::string const_port_direction[ ROUTER_PORT_NUM ];         /*!< @brief Full name of port direction. */
    static std::string const_port_direction_short[ ROUTER_PORT_NUM ];   /*!< @brief Short name of port direction. */
    static std::string const_port_axis_direction[ AXDIR_NUM ];          /*!< @brief Name of port direction in axis. */

    long m_input_vc_num;                /*!< @brief Number of input virtual channels. */
    long m_output_vc_num;               /*!< @brief Number of output virtual channels. */
    PortDirection m_port_dir;           /*!< @brief Direction of port in GUI. */
    long m_port_axis;                   /*!< @brief Axis of the port. */
    PortAxisDirection m_port_axis_dir;  /*!< @brief Direction of port on axis. */
    long m_neighbor_router;             /*!< @brief Id of connected router in output direction. */
    long m_neighbor_port;               /*!< @brief Id of connected neighbor port in output direction. */
    long m_input_buffer;                /*!< @brief Size of input buffer. */
    long m_output_buffer;               /*!< @brief Size of output buffer. */
    bool m_network_interface;           /*!< @brief Port connects network interface or not. */

public:
    /**
     * @name Constructor Function
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetworkCfgPort()
        : m_input_vc_num( 1 )
        , m_output_vc_num( 1 )
        , m_port_dir( ROUTER_PORT_SOUTHEAST )
        , m_port_axis( 0 )
        , m_port_axis_dir( AXDIR_DOWN )
        , m_neighbor_router( -1 )
        , m_neighbor_port( -1 )
        , m_input_buffer( 12 )
        , m_output_buffer( 12 )
        , m_network_interface( false )
    {}
    /**
     * @brief Constructs an item with specified value for some of fields.
     * @param input_vc_num  Number of input virtual channel.
     * @param output_vc_num Number of output virtual channel.
     * @param input_buffer  Size of input buffer.
     * @param output_buffer Size of output buffer.
     */
    EsyNetworkCfgPort( int input_vc_num, int output_vc_num, int input_buffer, int output_buffer )
        : m_input_vc_num( input_vc_num )
        , m_output_vc_num( output_vc_num )
        , m_port_dir( ROUTER_PORT_SOUTHEAST )
        , m_port_axis( 0 )
        , m_port_axis_dir( AXDIR_DOWN )
        , m_neighbor_router( -1 )
        , m_neighbor_port( -1 )
        , m_input_buffer( input_buffer )
        , m_output_buffer( output_buffer )
        , m_network_interface( false )
    {}
    /**
     * @brief Constructs an item with specified value for fields.
     * @param input_vc_num      Number of input virtual channel.
     * @param output_vc_num     Number of output virtual channel.
     * @param dir               Port direction in GUI.
     * @param axis              Axis of the port.
     * @param axis_dir          Direction of port on axis.
     * @param neighbor_router   Neighbor router.
     * @param neighbor_port     Neighbor port.
     * @param input_buffer      Size of input buffer.
     * @param output_buffer     Size of output buffer.
     * @param ni                Network interface flag of connection.
     */
    EsyNetworkCfgPort( int input_vc_num, int output_vc_num, PortDirection dir, long axis, PortAxisDirection axis_dir
        , long neighbor_router, long neighbor_port, int input_buffer, int output_buffer, bool ni
    )
        : m_input_vc_num( input_vc_num )
        , m_output_vc_num( output_vc_num )
        , m_port_dir( dir )
        , m_port_axis( axis )
        , m_port_axis_dir( axis_dir )
        , m_neighbor_router( neighbor_router )
        , m_neighbor_port( neighbor_port )
        , m_input_buffer( input_buffer )
        , m_output_buffer( output_buffer )
        , m_network_interface( ni )
    {}
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetworkCfgPort( const EsyNetworkCfgPort & t )
        : m_input_vc_num( t.m_input_vc_num )
        , m_output_vc_num( t.m_output_vc_num )
        , m_port_dir( t.m_port_dir )
        , m_port_axis( t.m_port_axis )
        , m_port_axis_dir( t.m_port_axis_dir )
        , m_neighbor_router( t.m_neighbor_router )
        , m_neighbor_port( t.m_neighbor_port )
        , m_input_buffer( t.m_input_buffer )
        , m_output_buffer( t.m_output_buffer )
        , m_network_interface( t.m_network_interface )
    {}
    /**
     * @}
     */

    /**
     * @name Overload operator
     * @{
     */
    /**
     * @brief Overload operator == function. Compare value of two items.
     * @param t  item to compare.
     * @return If t has the same value as this item at all field, return TRUE.
     */
    inline bool operator ==(const EsyNetworkCfgPort & t) const
    {
        if ( m_input_vc_num == t.m_input_vc_num && m_output_vc_num == t.m_output_vc_num
            && m_port_dir == t.m_port_dir && m_port_axis == t.m_port_axis && m_port_axis_dir == t.m_port_axis_dir
            && m_neighbor_router == t.m_neighbor_router && m_neighbor_port == t.m_neighbor_port
            && m_input_buffer == t.m_input_buffer && m_output_buffer == t.m_output_buffer
            && m_network_interface == t.m_network_interface )
        {
            return true;
        }
        return false;
    }
    /**
     * @brief Overload operator << function. Print NI configuration to output stream.
     * @param os output stream.
     * @param port_cfg Port configuration structure.
     * @return output stream.
     */
    friend std::ostream& operator<<( std::ostream & os, const EsyNetworkCfgPort & port_cfg );
    /**
     * @}
     */

    /**
     * @name Functions to access variables
     * @{
     */
    /**
     * @brief Return number of input virtual channels.
     */
    inline long inputVcNumber() const { return m_input_vc_num; }
    /**
     * @brief Return number of output virtual channels.
     */
    inline long outputVcNumber() const { return m_output_vc_num; }
    /**
     * @brief Return direction of port in GUI.
     */
    inline PortDirection portDirection() const { return m_port_dir; }
    /**
     * @brief Return the short name of port direction of this port.
     */
    inline const std::string portDirectionStrShort() const { return const_port_direction_short[ m_port_dir ]; }
    /**
     * @brief Return the full name of port direction of this port.
     */
    inline const std::string portDirectionStr() const { return const_port_direction[ m_port_dir ]; }
    /**
     * @brief Return the axis of this port.
     */
    inline long portAxis() const { return m_port_axis; }
    /**
     * @brief Return the direction on axis.
     */
    inline PortAxisDirection portAxisDir() const { return m_port_axis_dir; }
    /**
     * @brief Return the name of axis port direction of this port.
     */
    inline const std::string portAxisDirectionStr() const { return const_port_axis_direction[ m_port_axis_dir ]; }
    /**
     * @brief Return id of connected router in output direction.
     */
    inline long neighborRouter() const { return m_neighbor_router; }
    /**
     * @brief Return id of connected neighbor port in output direction.
     */
    inline long neighborPort() const { return m_neighbor_port; }
    /**
     * @brief Return size of input buffer.
     */
    inline long inputBuffer() const { return m_input_buffer; }
    /**
     * @brief Return size of output buffer.
     */
    inline long outputBuffer() const { return m_output_buffer; }
    /**
     * @brief Return network interface flag of port connection.
     */
    inline bool networkInterface() const { return m_network_interface; }
    /**
     * @}
     */

    /**
     * @name Functions to set variables
     * @{
     */
    /**
     * @brief Set number of input virtual channels.
     */
    inline void setInputVcNumber( long vc_num ) { m_input_vc_num = vc_num; }
    /**
     * @brief Set number of output virtual channels.
     */
    inline void setOutputVcNumber( long vc_num ) { m_output_vc_num = vc_num; }
    /**
     * @brief Set direction of port in GUI.
     */
    inline void setPortDirection( PortDirection dir ) { m_port_dir = dir; }
    /**
     * @brief Set axis of this port.
     */
    inline void setPortAxis( long axis ) { m_port_axis = axis; }
    /**
     * @brief Set direction on axis.
     */
    inline void setPortAxisDir( PortAxisDirection dir ) { m_port_axis_dir = dir; }
    /**
     * @brief Set id of connected router in output direction.
     */
    inline void setNeighborRouter( long id ) { m_neighbor_router = id; }
    /**
     * @brief set id of connected neighbor port in output direction.
     */
    inline void setNeighborPort( long port ) { m_neighbor_port = port; }
    /**
     * @brief Set size of input buffer.
     */
    inline void setInputBuffer( long buf ) { m_input_buffer = buf; }
    /**
     * @brief Set size of output buffer.
     */
    inline void setOutputBuffer( long buf ) { m_output_buffer = buf; }
    /**
     * @brief Set Network interface flag of port connection.
     */
    inline void setNetworkInterface( bool ni ) { m_network_interface = ni; }
    /**
     * @}
     */

    /**
     * @brief Access the full name of specified direction.
     * @param i  code of direction.
     * @return  the full name of specified direction.
     */
    static std::string portDirection2Str( int i ) { return const_port_direction[ i ]; }
    /**
     * @brief Access the short name of specified direction.
     * @param i  code of direction.
     * @return  the short name of specified direction.
     */
    static std::string portDirection2ShortStr( int i ) { return const_port_direction_short[ i ]; }
    /**
     * @brief Access the name of specified axis direction.
     * @param i  code of direction.
     * @return  the short name of specified axis direction.
     */
    static std::string portAxisDirection2Str( int i ) { return const_port_axis_direction[ i ]; }

    /**
     * @name Functions to access network configuration file in XML
     * @{
     */
    /**
     * @brief Read port configuration from XML file.
     * @param root  root of XML structure.
     */
    void readXml( TiXmlElement * root );
    /**
     * @brief Write port configuration to XML file.
     * @param root  root of XML structure.
     */
    void writeXml( TiXmlElement * root );
    /**
     * @}
     */

    /**
     * @name Maximum and total value
     * @{
     */
    /**
     * @brief Return total virtual channel. Sum of input and output virtual channel.
     */
    inline long totalVcNumber() const { return m_input_vc_num + m_output_vc_num; }
    /**
     * @}
     */
};

#endif
