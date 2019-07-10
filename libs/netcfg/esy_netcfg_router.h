/*
 * File name : esy_netcfg_router.h
 * Function : Declare network configuration structure for routers.
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

#ifndef INTERFACE_NETCFG_ROUTER_H
#define INTERFACE_NETCFG_ROUTER_H

#include <vector>

#include "esy_netcfg_port.h"

/**
 * @ingroup networkcfggroup
 * @brief Structure of router configuration item.
 */
class EsyNetworkCfgRouter
{
protected:
    long m_id;                                  /*!< @brief Router id. */
    double m_pipe_cycle;                        /*!< @brief Pipeline frequency of one router, used by frequency scaling, unit by cycle. */
    std::pair< double, double > m_pos;          /*!< @brief Position of router in 2D graphics scene, used by GUI. */
    std::vector< EsyNetworkCfgPort > m_port;    /*!< @brief Vector of port configuration items */

public:
    /**
     * @name Constructor Functions
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetworkCfgRouter()
        : m_id( 0 )
        , m_pipe_cycle( 1.0 )
        , m_pos( 0.0, 0.0 )
        , m_port( 0, EsyNetworkCfgPort() )
    {}
    /**
     * @brief Constructs an item by specified value for arguments.
     */
    EsyNetworkCfgRouter( long id, double pipe_cycle, std::pair< double, double > pos, const std::vector< EsyNetworkCfgPort >& port )
        : m_id( id )
        , m_pipe_cycle( pipe_cycle )
        , m_pos( pos )
        , m_port( port )
    {}
    /**
     * @brief Constructs an item by specified value for some of arguments.
     */
    EsyNetworkCfgRouter( double pipe_cycle, long phy_port, long input_vc_num, long output_vc_num, long input_buffer, long output_buffer )
        : m_id( 0 )
        , m_pipe_cycle( pipe_cycle )
        , m_pos( 0.0, 0.0 )
        , m_port( phy_port, EsyNetworkCfgPort( input_vc_num, output_vc_num, input_buffer, output_buffer ) )
    {}
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetworkCfgRouter( const EsyNetworkCfgRouter & t )
        : m_id( t.m_id )
        , m_pipe_cycle( t.m_pipe_cycle )
        , m_pos( t.m_pos )
        , m_port( t.m_port )
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
    inline bool operator ==( const EsyNetworkCfgRouter & t ) const
    {
        if ( m_port == t.port() && m_pipe_cycle == t.pipeCycle() )
        {
            return true;
        }
        return false;
    }
    /**
     * @brief Overload operator << function. Print NI configuration to output stream.
     * @param os output stream.
     * @param router_cfg Router configuration structure.
     * @return output stream.
     */
    friend std::ostream& operator<<( std::ostream & os, const EsyNetworkCfgRouter & router_cfg );
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
    inline long routerId() const { return m_id; }
    /**
     * @brief Return position of router in 2D graphics scene.
     */
    inline const std::pair< double, double > & pos() const { return m_pos; }
    /**
     * @brief Return the number of ports in this router.
     */
    inline long portNum() const { return (long)m_port.size(); }
    /**
     * @brief Return the configuration of all ports.
     */
    inline const std::vector< EsyNetworkCfgPort > & port() const { return m_port; }
    /**
     * @brief Return the configuration of specified port.
     */
    inline const EsyNetworkCfgPort & port( long i ) const { return m_port[ i ]; }
    /**
     * @brief Return the configuration of specified port.
     */
    inline EsyNetworkCfgPort & port( long i ) { return m_port[ i ]; }
    /**
     * @brief Return the pipeline frequency of this router.
     */
    inline double pipeCycle() const { return m_pipe_cycle; }
    /**
     * @}
     */

    /**
     * @name Functions to set variables
     * @{
     */
    /**
     * @brief Set router id.
     */
    inline void setRouterId( long id ) { m_id = id; }
    /**
     * @brief Set position of router in 2D graphics scene.
     */
    inline void setPos( double first, double second ) { m_pos.first = first; m_pos.second = second; }
    /**
     * @brief Set position of router on first dimension in GUI.
     */
    inline void setPosFirst( double pos ) { m_pos.first = pos; }
    /**
     * @brief Set position of router on second dimension in GUI.
     */
    inline void setPosSecond( double pos ) { m_pos.second = pos; }
    /**
     * @brief Set the number of ports in this router.
     */
    inline void resizePort( long num ) { m_port.resize( num ); }
    /**
     * @brief Set the configuration of all ports.
     */
    inline void setPort( const vector< EsyNetworkCfgPort > & port ) { m_port = port; }
    /**
     * @brief Set the configuration of one port.
     */
    inline void appendPort( const EsyNetworkCfgPort & port ) { m_port.push_back( port ); }
    /**
     * @brief Set pipeline frequency of this router.
     */
    inline void setPipeCycle( double pipe ) { m_pipe_cycle = pipe; }
    /**
     * @}
     */

    /**
     * @name Maximum and total number
     * @{
     */
    /**
     * @brief Return the maximum number of input virtual channels for all ports.
     */
    long maxInputVcNum() const;
    /**
     * @brief Return the maximum number of output virtual channels for all ports.
     */
    long maxOutputVcNum() const;
    /**
     * @brief Return the maximum size of input buffer for all ports.
     */
    long maxInputBuffer() const;
    /**
     * @brief Return the maximum size of output buffer for all ports.
     */
    long maxOutputBuffer() const;
    /**
     * @brief Return the total number of input virtual channels for all ports.
     */
    long totalInputVc() const;
    /**
     * @brief Return the total number of output virtual channels for all ports.
     */
    long totalOutputVc() const;
    /**
     * @}
     */

    /**
     * @brief Return the name of port.
     *
     * Format: \<port direction\>.\<port id\>. e.g. L.0, N.1, S.2, E.3, W.4. etc.
     *
     * @param i port id.
     * @return  name of port.
     */
    std::string portName( long i ) const;

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
