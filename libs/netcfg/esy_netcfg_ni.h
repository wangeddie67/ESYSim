/*
 * File name : esy_netcfg_ni.h
 * Function : Declare network configuration structure for network interface.
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

#ifndef INTERFACE_NETCFG_NI_H
#define INTERFACE_NETCFG_NI_H

#include <iostream>

#include "tinyxml.h"

/**
 * @ingroup networkcfggroup
 * @brief Structure of NI configuration item.
 */
class EsyNetCfgNI
{
protected:
    long m_id;                  /*!< @brief NI id. */
    long m_connect_router;      /*!< @brief Connected router id. */
    long m_connect_port;        /*!< @brief Connected port id. */
    double m_pipe_cycle;        /*!< @brief Pipeline frequency of one router, used by frequency scaling, unit by cycle. */
    long m_buffer_size;         /*!< @brief Size of ejection buffer. */
    double m_interrupt_delay;   /*!< @brief Delay to response NI interrupt. */

public:
    /**
     * @name Constructor Functions
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetCfgNI()
        : m_id( 0 )
        , m_connect_router( 0 )
        , m_connect_port( 0 )
        , m_pipe_cycle( 1.0 )
        , m_buffer_size( 12 )
        , m_interrupt_delay( 200 )
    {}
    /**
     * @brief Constructs an item with some of specified field.
     * @param pipe_cycle      Pipeline cycle.
     * @param buffer_size     Size of ejection buffer.
     * @param interrupt_delay Delay to response NI interrupt.
     */
    EsyNetCfgNI( double pipe_cycle, long buffer_size, double interrupt_delay )
        : m_id( 0 )
        , m_connect_router( 0 )
        , m_connect_port( 0 )
        , m_pipe_cycle( pipe_cycle )
        , m_buffer_size( buffer_size )
        , m_interrupt_delay( interrupt_delay )
    {}
    /**
     * @brief Constructs an item with specified field.
     * @param id              Network interface id.
     * @param router          Connect router id.
     * @param port            Connect port id.
     * @param pipe_cycle      Pipeline cycle.
     * @param buffer_size     Size of ejection buffer.
     * @param interrupt_delay Delay to response NI interrupt.
     */
    EsyNetCfgNI( long id, long router, long port, double pipe_cycle, long buffer_size, double interrupt_delay )
        : m_id( id )
        , m_connect_router( router )
        , m_connect_port( port )
        , m_pipe_cycle( pipe_cycle )
        , m_buffer_size( buffer_size )
        , m_interrupt_delay( interrupt_delay )
    {}
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetCfgNI( const EsyNetCfgNI & t )
        : m_id( t.m_id )
        , m_connect_router( t.m_connect_router )
        , m_connect_port( t.m_connect_port )
        , m_pipe_cycle( t.m_pipe_cycle )
        , m_buffer_size( t.m_buffer_size )
        , m_interrupt_delay( t.m_interrupt_delay )
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
    inline bool operator ==( const EsyNetCfgNI & t ) const
    {
        if ( m_pipe_cycle == t.m_pipe_cycle && m_buffer_size == t.m_buffer_size && m_interrupt_delay == t.m_interrupt_delay )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    /**
     * @brief Overload operator << function. Print NI configuration to output stream.
     * @param os output stream.
     * @param ni_cfg Network interface configuration structure.
     * @return output stream.
     */
    friend std::ostream& operator<<( std::ostream & os, const EsyNetCfgNI & ni_cfg );
    /**
     * @}
     */

    /**
     * @name Functions to access variables
     * @{
     */
    /**
     * @brief Return router id.
     */
    inline long niId() const { return m_id; }
    /**
     * @brief Return connected router.
     */
    inline long connectRouter() const { return m_connect_router; }
    /**
     * @brief Return connected port.
     */
    inline long connectPort() const { return m_connect_port; }
    /**
     * @brief Return connected router and port.
     */
    inline std::pair< long, long > connect() const { return std::pair< long, long >( m_connect_router, m_connect_port ); }
    /**
     * @brief Return the pipeline frequency of this router.
     */
    inline double pipeCycle() const { return m_pipe_cycle; }
    /**
     * @brief Return the size of ejection buffer.
     */
    inline long bufferSize() const { return m_buffer_size; }
    /**
     * @brief Return the interrupt delay.
     */
    inline double interruptDelay() const { return m_interrupt_delay; }
    /**
     * @}
     */

    /**
     * @name Functions to set variables
     * @{
     */
    /**
     * @brief Set NI id.
     */
    inline void setNiId( long id ) { m_id = id; }
    /**
     * @brief Return connected router.
     */
    inline void setConnectRouter( long router ) { m_connect_router = router; }
    /**
     * @brief Return connected port.
     */
    inline void setConnectPort( long port ) { m_connect_port = port; }
    /**
     * @brief Return connected router and port.
     */
    inline void setConnect( const std::pair< long, long >& connect ) { m_connect_router = connect.first, m_connect_port = connect.second; }
    /**
     * @brief Set the size of ejection buffer.
     */
    inline void setBufferSize( long buffer_size ) { m_buffer_size = buffer_size; }
    /**
     * @brief Set pipeline frequency of this router.
     */
    inline void setPipeCycle( double pipe_cycle ) { m_pipe_cycle = pipe_cycle; }
    /**
     * @brief Set interrupt delay.
     */
    inline void setInterruptDelay( double delay ) { m_interrupt_delay = delay; }
    /**
     * @}
     */

    /**
     * @name Functions to access network configuration file in XML
     * @{
     */
    /**
     * @brief Read router configuration from XML file.
     * @param root  root of XML structure.
     */
    void readXml( TiXmlElement * root );
    /**
     * @brief Write router configuration to XML file.
     * @param root  root of XML structure.
     */
    void writeXml( TiXmlElement * root );
    /**
     * @}
     */
};

#endif
