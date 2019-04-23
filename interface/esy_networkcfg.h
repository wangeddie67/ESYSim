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

using namespace std;

/** @defgroup networkcfggroup Network Configuration Interface. */

/**
 * @ingroup networkcfggroup
 * @brief Structure of port configuration item.
 *
 * Holds parameters and value of one port. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Default </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> char #m_vc_num </td>
 *   <td> Number of virtual channels. </td>
 *   <td> DEFAULT_VC_NUM = 1 </td>
 *   <td> #vcNumber(), #setVcNumber() </td>
 * </tr>
 * <tr>
 *   <td> RouterPortDirection #m_port_dir </td>
 *   <td> Direction of port in GUI. </td>
 *   <td> DEFAULT_PORT_DIRECTION = #ROUTER_PORT_SOUTHEAST </td>
 *   <td> #portDirection(), #setPortDirection() </td>
 * </tr>
 * <tr>
 *   <td> long #m_neighbor_router </td>
 *   <td> Id of connected router in output direction. </td>
 *   <td> -1, means no connection </td>
 *   <td> #neighborRouter(), #setNeighborRouter() </td>
 * </tr>
 * <tr>
 *   <td> long #m_neighbor_port </td>
 *   <td> Id of connected neighbor port in output direction. </td>
 *   <td> -1, means no connection </td>
 *   <td> #neighborPort(), #setNeighborPort() </td>
 * </tr>
 * <tr>
 *   <td> long #m_input_buffer </td>
 *   <td> Size of input buffer. </td>
 *   <td> DEFAULT_INPUT_BUFFER = 12 </td>
 *   <td> #inputBuffer(), #setInputBuffer() </td>
 * </tr>
 * <tr>
 *   <td> long #m_output_buffer </td>
 *   <td> Size of output buffer. </td>
 *   <td> DEFAULT_OUTPUT_BUFFER = 12 </td>
 *   <td> #outputBuffer(), #setOutputBuffer() </td>
 * </tr>
 * <tr>
 *   <td> bool #m_network_interface </td>
 *   <td> Port connects network interface or not. </td>
 *   <td> DEFAULT_NETWORK_INTERFACE = false </td>
 *   <td> #networkInterface(), #setNetworkInterface() </td>
 * </tr>
 * </table>
 *
 * In specified router configuration, all field is emplied.
 * In general network configuration, connected router and port
 * (#m_neighbor_router and #m_neighbor_port) are not emplied.
 * These two parameters will be sets according to the toplogy and router id.
 *
 * Parameters and values can be stored in XML file through TinyXML.
 *
 * \sa EsyNetworkCfgRouter, EsyNetworkCfg
 */
class EsyNetworkCfgPort
{
public:
	/*!
	 * \brief Port direction in GUI.
	 *
	 * In Graphics User Interface, a router has eight edges and one port can be
	 * placed at one of them
	 */
	enum RouterPortDirection
	{
		ROUTER_PORT_NORTH,  /**< North edge */
		ROUTER_PORT_SOUTH,  /**< South edge */
		ROUTER_PORT_WEST,   /**< West edge */
		ROUTER_PORT_EAST,   /**< East edge */
		ROUTER_PORT_NORTHWEST,  /**< North-West edge */
		ROUTER_PORT_NORTHEAST,  /**< North-East edge */
		ROUTER_PORT_SOUTHWEST,  /**< South-West edge */
		ROUTER_PORT_SOUTHEAST,  /**< South-East edge */
		ROUTER_PORT_NUM  /**< Count of edges */
	};

protected:
	static string const_port_direction[ ROUTER_PORT_NUM ]; /**< \brief
		Full name of port direction. */
	static string const_port_direction_short[ ROUTER_PORT_NUM ];/**< \brief
		Short name of port direction. */

