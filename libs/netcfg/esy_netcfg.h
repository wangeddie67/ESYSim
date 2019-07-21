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

#ifndef INTERFACE_NETCFG_H
#define INTERFACE_NETCFG_H

#include "esy_xmlerror.h"
#include "esy_netcfg_router.h"
#include "esy_netcfg_ni.h"

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

    /*!
     * @brief Axis direction for 2D topology.
     */
    enum AXIS_DIR
    {
        AX_X = 0,   /*!< @brief X, row */
        AX_Y = 1,   /*!< @brief Y, column */
        AX_Z = 2,   /*!< @brief Y, column */
        AX_NUM      /*!< @brief count of axis */
    };

    #define NETCFG_EXTENSION "netcfg"

protected:
    static std::string const_noc_topology[ NT_COUNT ];  /*!< @brief String for NoC topology. */
    static std::string const_axis_dir[ AX_NUM ];        /*!< @brief String for axis. */

    NoCTopology m_topology;     /*!< @brief Network topology. */
    std::vector< long > m_size; /*!< @brief Size of network in dimensions. */

    EsyNetCfgRouter m_template_router;          /*!< @brief Template router. */
    EsyNetCfgNI m_template_ni;                  /*!< @brief Template network interface. */
    std::vector< EsyNetCfgRouter > m_router;    /*!< @brief Vector of router configuration item. */
    std::vector< EsyNetCfgNI > m_ni;            /*!< @brief Vector of NI configuration item. */

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
        , m_size( 1, 1 )
        , m_template_router()
        , m_template_ni()
        , m_router( 1, EsyNetCfgRouter() )
        , m_ni( 0, EsyNetCfgNI() )
    {}
    /**
     * @brief Constructs an regular network configuration.
     * @param topology Network topology.
     * @param size Network size in diamensions.
     * @param pipe_cycle Pipeline cycle.
     * @param phy_port Number of physical channel for each router.
     * @param input_vc_num Number of input virtual channel for each physical channel.
     * @param output_vc_num Number of output virtual channel for each physical channel.
     * @param input_buffer Size of input buffer.
     * @param output_buffer Size of output buffer.
     * @param ni_pipe_cycle Pipeline cycle of NI.
     * @param ni_buffer Size of eject buffer in NI.
     * @param ni_interrupt_delay Delay to response NI interrupt.
     */
    EsyNetCfg( NoCTopology topology, const std::vector< long >& size
        , double pipe_cycle, long phy_port, long input_vc_num, long output_vc_num, long input_buffer, long output_buffer
        , double ni_pipe_cycle, long ni_buffer, double ni_interrupt_delay
                 );
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetCfg( const EsyNetCfg & t );
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
    inline NoCTopology topology() const { return m_topology; }
    /**
     * @brief Return the string of network topology.
     */
    inline const std::string topologyStr() const { return const_noc_topology[ m_topology ]; }
    /**
     * @brief Return network size in diamension.
     */
    inline const std::vector< long > & size() const { return m_size; }
    /**
     * @brief Return network size in specified diamension.
     */
    inline long size( long dim ) const { return m_size[ dim ]; }
    /**
     * @brief Return dimension of network size.
     */
    inline long dim() const { return (long)m_size.size(); }
    /**
     * @brief Return the number of router.
     */
    inline long routerCount() const { return (long)m_router.size(); }
    /**
     * @brief Return the number of NI.
     */
    inline long niCount() const { return (long)m_ni.size(); }
    /**
     * @brief Return template router configuraion.
     */
    inline const EsyNetCfgRouter & templateRouter() const { return m_template_router; }
    /**
     * @brief Return template router configuraion.
     */
    inline EsyNetCfgRouter & templateRouter() { return m_template_router; }
    /**
     * @brief Return template NI configuraion.
     */
    inline const EsyNetCfgNI & templateNI() const { return m_template_ni; }
    /**
     * @brief Return template NI configuraion.
     */
    inline EsyNetCfgNI & templateNI() { return m_template_ni; }
    /**
     * @brief Return all router configurations.
     */
    inline const std::vector< EsyNetCfgRouter > & router() const { return m_router; }
    /**
     * @brief Return configuration of specified router.
     */
    inline const EsyNetCfgRouter & router( int id ) const { return m_router[ id ]; }
    /**
     * @brief Return configuration of specified router.
     */
    inline EsyNetCfgRouter & router( int id ) { return m_router[ id ]; }
    /**
     * @brief Return all router configurations.
     */
    inline const std::vector< EsyNetCfgNI > & ni() const { return m_ni; }
    /**
     * @brief Return configuration of specified router.
     */
    inline const EsyNetCfgNI & ni( int id ) const { return m_ni[ id ]; }
    /**
     * @brief Return configuration of specified router.
     */
    inline EsyNetCfgNI & ni( int id ) { return m_ni[ id ]; }
    /**
     * @}
     */

    /**
     * @name Functions to set variables
     * @{
     */
    /**
     * @brief Set network topology and resize diamension.
     */
    void setTopology( NoCTopology topology );
    /**
     * @brief Set network size in diamension.
     */
    inline void setSize( const std::vector< long > & size ) { m_size = size; }
    /**
     * @brief Set network size in specified diamension.
     */
    inline void setSize( long dim, long size ) { m_size[ dim ] = size; }
    /**
     * @brief Set dimension of network size.
     */
    inline void resizeDim( long dim ) { m_size.resize( dim ); }
    /**
     * @brief Set template router configuration.
     */
    inline void setTemplateRouter( const EsyNetCfgRouter & t ) { m_template_router = t; }
    /**
     * @brief Set template NI configuration.
     */
    inline void setTemplateNI( const EsyNetCfgNI & t ) { m_template_ni = t; }
    /**
     * @}
     */

    /**
     * @name Functions to access constant string
     * @{
     */
    /**
     * @brief Return name string of topology.
     */
    static std::string nocTopologyStrVector( int i ) { return const_noc_topology[ i ]; }
    /**
     * @brief Return name string of axis.
     */
    static std::string nocAxisStrVector( int i ) { return const_axis_dir[ i ]; }
    /**
     * @}
     */

    /**
     * @name Maximum and total number
     * @{
     */
    /**
     * @brief Return the maximum number of physical channels for all routers.
     */
    long maxPcNum() const;
    /**
     * @brief Return the maximum number of input virtual channels for all ports of all routers.
     */
    long maxInputVcNum() const;
    /**
     * @brief Return the maximum number of output virtual channels for all ports of all routers.
     */
    long maxOutputVcNum() const;
    /**
     * @brief Return the maximum size of input buffer for all ports of all routers.
     */
    long maxInputBuffer() const;
    /**
     * @brief Return the maximum number of output buffer for all ports of all routers.
     */
    long maxOutputBuffer() const;
    /**
     * @}
     */

    /**
     * @name Functions to access network configuration file in XML
     * @{
     */
    /**
     * @brief Read network configuration from XML file.
     * @param root  root of XML structure.
     */
    void readXml( TiXmlElement * root );
    /**
     * @brief Open XML network configuration file for reading.
     * @param fname  direction of network configuration file.
     * @return  XML error handler.
     */
    EsyXmlError readXml( const std::string & fname );
    /**
     * @brief Write network configuration to XML file.
     * @param root  root of XML structure.
     */
    void writeXml( TiXmlElement * root );
    /**
     * @brief Open XML network configuration file for writing.
     * @param fname  direction of network configuration file.
     * @return  XML error handler.
     */
    EsyXmlError writeXml( const std::string & fname );
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
     * @brief Check the router id is valid or not.
     * @param id router id to check.
     * @return  true, if the router id is valid.
     */
    bool validRouterId( long id ) const { return ( ( id >= 0 ) && ( id < routerCount() ) ); }
    /**
     * @}
     */

    /**
     * @brief Generate configuration of routers and NIs based on general configurations and topology.
     */
    void updateNetwork();
    /**
     * @brief Generate configuration of NIs based on general configurations and topology.
     */
    void updateNI();

};

#endif
