/*
 * File name : esy_netcfg_link.h
 * Function : Declare network configuration structure.
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

#ifndef LIBS_NETCFG_LINK_H_
#define LIBS_NETCFG_LINK_H_

#include "esy_netcfgport.h"

/**
 * @ingroup networkcfggroup
 * @brief Configuration structure of one link.
 *
 * In Esynet, the link presents the output buffer in previous-stage node, 
 * the wire connects two nodes, and the input buffer in next-stage node.
 */
class EsyNetCfgLink
{
public:
    /**
     * @brief Link direction in GUI.
     *
     * In GUI, a node has eight edges and one port can be placed at one of them.
     */
    enum LinkDir
    {
        LINKDIR_N_S,    /*!< @brief North to South */
        LINKDIR_S_N,    /*!< @brief South to North */
        LINKDIR_W_E,    /*!< @brief West to East */
        LINKDIR_E_W,    /*!< @brief East toWest */
        LINKDIR_NW_SE,  /*!< @brief North-West to South-East */
        LINKDIR_NE_SW,  /*!< @brief North-East to South-West */
        LINKDIR_SW_NE,  /*!< @brief South-West to North-East */
        LINKDIR_SE_NW,  /*!< @brief South-East to North-West */
        LINKDIR_NUM     /*!< @brief Count of directions */
    };

    /**
     * @brief Port direction in axis.
     */
    enum LinkAxisDir
    {
        LINKAXISDIR_UP,     /*!< @brief Increase id on axis. */
        LINKAXISDIR_DOWN,   /*!< @brief Decrease id on axis. */
        LINKAXISDIR_NUM,    /*!< @brief Count of axis direction. */
    };

private:
    LinkDir m_link_dir;             /*!< @brief Link direction. */
    long m_link_axis;               /*!< @brief Link axis. */
    LinkAxisDir m_link_axis_dir;    /*!< @brief Link axis direction. */

    EsyNetCfgPort m_output_port;    /*!< @brief Output port in previous-stage
        node. */
    EsyNetCfgPort m_input_port;     /*!< @brief Input port in next-stage
        node. */

    double m_ni_int_delay;  /*!< @brief Delay to response NI interrupt. */

public:
    /**
     * @name Constructor functions.
     * @{
     */
    /**
     * @brief Constructs one empty link.
     */
    EsyNetCfgLink()
        : m_link_dir( LINKDIR_NUM )
        , m_link_axis( -1 )
        , m_link_axis_dir( LINKAXISDIR_NUM )
        , m_output_port()
        , m_input_port()
        , m_ni_int_delay( 100.0 )
    {}
    /**
     * @brief Construct one template link.
     *
     * Template link does not specify the source and the destination.
     *
     * @param out_buf       Buffer size of output virtual channels.
     * @param out_delay     Clock period at the output port.
     * @param in_buf        Buffer size of input virtual channels.
     * @param in_delay      Clock period at the output port.
     * @param ni_int_delay  Delay to response NI interrupt.
     */
    EsyNetCfgLink( const std::vector< long > & out_buf,
                   double out_delay,
                   const std::vector< long > & in_buf,
                   double in_delay,
                   double ni_int_delay )
        : m_link_dir( LINKDIR_NUM )
        , m_link_axis( -1 )
        , m_link_axis_dir( LINKAXISDIR_NUM )
        , m_output_port( -1, -1, false, out_buf, out_delay )
        , m_input_port( -1, -1, false, in_buf, in_delay )
        , m_ni_int_delay( ni_int_delay )
    {}
    /**
     * @brief Construct one link.
     * @param link_dir       Direction on GUI.
     * @param link_axis      Axis of the link.
     * @param link_axis_dir  Direction on the axis.
     * @param out_port       Output port/Source port.
     * @param in_port        Input port/Destination port.
     * @param ni_int_delay   Delay to response NI interrupt.
     */
    EsyNetCfgLink( LinkDir link_dir,
                   long link_axis,
                   LinkAxisDir link_axis_dir,
                   const EsyNetCfgPort& out_port,
                   const EsyNetCfgPort& in_port,
                   double ni_int_delay )
        : m_link_dir( link_dir )
        , m_link_axis( link_axis )
        , m_link_axis_dir( link_axis_dir )
        , m_output_port( out_port )
        , m_input_port( in_port )
        , m_ni_int_delay( ni_int_delay )
    {}
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetCfgLink( const EsyNetCfgLink & t )
        : m_link_dir( t.m_link_dir )
        , m_link_axis( t.m_link_axis )
        , m_link_axis_dir( t.m_link_axis_dir )
        , m_output_port( t.m_output_port )
        , m_input_port( t.m_input_port )
        , m_ni_int_delay( t.m_ni_int_delay )
    {}
    /**
     * @}
     */

    /**
     * @name Functions to access variables
     * @{
     */
    /**
     * @brief Return link direction in GUI.
     */
    LinkDir linkDir() const { return m_link_dir; }
    /**
     * @brief Return link axis.
     */
    long linkAxis() const { return m_link_axis; }
    /**
     * @brief Return direction on axis.
     */
    LinkAxisDir linkAxisDir() const { return m_link_axis_dir; }
    /**
     * @brief Return output port/source port.
     */
    const EsyNetCfgPort & outputPort() const { return m_output_port; }
    /**
     * @brief Return input port/destination port.
     */
    const EsyNetCfgPort & inputPort() const { return m_input_port; }

    /**
     * @brief Return delay to response NI interrupt.
     */
    double niIntDelay() const { return m_ni_int_delay; }
    /**
     * @}
     */
};

#endif