	long m_input_vc_num;  /**< \brief Number of input virtual channels. */
	long m_output_vc_num;  /**< \breif Number of output virtual channels. */
	RouterPortDirection m_port_dir; /**< \brief Direction of port in GUI. */
	long m_neighbor_router; /**< \brief Id of connected router in output
		direction. */
	long m_neighbor_port;   /**< \brief Id of connected neighbor port in output
		direction. */
	long m_input_buffer;  /**< \brief Size of input buffer. */
	long m_output_buffer; /**< \brief Size of output buffer. */
	bool m_network_interface; /**< \brief Port connects network interface or
		not. */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item with default value.
	 */
	EsyNetworkCfgPort();
	/*!
	 * \brief Constructs an item with specified value for fields.
	 * \param vc_num		Number of virtual channel.
	 * \param dir		   Port direction in GUI.
	 * \param input_buffer  Size of input buffer.
	 * \param output_buffer Size of output buffer.
	 * \param ni			Network interface flag of connection.
	 */
	EsyNetworkCfgPort(int input_vc_num, int output_vc_num,
		RouterPortDirection dir,
		int input_buffer, int output_buffer, bool ni );
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsyNetworkCfgPort(const EsyNetworkCfgPort & t);
	///@}

	/** @name Functions to access variables. */
	///@{
	/*!
	 * \brief Set number of virtual channels #m_vc_num.
	 * \param vc_num  number of virtual channels.
	 */
	void setInputVcNumber( long vc_num ) { m_input_vc_num = vc_num; }
	/*!
	 * \brief Access number of virtual channels #m_vc_num.
	 * \return number of virtual channels #m_vc_num.
	 */
	long inputVcNumber() const { return m_input_vc_num; }
	/*!
	 * \brief Set number of virtual channels #m_vc_num.
	 * \param vc_num  number of virtual channels.
	 */
	void setOutputVcNumber( long vc_num ) { m_output_vc_num = vc_num; }
	/*!
	 * \brief Access number of virtual channels #m_vc_num.
	 * \return number of virtual channels #m_vc_num.
	 */
	long outputVcNumber() const { return m_output_vc_num; }
	/*!
	 * \brief Set direction of port in GUI #m_port_dir.
	 * \param dir  direction of port in GUI.
	 */
	void setPortDirection( RouterPortDirection dir ) { m_port_dir = dir; }
	/*!
	 * \brief Access direction of port in GUI #m_port_dir.
	 * \return  direction of port in GUI #m_port_dir.
	 */
	RouterPortDirection portDirection() const { return m_port_dir; }
	/*!
	 * \brief Access the short name of port direction of this port based on
	 * current #m_port_dir.
	 * \return  the short name of port direction.
	 */
	const string portDirectionStrShort() const
		{ return const_port_direction_short[ m_port_dir ]; }
	/*!
	 * \brief Access the full name of port direction of this port based on
	 * current #m_port_dir.
	 * \return  the full name of port direction.
	 */
	const string portDirectionStr() const
		{ return const_port_direction[ m_port_dir ]; }
	/*!
	 * \brief Set id of connected router in output direction #m_neighbor_router.
	 * \param id  id of connected router in output direction.
	 */
	void setNeighborRouter( long id ) { m_neighbor_router = id; }
	/*!
	 * \brief Access id of connected router in output direction
	 * #m_neighbor_router.
	 * \return id of connected router in output direction #m_neighbor_router.
	 */
	long neighborRouter() const { return m_neighbor_router; }
	/*!
	 * \brief set id of connected neighbor port in output direction
	 * #m_neighbor_port.
	 * \param port id of connected neighbor port in output direction.
	 */
	void setNeighborPort( long port ) { m_neighbor_port = port; }
	/*!
	 * \brief Access id of connected neighbor port in output direction
	 * #m_neighbor_port.
	 * \return id of connected neighbor port in output direction
	 * #m_neighbor_port.
	 */
	long neighborPort() const { return m_neighbor_port; }
	/*!
	 * \brief Set size of input buffer #m_input_buffer.
	 * \param buf  size of input buffer.
	 */
	void setInputBuffer( long buf ) { m_input_buffer = buf; }
	/*!
	 * \brief Access size of input buffer #m_input_buffer.
	 * \return size of input buffer #m_input_buffer.
	 */
	long inputBuffer() const { return m_input_buffer; }
	/*!
	 * \brief Set size of output buffer #m_output_buffer.
	 * \param buf  size of output buffer.
	 */
	void setOutputBuffer( long buf ) { m_output_buffer = buf; }
	/*!
	 * \brief Access size of output buffer #m_output_buffer.
	 * \return size of output buffer #m_output_buffer.
	 */
	long outputBuffer() const { return m_output_buffer; }
	/*!
	 * \brief Set Network interface flag of port connection #m_network_interface.
	 * \param ni network interface flag of port connection.
	 */
	void setNetworkInterface( bool ni ) { m_network_interface = ni; }
	/*!
	 * \brief Access network interface flag of port connection
	 * #m_network_interface.
	 * \return network interface flag of port connection #m_network_interface.
	 */
	bool networkInterface() const { return m_network_interface; }
	///@}

