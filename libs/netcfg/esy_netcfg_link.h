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

#include "esy_netcfg_port.h"

/**
 * @ingroup networkcfggroup
 * @brief Configuration structure of link.
 *
 * In Esynet, the link presents the output buffer in previous-stage router, 
 * the wire connects two routers, and the input buffer in next-stage router.
 */
class EsyNetCfgLink
{
public:
    /**
     * @brief Link direction in GUI.
     *
     * In Graphics User Interface, a router has eight edges and one port can be
     * placed at one of them.
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
    LinkAxisDir m_link_axis_dir;    /*!< @brief LInk axis direction. */

    EsyNetCfgPort m_output_port;    /*!< @brief Output port in previous-stage
        router. */
    EsyNetCfgPort m_input_port;     /*!< @brief Input port in next-stage
        router. */

    double m_ni_int_delay;  /*!< @brief Delay to response NI interrupt. */

public:
    /**
     * @name Constructor functions.
     * @{
     */
    /**
     * @brief Construct one empty item.
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
     */
    EsyNetCfgLink( LinkDir link_dir, long link_axis, LinkAxisDir link_axis_dir,
                   const EsyNetCfgPort& out_port, const EsyNetCfgPort& in_port,
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
     * @name Overload operator
     * @{
     */
    /**
     * @brief Overload operator == function. Compare value of two items.
     * @param t  item to compare.
     * @return If t has the same value as this item at all field, return TRUE.
     */
    EsyNetCfgLink& operator =(const EsyNetCfgLink & t)
    {
        m_link_dir = t.m_link_dir;
        m_link_axis_dir = t.m_link_axis_dir;
        m_link_axis = t.m_link_axis;
        m_output_port = t.m_output_port;
        m_input_port = t.m_input_port;
        m_ni_int_delay = t.m_ni_int_delay;

        return *this;
    }
    /**
     * @}
     */

    /**
     * @name Functions to access variables
     * @{
     */
    /**
     * @brief Return link direction.
     */
    LinkDir linkDir() const { return m_link_dir; }
    /**
     * @brief Return link axis.
     */
    long linkAxis() const { return m_link_axis; }
    /**
     * @brief Return link axis direction.
     */
    LinkAxisDir linkAxisDir() const { return m_link_axis_dir; }

    /**
     * @brief Output port.
     */
    const EsyNetCfgPort & outputPort() const { return m_output_port; }
    /**
     * @brief Input port.
     */
    const EsyNetCfgPort & inputPort() const { return m_input_port; }

    /**
     * @brief Return interrupt delay.
     */
    double niIntDelay() const { return m_ni_int_delay; }
    /**
     * @}
     */
};

#endif
