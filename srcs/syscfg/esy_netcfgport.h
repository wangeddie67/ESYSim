/*
 * File name : esy_netcfg_port.h
 * Function : Declare configuration structure for ports.
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

#ifndef LIBS_NETCFG_PORT_H_
#define LIBS_NETCFG_PORT_H_

#include <vector>

/**
 * @ingroup networkcfggroup
 * @brief Configuration structure of one port.
 */
class EsyNetCfgPort
{
private:
    long m_node;    /*!< @brief Router/NI id. */
    long m_port;    /*!< @brief Port id. */
    bool m_ni;      /*!< @brief The component is NI. */

    std::vector< long > m_buf;  /*!< @brief Virtual channel and buffer size. */
    double m_out_period; /*!< @brief Period to read from output port. */

public:
    /**
     * @name Constructor Function
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetCfgPort()
        : m_node( -1 )
        , m_port( -1 )
        , m_ni( false )
        , m_buf()
        , m_out_period( 1.0 )
    {}
    /**
     * @brief Constructs an item with specified value.
     * @param node        Router/NI id.
     * @param port        Port id.
     * @param ni          The component is NI.
     * @param buf         Virtual channel and buffer size.
     * @param out_period  Period to read from output port.
     */
    EsyNetCfgPort( long node,
                   long port,
                   bool ni,
                   const std::vector< long >& buf,
                   double out_period )
        : m_node( node )
        , m_port( port )
        , m_ni( ni )
        , m_buf( buf )
        , m_out_period( out_period )
    {}
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetCfgPort( const EsyNetCfgPort & t )
        : m_node( t.m_node )
        , m_port( t.m_port )
        , m_ni( t.m_ni )
        , m_buf( t.m_buf )
        , m_out_period( t.m_out_period )
    {}
    /**
     * @}
     */

    /**
     * @name Functions to access variables
     * @{
     */
    /**
     * @brief Return node/NI id.
     */
    long node() const { return m_node; }
    /**
     * @brief Return port id.
     */
    long port() const { return m_port; }
    /**
     * @brief Return true if the node is NI.
     */
    bool isNi() const { return m_ni; }
    /**
     * @brief Return the number of virtual channel.
     */
    std::size_t vc() const { return m_buf.size(); }
    /**
     * @brief Return the vector of buffer size for each virtual channel.
     */
    const std::vector< long >& bufs() const { return m_buf; }
    /**
     * @brief Return the buffer size of one virtual channel.
     */
    long buf( std::size_t i ) const { return m_buf[ i ]; }
    /**
     * @brief Return clock period at output.
     */
    double outPeriod() const { return m_out_period; }
    /**
     * @}
     */
};

#endif