	/*!
	 * \brief Access the full name of specified direction.
	 * \param i  code of direction.
	 * \return  the full name of specified direction.
	 */
	static string portDirectionStrVector( int i )
		{ return const_port_direction[ i ]; }

	/** @name Functions to access network configuration file in XML. */
	/*!
	 * \brief Read port configuration from XML file.
	 * \param root  root of XML structure.
	 */
	void readXml( TiXmlElement * root );
	/*!
	 * \brief Write port configuration to XML file.
	 *
	 * If full is true, print out all fields. Otherwise, #m_neighbor_router and
	 * #m_neighbor_port are not printed.
	 *
	 * \param full  Full configuraion flag.
	 * \param root  root of XML structure.
	 */
	void writeXml( bool full, TiXmlElement * root );
	///@}

	long maxVcNumber() const
		{ return (m_input_vc_num > m_output_vc_num)?
			m_input_vc_num:m_output_vc_num; }
	long totalVcNumber() const { return m_input_vc_num + m_output_vc_num; }

	/*!
	 * \brief Overload operator == function. Compare value of two items.
	 * \param t  item to compare.
	 * \return If t has the same value as this item at all field, return TRUE.
	 * Otherwise, return FALSE.
	 */
	bool operator ==(const EsyNetworkCfgPort & t) const;
};

/**
 * @ingroup networkcfggroup
 * @brief Structure of router configuration item.
 *
 * Holds parameters and value of one router. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Default </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> long #m_id </td>
 *   <td> Router id. </td>
 *   <td> 0 </td>
 *   <td> #routerId(), #setRouterId() </td>
 * </tr>
 * <tr>
 *   <td> pair< double, double > #m_pos </td>
 *   <td> Position of router in 2D graphics scene, used by GUI. </td>
 *   <td> (0.0, 0.0) </td>
 *   <td> #pos(), #setPos(), #setPosFirst(), #setPosSecond() </td>
 * </tr>
 * <tr>
 *   <td> vector< EsyNetworkCfgPort > #m_port </td>
 *   <td> Vector of port configuration items. </td>
 *   <td> DEFAULT_PHY_NUM = 0 </td>
 *   <td> #portNum(), #resizePort(), #port(), #setPort() </td>
 * </tr>
 * <tr>
 *   <td> double #m_pipe_cycle </td>
 *   <td> Pipeline frequency of one router, used by frequency scaling, unit by
 * cycle. </td>
 *   <td> 1.0 </td>
 *   <td> #pipeCycle(), #setPipeCycle() </td>
 * </tr>
 * </table>
 *
 * Parameters and values can be stored in XML file through TinyXML.
 *
 * \sa EsyNetworkCfgPort, EsyNetworkCfg
 */
class EsyNetworkCfgRouter
{
protected:
	long m_id;  /**< \brief Router id. */
	pair< double, double > m_pos; /**< \brief Position of router in 2D graphics
		scene, used by GUI. */
	vector< EsyNetworkCfgPort > m_port; /**< \brief Vector of port configuration
		items */
	double m_pipe_cycle;  /**< \brief Pipeline frequency of one router, used by
		frequency scaling, unit by cycle. */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item with default value.
	 */
	EsyNetworkCfgRouter();
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsyNetworkCfgRouter(const EsyNetworkCfgRouter & t);
	///@}

