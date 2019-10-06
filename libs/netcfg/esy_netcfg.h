/*
 * File name : esy_netcfg.h
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

#ifndef LIBS_NETCFG_H_
#define LIBS_NETCFG_H_

#include <vector>
#include <iostream>
#include "esy_netcfg_link.h"

/**
 * @ingroup networkcfggroup
 * @brief Structure of network configuration item.
 */
class EsyNetCfg
{
public:
    /**
     * @brief Network topology.
     */
    enum NoCTopology
    {
        NT_SWITCH,      /*!< @brief Single router */
        NT_RING,        /*!< @brief Ring. */
        NT_MESH_2D,     /*!< @brief 2D-Mesh. */
        NT_TORUS_2D,    /*!< @brief 2D-Torus */
        NT_MESH_DIA,    /*!< @brief Multiple diamension mesh. */
        NT_TORUS_DIA,   /*!< @brief Multiple diamension torus. */
        NT_IRREGULAR,   /*!< @brief Irregular, all customed */
        NT_COUNT        /*!< @brief Count of topolgies */
    };

protected:
    NoCTopology m_topology;     /*!< @brief Network topology. */
    std::vector< long > m_size; /*!< @brief Size of network in dimensions. */

    long m_router_num;  /*!< @brief Number of routers. */
    long m_ni_num;      /*!< @brief Number of nis. */

    std::vector< EsyNetCfgLink > m_links;   /*!< @brief Vector of configuration 
        structure of links. */

public:
    /**
     * @name Constructor Functions
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetCfg()
        : m_topology( NT_SWITCH )
        , m_size( 1, 0 )
        , m_router_num( 0 )
        , m_ni_num( 0 )
        , m_links( 0, EsyNetCfgLink() )
    {}
    /**
     * @brief Constructs an regular network configuration.
     * @param topology Network topology.
     * @param axis_size Network size in diamensions.
     * @param temp_links Template links from one router.
     */
    EsyNetCfg( NoCTopology topology,
               const std::vector< long >& axis_size,
               const std::vector< EsyNetCfgLink >& temp_links );
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetCfg( const EsyNetCfg & t )
        : m_topology( t.m_topology )
        , m_size( t.m_size )
        , m_router_num( t.m_router_num )
        , m_ni_num( t.m_ni_num )
        , m_links( t.m_links )
    {}
    /**
     * @}
     */

    /**
     * @brief Overload operator << function. Print NI configuration to output stream.
     * @param os output stream.
     * @param net_cfg Network configuration structure.
     * @return output stream.
     */
    friend std::ostream& operator<<( std::ostream & os, const EsyNetCfg & net_cfg );

    /**
     * @name Functions to access variables
     * @{
     */
    /**
     * @brief Return network topology.
     */
    NoCTopology topology() const { return m_topology; }
    /**
     * @brief Return network size in diamension.
     */
    const std::vector< long > & size() const { return m_size; }
    /**
     * @brief Return the number of router.
     */
    long routerCount() const { return m_router_num; }
    /**
     * @brief Return the number of NI.
     */
    long niCount() const { return m_ni_num; }
    /**
     * @brief Return all link configurations.
     */
    const std::vector< EsyNetCfgLink > & links() const { return m_links; }
    /**
     * @}
     */

    /**
     * @name Function to convert coordinary and sequence
     * @{
     */
    /**
     * @brief Convert router coordinary to router id based on topology.
     * @param coord  router coordinary.
     * @return  router id.
     */
    long coord2Seq( const std::vector< long > & coord ) const;
    /**
     * @brief Convert router id to router coordinary based on topology.
     * @param seq  router id.
     * @return  router coordinary.
     */
    std::vector< long > seq2Coord( long seq ) const;
    /**
     * @brief Calculate the distance between two positions.
     * @param p1  position 1.
     * @param p2  position 2.
     * @return  Mahanten distance between positions.
     */
    int coordDistance( long p1, long p2 ) const;
    /**
     * @}
     */
};

#endif
