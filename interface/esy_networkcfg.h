/*
 Copyright @ UESTC

 ESY-series Many-core System-on-chip Simulation Environment is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.

 ESY-series Many-core System-on-chip Simulation Environment is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/.
 */


#ifndef INTERFACE_ROUTER_CFG_H
#define INTERFACE_ROUTER_CFG_H

#include "esy_xmlerror.h"
#include "tinyxml.h"

/** @defgroup networkcfggroup Network Configuration Interface. */

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
    enum RouterPortDirection
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
    enum RouterPortAxisDirection
    {
        AXDIR_INC,  /*!< @brief Increase id on axis. */
        AXDIR_DEC,  /*!< @brief Decrease id on axis. */
        AXDIR_NUM,  /*!< @brief Count of axis direction. */
    };

protected:
    static std::string const_port_direction[ ROUTER_PORT_NUM ];         /*!< @brief Full name of port direction. */
    static std::string const_port_direction_short[ ROUTER_PORT_NUM ];   /*!< @brief Short name of port direction. */

    long m_input_vc_num;            /*!< @brief Number of input virtual channels. */
    long m_output_vc_num;           /*!< @brief Number of output virtual channels. */
    RouterPortDirection m_port_dir;             /*!< @brief Direction of port in GUI. */
    long m_port_axis;                           /*!< @brief Axis of the port. */
    RouterPortAxisDirection m_port_axis_dir;    /*!< @brief Direction of port on axis. */
    long m_neighbor_router;     /*!< @brief Id of connected router in output direction. */
    long m_neighbor_port;       /*!< @brief Id of connected neighbor port in output direction. */
    long m_input_buffer;        /*!< @brief Size of input buffer. */
    long m_output_buffer;       /*!< @brief Size of output buffer. */
    bool m_network_interface;   /*!< @brief Port connects network interface or not. */

public:
    /**
     * @name Constructor Function
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetworkCfgPort();
    /**
     * @brief Constructs an item with specified value for fields.
     * @param input_vc_num  Number of input virtual channel.
     * @param output_vc_num Number of output virtual channel.
     * @param dir           Port direction in GUI.
     * @param axis          Axis of the port.
     * @param axis_dir      Direction of port on axis.
     * @param input_buffer  Size of input buffer.
     * @param output_buffer Size of output buffer.
     * @param ni            Network interface flag of connection.
     */
    EsyNetworkCfgPort( int input_vc_num, int output_vc_num
        , RouterPortDirection dir, long axis, RouterPortAxisDirection axis_dir
        , int input_buffer, int output_buffer, bool ni );
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetworkCfgPort( const EsyNetworkCfgPort & t );
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
    inline RouterPortDirection portDirection() const { return m_port_dir; }
    /**
     * @brief Return the short name of port direction of this port.
     */
    inline const string portDirectionStrShort() const { return const_port_direction_short[ m_port_dir ]; }
    /**
     * @brief Return the full name of port direction of this port.
     */
    inline const string portDirectionStr() const { return const_port_direction[ m_port_dir ]; }
    /**
     * @brief Return the axis of this port.
     */
    inline long portAxis() const { return m_port_axis; }
    /**
     * @brief Return the direction on axis.
     */
    inline RouterPortAxisDirection portAxisDir() const { return m_port_axis_dir; }
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
    inline void setPortDirection( RouterPortDirection dir ) { m_port_dir = dir; }
    /**
     * @brief Set axis of this port.
     */
    inline void setPortAxis( long axis ) { m_port_axis = axis; }
    /**
     * @brief Set direction on axis.
     */
    inline void setPortAxisDir( RouterPortAxisDirection dir ) { m_port_axis_dir = dir; }
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
    static string portDirectionStrVector( int i ) { return const_port_direction[ i ]; }

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
     *
     * If full is true, print out all fields. Otherwise, connected router and port are not printed.
     *
     * @param full  Full configuraion flag.
     * @param root  root of XML structure.
     */
    void writeXml( bool full, TiXmlElement * root );
    /**
     * @}
     */

    /**
     * @name Maximum and total value
     * @{
     */
    /**
     * @brief Return maximum virtual channel. Higher of input and output virtual channel.
     */
    inline long maxVcNumber() const { return (m_input_vc_num > m_output_vc_num)? m_input_vc_num : m_output_vc_num; }
    /**
     * @brief Return total virtual channel. Sum of input and output virtual channel.
     */
    inline long totalVcNumber() const { return m_input_vc_num + m_output_vc_num; }
    /**
     * @}
     */

    /**
     * @brief Overload operator == function. Compare value of two items.
     * @param t  item to compare.
     * @return If t has the same value as this item at all field, return TRUE.
     * Otherwise, return FALSE.
     */
    bool operator ==(const EsyNetworkCfgPort & t) const;
};

/**
 * @ingroup networkcfggroup
 * @brief Structure of router configuration item.
 */
class EsyNetworkCfgRouter
{
protected:
    long m_id;                                  /*!< @brief Router id. */
    std::pair< double, double > m_pos;          /*!< @brief Position of router in 2D graphics scene, used by GUI. */
    std::vector< EsyNetworkCfgPort > m_port;    /*!< @brief Vector of port configuration items */
    double m_pipe_cycle;                        /*!< @brief Pipeline frequency of one router, used by frequency scaling, unit by cycle. */

public:
    /**
     * @name Constructor Functions
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetworkCfgRouter();
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetworkCfgRouter(const EsyNetworkCfgRouter & t);
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
     * @brief Return the maximum number of virtual channels for all ports.
     */
    long maxVcNum() const;
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

    /**
     * @brief Overload operator == function. Compare value of two items.
     * @param t  item to compare.
     * @return If t has the same value as this item at all field, return TRUE. 
     */
    bool operator ==( const EsyNetworkCfgRouter & t ) const;
};