	/** @name Functions to access variables. */
	///@{
	/*!
	 * \brief Access router id #m_id.
	 * \return router id #m_id.
	 */
	long routerId() const { return m_id; }
	/*!
	 * \brief Set router id #m_id.
	 * \param id router id.
	 */
	void setRouterId( long id ) { m_id = id; }
	/*!
	 * \brief Access position of router in 2D graphics scene #m_pos.
	 * \return position of router in 2D graphics scene #m_pos.
	 */
	const pair< double, double > & pos() const { return m_pos; }
	/*!
	 * \brief Set position of router in 2D graphics scene #m_pos.
	 * \param first  position on first dimension.
	 * \param second position on second dimension.
	 */
	void setPos( double first, double second )
		{ m_pos.first = first; m_pos.second = second; }
	/*!
	 * \brief Set position of router on first dimension in GUI.
	 * \param pos position on first dimension.
	 */
	void setPosFirst( double pos ) { m_pos.first = pos; }
	/*!
	 * \brief Set position of router on second dimension in GUI.
	 * \param pos position on second dimension.
	 */
	void setPosSecond( double pos ) { m_pos.second = pos; }
	/*!
	 * \brief Return the number of port in router.
	 * \return  the number of port in router.
	 */
	long portNum() const { return (long)m_port.size(); }
	/*!
	 * \brief Sets the number of port in router.
	 * \param num  the number of port in router.
	 */
	void resizePort( long num ) { m_port.resize( num ); }
	/*!
	 * \brief Access the vector of port configuration #m_port.
	 * \return vector of port configuration #m_port.
	 */
	const vector< EsyNetworkCfgPort > & port() const { return m_port; }
	/*!
	 * \brief Access configuration of specified port.
	 * \param i id of port.
	 * \return configuration of port i.
	 */
	const EsyNetworkCfgPort & port(long i) const { return m_port[i]; }
	/*!
	 * \brief Access configuration of specified port.
	 * \param i id of port.
	 * \return configuration of port i.
	 */
	EsyNetworkCfgPort & port(long i) { return m_port[i]; }
	/*!
	 * \brief Set the vector of port configuration #m_port.
	 * \param port vector of port configuration.
	 */
	void setPort(const vector< EsyNetworkCfgPort > & port) { m_port = port; }
	void appendPort( const EsyNetworkCfgPort & port ) 
		{ m_port.push_back( port ); }
	/*!
	 * \brief Access pipeline frequency of one router #m_pipe_cycle.
	 * \return pipeline frequency of one router #m_pipe_cycle.
	 */
	double pipeCycle() const { return m_pipe_cycle; }
	/*!
	 * \brief Set pipeline frequency of one router #m_pipe_cycle.
	 * \param pipe pipeline frequency of one router.
	 */
	void setPipeCycle( double pipe ) { m_pipe_cycle = pipe; }
	///@}

	/*!
	 * \brief Return the maximum number of virtual channels for all ports.
	 * \return  the maximum number of virtual channels.
	 */
	long maxVcNum() const;
	/*!
	 * \brief Return the maximum size of input buffer for all ports.
	 * \return  the maximum size of input buffer.
	 */
	long maxInputBuffer() const;
	/*!
	 * \brief Return the maximum size of output buffer for all ports.
	 * \return  the maximum size of output buffer.
	 */
	long maxOutputBuffer() const;
	
	long totalInputVc() const;
	long totalOutputVc() const;

	/*!
	 * \brief Get the name of port.
	 *
	 * Format: \<port direction\>.\<port id\>. e.g. L.0, N.1, S.2, E.3, W.4. etc.
	 *
	 * \param i port id.
	 * \return  name of port.
	 */
	string portName( long i ) const;

	/** @name Functions to access network configuration file in XML. */
	///@{
	/*!
	 * \brief Read router configuration from XML file.
	 * \param root  root of XML structure.
	 */
	void readXml( TiXmlElement * root );
	/*!
	 * \brief Write router configuration to XML file.
	 * \param root  root of XML structure.
	 */
	void writeXml( TiXmlElement * root );
	///@}

	/*!
	 * \brief Overload operator == function. Compare value of two items.
	 * \param t  item to compare.
	 * \return If t has the same value as this item at all field, return TRUE.
	 * Otherwise, return FALSE.
	 */
	bool operator ==( const EsyNetworkCfgRouter & t ) const;
};

/**
 * @ingroup networkcfggroup
 * @brief Structure of network configuration item.
 *
 * Holds parameters and value of one network. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Default </th>
 *   <th> Access </th>
 * </tr>
 * <tr>
 *   <td> NoCTopology #m_topology </td>
 *   <td> Network topology. </td>
 *   <td> DEFAULT_TOPOLOGY = NOC_TOPOLOGY_ROUTER </td>
 *   <td> #topology(), #setTopology() </td>
 * </tr>
 * <tr>
 *   <td> vector< long > #m_size </td>
 *   <td> Size of network in dimensions. </td>
 *   <td> DEFAULT_NETWORK_DIM = 1, DEFAULT_NETWORK_SIZE = 1 </td>
 *   <td> #size(), #setSize(), #dim(), #resizeDim() </td>
 * </tr>
 * <tr>
 *   <td> long #m_flit_size </td>
 *   <td> Size of flit, unit by 64 bits. </td>
 *   <td> DEFAULT_FLIT_SIZE = 1 </td>
 *   <td> #flitSize(), #setFlitSize() </td>
 * </tr>
 * <tr>
 *   <td> long #m_data_width </td>
 *   <td> Width of data path. </td>
 *   <td> DEFAULT_NETWORK_DATA_WIDTH = 32 </td>
 *   <td> #dataPathWidth(), #setDataPathWidth() </td>
 * </tr>
 * <tr>
 *   <td> string #m_routing_alg </td>
 *   <td> Routing algorithm. </td>
 *   <td> DEFAULT_ROUTER_ALG = "XY" </td>
 *   <td> #routingAlg(), #setRoutingAlg() </td>
 * </tr>
 * <tr>
 *   <td> long #m_vc_share </td>
 *   <td> Share strategy for virtual channel. </td>
 *   <td> 0 </td>
 *   <td> #vcShare(), #setVcShare() </td>
 * </tr>
 * <tr>
 *   <td> vector< EsyNetworkCfgPort > #m_port </td>
 *   <td> Vector of port configuration items valid for all routers expect
 * routers configured by items in #m_router. </td>
 *   <td> DEFAULT_PHY_NUM = 0 </td>
 *   <td> #portNum(), #resizePort(), #port(), #appendPort() </td>
 * </tr>
 * <tr>
 *   <td> vector< EsyNetworkCfgRouter > #m_router </td>
 *   <td> Vector of router configuration item. </td>
 *   <td> DEFAULT_ROUTER_COUNT = 1, default router </td>
 *   <td> #router() </td>
 * </tr>
 * </table>
 *
 * Network configuration has general configuration and custom configuration.
 * General configuration is valid for all routers. The configuration of ports in
 * general configuration is stored in #m_port. Each port item does not specified
 * the output neighbor. The configuration of routers in custom configuration is
 * stored in #m_router.
 *
 * Parameters and values can be stored in XML file through TinyXML. Only
 * custom configuration of router is stored in XML file. The interface can
 * generate the configuration of routers based on general configuration
 * automatically.
 *
 * \sa EsyNetworkCfgPort, EsyNetworkCfgRouter
 */