/**
 * @ingroup networkcfggroup
 * @brief Structure of network configuration item.
 */
class EsyNetworkCfg
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
        AX_NUM      /*!< @brief count of axis */
    };

    #define NETCFG_EXTENSION "netcfg"

protected:
    static std::string const_noc_topology[ NT_COUNT ];  /*!< @brief String for NoC topology. */
    static std::string const_axis_dir[ AX_NUM ];        /*!< @brief String for axis. */

    NoCTopology m_topology;     /*!< @brief Network topology. */
    std::vector< long > m_size; /*!< @brief Size of network in dimensions. */
    long m_router_count;        /*!< @brief Number of router. */
    long m_data_width;          /*!< @brief Width of data path. */

    EsyNetworkCfgRouter m_template_router;  /*!< @brief Template router. */
    std::vector< EsyNetworkCfgRouter > m_router;    /*!< @brief Vector of router configuration item. */

public:
    /**
     * @name Constructor Functions
     * @{
     */
    /**
     * @brief Constructs an empty item with default value.
     */
    EsyNetworkCfg();
    /**
     * @brief Constructs an regular network configuration.
     * @param topology Network topology.
     * @param size Network size in diamensions.
     * @param data_width Width of data path.
     * @param pipe_cycle Pipeline cycle.
     * @param phy_port Number of physical channel for each router.
     * @param input_vc_num Number of input virtual channel for each physical channel.
     * @param output_vc_num Number of output virtual channel for each physical channel.
     * @param input_buffer Size of input buffer.
     * @param output_buffer Size of output buffer.
     */
    EsyNetworkCfg( NoCTopology topology, const std::vector< long >& size, long data_width
        , double pipe_cycle, long phy_port
        , long input_vc_num, long output_vc_num, long input_buffer, long output_buffer );
    /**
     * @brief Constructs an item by copying an exist item.
     * @param t  entity to copy.
     */
    EsyNetworkCfg( const EsyNetworkCfg & t );
    /**
     * @}
     */

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
    inline  const string topologyStr() const { return const_noc_topology[ m_topology ]; }
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
    inline long routerCount() const { return (long)m_router_count; }
    /**
     * @brief Return flit size in 64-bit.
     */
    inline long flitSize( long atom_width ) const { return ( m_data_width / atom_width ) + ( ( m_data_width % atom_width > 0 ) ? 1 : 0 ); }
    /**
     * @brief Return width of data path.
     */
    inline long dataPathWidth() const { return m_data_width; }
    /**
     * @brief Return template router configuraion.
     */
    inline const EsyNetworkCfgRouter & templateRouter() const { return m_template_router; }
    /**
     * @brief Return template router configuraion.
     */
    inline EsyNetworkCfgRouter & templateRouter() { return m_template_router; }
    /**
     * @brief Return all router configurations.
     */
    inline const vector< EsyNetworkCfgRouter > & router() const { return m_router; }
    /**
     * @brief Return configuration of specified router.
     */
    inline const EsyNetworkCfgRouter & router( int id ) const { return m_router[ id ]; }
    /**
     * @brief Return configuration of specified router.
     */
    inline EsyNetworkCfgRouter & router( int id ) { return m_router[ id ]; }
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
    inline void setTopology( NoCTopology topology );
    /**
     * @brief Set network size in diamension.
     */
    inline void setSize( const std::vector< long > & size ) { m_size = size; m_router_count = routerCount(); }
    /**
     * @brief Set network size in specified diamension.
     */
    inline void setSize( long dim, long size ) { m_size[ dim ] = size; m_router_count = routerCount(); }
    /**
     * @brief Set dimension of network size.
     */
    inline void resizeDim( long dim ) { m_size.resize( dim ); m_router_count = routerCount(); }
    /**
     * @brief Set width of data path.
     */
    inline void setDataPathWidth( long size ) { m_data_width = size; }
    /**
     * @brief Set template router configuration.
     */
    inline void setTemplateRouter( const EsyNetworkCfgRouter & t ) { m_template_router = t; }
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
    static string nocTopologyStrVector( int i ) { return const_noc_topology[ i ]; }
    /**
     * @brief Return name string of axis.
     */
    static string nocAxisStrVector( int i ) { return const_axis_dir[ i ]; }
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
     * @brief Return the maximum number of virtual channels for all ports of all routers.
     */
    long maxVcNum() const;
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
    EsyXmlError readXml( const string & fname );
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
    EsyXmlError writeXml( const string & fname );
    /**
     * @}
     */

    /**
     * @name Function to convert coordinary and sequence
     * @{
     */
    /**
     * @brief Calculate the number of routers in network.
     * @return  number of routers
     */
    int setRouterCount() const;
    /**
     * @brief Calculate the number of NI in network.
     * @return  number of NI
     */
    int setNiCount() const;
    /**
     * @brief Convert router coordinary to router id based on topology.
     * @param coord  router coordinary.
     * @return  router id.
     */
    long coord2Seq( const std::vector< long > & coord );
    /**
     * @brief Convert router id to router coordinary based on topology.
     * @param seq  router id.
     * @return  router coordinary.
     */
    std::vector< long > seq2Coord( long seq );
    /**
     * @brief Calculate the distance between two positions.
     * @param p1  position 1.
     * @param p2  position 2.
     * @return  Mahanten distance between positions.
     */
    int coordDistance( long p1, long p2 );
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
     * @brief Get the configuration of specified router based on general configurations.
     * @param id id of specified router.
     * @return  router configuration item.
     */
    EsyNetworkCfgRouter defaultRouterCfg( long id );
    /**
     * @brief Generate configuration of each routers based on general configurations.
     */
    void updateRouter();

};

#endif