class EsyNetworkCfg
{
public:
	/*!
	 * \brief Network topology.
	 */
	enum NoCTopology
	{
		NOC_TOPOLOGY_ROUTER,	/**< Single router */
		NOC_TOPOLOGY_2DMESH,	/**< 2D Mesh */
		NOC_TOPOLOGY_2DTORUS,   /**< 2D Torus */
		NOC_TOPOLOGY_IRREGULAR, /**< Irregular, all customed */
		NOC_TOPOLOGY_COUNT /**< count of topolgies */
	};

	/*!
	 * \brief Axis direction for 2D topology.
	 */
	enum AXIS_DIR
	{
		AX_X = 0,  /**< X, row */
		AX_Y = 1,  /**< Y, column */
		AX_NUM /**< count of axis */
	};

	#define NETCFG_EXTENSION "netcfg"
	
protected:
	static string const_noc_topology[ NOC_TOPOLOGY_COUNT ];  /**< \brief
		String for NoC topology. */
	static string const_axis_dir[ AX_NUM ];  /**< \brief String for axis. */

	NoCTopology m_topology;  /**< \brief Network topology. */
	vector< long > m_size;  /**< \brief Size of network in dimensions. */
	long m_data_width;  /**< \brief Width of data path. */
	EsyNetworkCfgRouter m_template_router;  /**< \brief Template router, used to
	 * generate regular topology. */
	vector< EsyNetworkCfgRouter > m_router;  /**< \brief Vector of router
	 * configuration item. */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item with default value.
	 */
	EsyNetworkCfg();
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsyNetworkCfg( const EsyNetworkCfg & t );
	///@}

	/** @name Functions to access variables. */
	///@{
	/*!
	* \brief Access network topology #m_topology.
	* \return network topology #m_topology.
	*/
	NoCTopology topology() const { return m_topology; }
	/*!
	 * \brief Set network topology #m_topology.
	 * \param topology network topology.
	 */
	void setTopology( NoCTopology topology );
	/*!
	 * \brief Access size of network.
	 * \return size of network.
	 */
	const string topologyStr() const { return const_noc_topology[ m_topology ]; }
	const vector< long > & size() const { return m_size; }
	/*!
	 * \brief Access size of network in specified dimension.
	 * \param dim specified dimension.
	 * \return  size of network in dimension dim.
	 */
	long size( long dim ) const { return m_size[ dim ]; }
	/*!
	 * \brief Set size of network #m_size.
	 * \param size  size of network.
	 */
	void setSize( const vector< long > & size ) { m_size = size; }
	/*!
	 * \brief Set size of network in specified dimension.
	 * \param dim  specified dimension.
	 * \param size size in specified dimension.
	 */
	void setSize( long dim, long size ) { m_size[ dim ] = size; }
	/*!
	 * \brief Access dimension of network size.
	 * \return  dimension of network size.
	 */
	long dim() { return (long)m_size.size(); }
	/*!
	 * \brief Set dimension of network size.
	 * \param dim  dimension of network size.
	 */
	void resizeDim( long dim ) { m_size.resize( dim ); }
	/*!
	 * \brief Access size of flit #m_flit_size.
	 * \return size of flit #m_flit_size.
	 */
	long flitSize( long atom_width ) const
		{ return ( m_data_width / atom_width ) + 
			( m_data_width % atom_width > 0 )?1:0; }
	/*!
	 * \brief Access width of data path #m_data_width.
	 * \return width of data path #m_data_width.
	 */
	long dataPathWidth() const { return m_data_width; }
	/*!
	 * \brief Set width of data path #m_data_width.
	 * \param size width of data path.
	 */
	void setDataPathWidth( long size ) { m_data_width = size; }


	const EsyNetworkCfgRouter & templateRouter() const
		{ return m_template_router; }
	EsyNetworkCfgRouter & templateRouter() { return m_template_router; }
	void setTemplateRouter( const EsyNetworkCfgRouter & t ) 
		{ m_template_router = t; }
	/*!
	 * \brief Access router configuration items #m_router.
	 * \return router configuration items #m_router.
	 */
	const vector< EsyNetworkCfgRouter > & router() const { return m_router; }
	/*!
	 * \brief Access specified router configuration item.
	 * \param id router id.
	 * \return specified router configuration item.
	 */
	const EsyNetworkCfgRouter & router( int id ) const { return m_router[ id ]; }
	/*!
	 * \brief Access specified router configuration item.
	 * \param id router id.
	 * \return specified router configuration item.
	 */
	EsyNetworkCfgRouter & router( int id ) { return m_router[ id ]; }
	///@}
	
	/*!
	 * \brief Access the name of NoC topology.
	 * \param i  code of NoC toplogy.
	 * \return  the full name of NoC toplogy.
	 */
	static string nocTopologyStrVector( int i )
		{ return const_noc_topology[ i ]; }
	/*!
	 * \brief Access the name of NoC axis.
	 * \param i  code of NoC axis.
	 * \return  the full name of NoC axis.
	 */
	static string nocAxisStrVector( int i )
		{ return const_axis_dir[ i ]; }

	/*!
	 * \brief Return the maximum number of physical channels for all routers.
	 * \return  the maximum number of physical channels
	 */
	long maxPcNum() const;
	/*!
	 * \brief Return the maximum number of virtual channels for all ports of
	 * all routers.
	 * \return  the maximum number of virtual channels.
	 */
	long maxVcNum() const;
	/*!
	 * \brief Return the maximum size of input buffer for all ports of all
	 * routers.
	 * \return  the maximum size of input buffer.
	 */
	long maxInputBuffer() const;
	/*!
	 * \brief Return the maximum number of output buffer for all ports of all
	 * routers.
	 * \return  the maximum size of output buffer.
	 */
	long maxOutputBuffer() const;

	/** @name Functions to access network configuration file in XML. */
	///@{
	/*!
	 * \brief Read network configuration from XML file.
	 * \param root  root of XML structure.
	 */
	void readXml( TiXmlElement * root );
	/*!
	 * \brief Open XML network configuration file for reading.
	 * \param fname  direction of network configuration file.
	 * \return  XML error handler.
	 * \sa readXml(TiXmlElement * root), EsyNetworkCfgRouter::readXml(),
	 * EsyNetworkCfgPort::readXml()
	 */
	EsyXmlError readXml( const string & fname );
	/*!
	 * \brief Write network configuration to XML file.
	 * \param root  root of XML structure.
	 */
	void writeXml( TiXmlElement * root );
	/*!
	 * \brief Open XML network configuration file for writing.
	 * \param fname  direction of network configuration file.
	 * \return  XML error handler.
	 * \sa writeXml(TiXmlElement * root), EsyNetworkCfgRouter::writeXml(),
	 * EsyNetworkCfgPort::writeXml()
	 */
	EsyXmlError writeXml( const string & fname );
	///@}

	/*!
	 * \brief Calculate the number of routers in network.
	 * \return  number of routers
	 */
	int routerCount() const;
	/*!
	 * \brief Convert router coordinary to router id based on topology.
	 * \param coord  router coordinary.
	 * \return  router id.
	 */
	long coord2Seq( const vector< long > & coord );
	/*!
	 * \brief Convert router id to router coordinary based on topology.
	 * \param seq  router id.
	 * \return  router coordinary.
	 */
	vector< long > seq2Coord( long seq );
	/*!
	 * \brief Calculate the distance between two positions.
	 * \param p1  position 1.
	 * \param p2  position 2.
	 * \return  Mahanten distance between positions.
	 */
	int coordDistance( long p1, long p2 );

	/*!
	 * \brief Get the configuration of specified router based on general
	 * configurations.
	 * \param id id of specified router.
	 * \return  router configuration item.
	 */
	EsyNetworkCfgRouter defaultRouterCfg( long id );
	/*!
	 * \brief Generate configuration of each routers based on general
	 * configurations.
	 */
	void updateRouter();

	/*!
	 * \brief Check the router id is valid or not.
	 * \param id router id to check.
	 * \return  true, if the router id is valid.
	 */
	bool validRouterId( long id ) const
		{ return ( ( id >= 0 ) && ( id < routerCount() ) ); }
};

#endif
