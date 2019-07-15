/*
 * File name : netcfgeditor.cc
 * Function : Network configuration editor
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

/**
 * @ingroup TOOLS_NETCFGEDITOR
 * @file netcfgeditor.cc
 * @brief Implement network configuration editor.
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "esy_netcfg.h"
#include "esy_argument.h"

int main( int argc, char * argv[])
{
    // Define arguments
    EsyArgumentEnum a_topology( EsyNetworkCfg::NT_MESH_2D );    /**< @brief Network topology. */
    std::vector< long > a_net_size( 2, 8 );     /**< @brief Size of network in each dimension. */

    double a_pipe_cycle = -1;      /**< @brief Pipe cycle of router. */
    long a_phy_port_num = -1;        /**< @brief The number of physical ports in each router. */
    long a_in_vc_num = -1;           /**< @brief The number of virtual channels in each physical port. */
    long a_out_vc_num = -1;          /**< @brief The number of virtual channels in each physical port. */
    long a_in_buf_size = -1;        /**< @brief Input buffer size of each virtual channel. */
    long a_out_buf_size = -1;       /**< @brief Output buffer size of each virtual channel. */
    long a_axis = -1;           /**< @brief Port axis. */
    EsyArgumentEnum a_axis_dir( EsyNetworkCfgPort::AXDIR_NUM ); /**< @brief Direction on axis. */
    long a_conn_router = -1;     /**< @brief Connected router/NI. */
    long a_conn_port = -1;       /**< @brief Connected port. */
    bool a_conn_ni = false;         /**< @brief The port connect to NI. */
    bool a_not_conn_ni = false;     /**< @brief The port does not connect to NI. */

    double a_ni_pipe_cycle = -1;   /**< @brief Pipe cycle of NI. */
    long a_ni_buf_size = -1;        /**< @brief Size of buffer in NI. */
    double a_ni_int_delay = -1;      /**< @brief Delay of NI interruption. */
    long a_ni_conn_router = -1;     /**< @brief Connected router. */
    long a_ni_conn_port = -1;       /**< @brief Connected port. */

    std::string a_netcfg_file_name = "../example/routing";      /**< @brief Network configuration file name. */

    bool a_create = false;
    bool a_view = false;
    bool a_modify = false;
    bool a_search = false;

    long a_router_id = -1;
    long a_ni_id = -1;
    long a_port_id = -1;
    bool a_template_router = false;
    bool a_template_ni = false;

    bool a_update = false;

    EsyArgumentList argu;
    a_topology.addOption( EsyNetworkCfg::NT_SWITCH,    "Switch" );
    a_topology.addOption( EsyNetworkCfg::NT_RING,      "Ring" );
    a_topology.addOption( EsyNetworkCfg::NT_MESH_2D,   "2DMesh" );
    a_topology.addOption( EsyNetworkCfg::NT_TORUS_2D,  "2DTorus" );
    a_topology.addOption( EsyNetworkCfg::NT_MESH_DIA,  "DiaMesh" );
    a_topology.addOption( EsyNetworkCfg::NT_TORUS_DIA, "DiaTorus" );
    a_topology.addOption( EsyNetworkCfg::NT_IRREGULAR, "Irregular" );
    argu.insertEnum( "-topology", "Code for topology", &a_topology );
    argu.insertLongVector( "-network_size", "Size of network in diamension", &a_net_size );

    argu.insertDouble( "-pipe_cycle", "Pipeline cycle of routers", &a_pipe_cycle );
    argu.insertLong( "-phy_number", "Number of physical port of a router", &a_phy_port_num );
    argu.insertLong( "-in_vc_number", "Number of input virtual channel", &a_in_vc_num );
    argu.insertLong( "-out_vc_number", "Number of output virtual channel", &a_out_vc_num );
    argu.insertLong( "-in_buffer_size", "Buffer size of each VC", &a_in_buf_size );
    argu.insertLong( "-out_buffer_size", "Output buffer size", &a_out_buf_size );
    argu.insertLong( "-axis", "Axis of the port", &a_axis );
    a_axis_dir.addOption( EsyNetworkCfgPort::AXDIR_UP,   "Up" );
    a_axis_dir.addOption( EsyNetworkCfgPort::AXDIR_DOWN, "Down" );
    argu.insertEnum( "-axis_dir", "Direction of axis", &a_axis_dir );
    argu.insertLong( "-connect_router", "Connected router or NI. NI is after routers.", &a_conn_router );
    argu.insertLong( "-connect_port", "Connected port. NI is after routers.", &a_conn_port );
    a_axis_dir.addOption( EsyNetworkCfgPort::AXDIR_UP,   "Up" );
    a_axis_dir.addOption( EsyNetworkCfgPort::AXDIR_DOWN, "Down" );
    argu.insertBool( "-connect_ni", "Connect to NI.", &a_conn_ni );
    argu.insertBool( "-not_connect_ni", "Not connect to NI.", & a_not_conn_ni );

    argu.insertDouble( "-ni_pipe_cycle", "Pipeline cycle of NIs", &a_ni_pipe_cycle );
    argu.insertLong( "-ni_buffer_size", "Buffer size of NI, #unit", &a_ni_buf_size );
    argu.insertDouble( "-ni_interrupt_delay", "interruption delay of NI, #cycle", &a_ni_int_delay );
    argu.insertLong( "-ni_connect_router", "Connected router of NI", &a_ni_conn_router );
    argu.insertLong( "-ni_connect_port", "Connected port of NI", &a_ni_conn_port );

    argu.insertOpenFile( "-network_cfg_file_name", "Network configuration file name", &a_netcfg_file_name, "", 1, NETCFG_EXTENSION );

    argu.insertBool( "-create", "Create new network configuration file", &a_create );
    argu.insertBool( "-view", "Print network configuration file", &a_view );
    argu.insertBool( "-modify", "Modify network configuration file", &a_modify );
    argu.insertBool( "-search", "Search parameters of specified network/router/port/ni", &a_search );

    argu.insertLong( "-router_id", "ID of the router to modify", &a_router_id );
    argu.insertBool( "-template_router", "Modify template router", &a_template_router );
    argu.insertLong( "-port_id", "ID of the port to modify", &a_port_id );
    argu.insertLong( "-ni_id", "ID of the NI to modify", &a_ni_id );
    argu.insertBool( "-template_ni", "Modify template NI", &a_template_ni );

    argu.insertBool( "-update", "Update network configuration", &a_update );

    if ( argu.analyse( argc, argv ) != EsyArgumentList::RESULT_OK )
    {
        return 1;
    }

    // View the network configuration file.
    if ( a_view )
    {
        // Read in network configuration file
        EsyNetworkCfg net_cfg;
        EsyXmlError xml_error = net_cfg.readXml( a_netcfg_file_name );
        if ( xml_error.hasError() )
        {
            std::cout << "Error: cannot read file " << a_netcfg_file_name << "." << std::endl;
            return 2;
        }
        std::cout << net_cfg;
    }
    // Create new network configuration file.
    else if ( a_create )
    {
        // default values
        if ( a_pipe_cycle < 0 )
        {
            a_pipe_cycle = 1.0;
        }
        if ( a_phy_port_num <= 0 )
        {
            a_phy_port_num = a_net_size.size() * 2 + 1;
        }
        if ( a_in_vc_num <= 0 )
        {
            a_in_vc_num = 1;
        }
        if ( a_out_vc_num <= 0 )
        {
            a_out_vc_num = 1;
        }
        if ( a_in_buf_size <= 0 )
        {
            a_in_buf_size = 10;
        }
        if ( a_out_buf_size <= 0 )
        {
            a_out_buf_size = 10;
        }
        if ( a_ni_pipe_cycle < 0 )
        {
            a_ni_pipe_cycle = 1.0;
        }
        if ( a_ni_buf_size <= 0 )
        {
            a_ni_buf_size = 10;
        }
        if ( a_ni_int_delay < 0 )
        {
            a_ni_int_delay = 100;
        }

        // Create new network configuration
        EsyNetworkCfg net_cfg( ( EsyNetworkCfg::NoCTopology )( long )a_topology, a_net_size
            , a_pipe_cycle, a_phy_port_num, a_in_vc_num, a_out_vc_num, a_in_buf_size, a_out_buf_size
            , a_ni_pipe_cycle, a_ni_buf_size, a_ni_int_delay );
        // Print network configuration
        std::cout << net_cfg;

        EsyXmlError xml_error = net_cfg.writeXml( a_netcfg_file_name );
        if ( xml_error.hasError() )
        {
            std::cout << "Error: cannot write file " << a_netcfg_file_name << "." << std::endl;
            return 2;
        }
    }
    // Search parameters of network or specified router/ni/port
    else if ( a_search )
    {
        // Read in network configuration file
        EsyNetworkCfg net_cfg;
        EsyXmlError xml_error = net_cfg.readXml( a_netcfg_file_name );
        if ( xml_error.hasError() )
        {
            std::cout << "Error: cannot read file " << a_netcfg_file_name << "." << std::endl;
            return 2;
        }

        // Configure template router
        if ( a_template_router )
        {
            EsyNetworkCfgRouter& router_cfg = net_cfg.templateRouter();
            // Configure port
            if ( a_port_id > 0 && a_port_id <= router_cfg.portNum() )
            {
                EsyNetworkCfgPort& port_cfg = router_cfg.port( a_port_id );
                std::cout << "in_vc_num = " << port_cfg.inputVcNumber() << std::endl;
                std::cout << "out_vc_num = " << port_cfg.outputVcNumber() << std::endl;
                std::cout << "in_buf_size = " << port_cfg.inputBuffer() << std::endl;
                std::cout << "out_buf_size = " << port_cfg.outputBuffer() << std::endl;
                std::cout << "axis = " << port_cfg.portAxis() << std::endl;
                std::cout << "axis_dir = " << port_cfg.portAxisDirectionStr() << std::endl;
                std::cout << "ni = " << ( port_cfg.networkInterface() ? "True" : "False" ) << std::endl;
            }
            // Configure router
            else
            {
                std::cout << "pipe_cycle = " << router_cfg.pipeCycle() << std::endl;
                std::cout << "port_num = " << router_cfg.portNum() << std::endl;
            }

        }
        else if ( a_template_ni )
        {
            EsyNetworkCfgNI& ni_cfg = net_cfg.templateNI();
            std::cout << "ni_pipe_cycle = " << ni_cfg.pipeCycle() << std::endl;
            std::cout << "ni_buf_size = " << ni_cfg.bufferSize() << std::endl;
            std::cout << "ni_int_delay = " << ni_cfg.interruptDelay() << std::endl;
        }
        // Configure router
        else if ( a_router_id >= 0 && a_router_id < net_cfg.routerCount() )
        {
            EsyNetworkCfgRouter& router_cfg = net_cfg.router( a_router_id );
            // Configure port
            if ( a_port_id >= 0 && a_port_id < router_cfg.portNum() )
            {
                EsyNetworkCfgPort& port_cfg = router_cfg.port( a_port_id );
                std::cout << "in_vc_num = " << port_cfg.inputVcNumber() << std::endl;
                std::cout << "out_vc_num = " << port_cfg.outputVcNumber() << std::endl;
                std::cout << "in_buf_size = " << port_cfg.inputBuffer() << std::endl;
                std::cout << "out_buf_size = " << port_cfg.outputBuffer() << std::endl;
                std::cout << "axis = " << port_cfg.portAxis() << std::endl;
                std::cout << "axis_dir = " << port_cfg.portAxisDirectionStr() << std::endl;
                std::cout << "ni = " << ( port_cfg.networkInterface() ? "True" : "False" ) << std::endl;
                std::cout << "conn_router = " << port_cfg.neighborRouter() << std::endl;
                std::cout << "conn_port = " << port_cfg.neighborPort() << std::endl;
            }
            // Configure router
            else
            {
                std::cout << "pipe_cycle = " << router_cfg.pipeCycle() << std::endl;
                std::cout << "port_num = " << router_cfg.portNum() << std::endl;
            }
        }
        // Configure NI
        else if ( a_ni_id >= 0 && a_ni_id < net_cfg.niCount() )
        {
            EsyNetworkCfgNI& ni_cfg = net_cfg.ni( a_ni_id );
            std::cout << "ni_pipe_cycle = " << ni_cfg.pipeCycle() << std::endl;
            std::cout << "ni_buf_size = " << ni_cfg.bufferSize() << std::endl;
            std::cout << "ni_int_delay = " << ni_cfg.interruptDelay() << std::endl;
            std::cout << "ni_conn_router = " << ni_cfg.connectRouter() << std::endl;
            std::cout << "ni_conn_port = " << ni_cfg.connectPort() << std::endl;
        }
        // Return parameters of network
        else
        {
            std::cout << "topology = " << net_cfg.topologyStr() << std::endl;
            std::cout << "net_size = ";
            for ( int i = 0; i < net_cfg.dim(); i ++ )
            {
                std::cout << net_cfg.size( i ) << " ";
            }
            std::cout << std::endl;
            std::cout << "router_count = " << net_cfg.routerCount() << std::endl;
            std::cout << "ni_count = " << net_cfg.niCount() << std::endl;
        }
    }
    // Create new network configuration file.
    else if ( a_modify )
    {
        EsyNetworkCfg net_cfg;
        EsyXmlError xml_error = net_cfg.readXml( a_netcfg_file_name );
        if ( xml_error.hasError() )
        {
            std::cerr << "Error: cannot read file " << a_netcfg_file_name << "." << std::endl;
            return 2;
        }

        // Configure template router or template NI
        if ( a_template_router || a_template_ni )
        {
            // Configure template router
            if ( a_template_router )
            {
                EsyNetworkCfgRouter& router_cfg = net_cfg.templateRouter();
                // Configure port
                if ( a_port_id > 0 && a_port_id <= router_cfg.portNum() )
                {
                    EsyNetworkCfgPort& port_cfg = router_cfg.port( a_port_id );
                    if ( a_in_vc_num > 0 )
                    {
                        port_cfg.setInputVcNumber( a_in_vc_num );
                    }
                    if ( a_out_vc_num > 0 )
                    {
                        port_cfg.setOutputVcNumber( a_out_vc_num );
                    }
                    if ( a_in_buf_size > 0 )
                    {
                        port_cfg.setInputBuffer( a_in_buf_size );
                    }
                    if ( a_out_buf_size > 0 )
                    {
                        port_cfg.setOutputBuffer( a_out_buf_size );
                    }
                    if ( a_axis >= 0 )
                    {
                        port_cfg.setPortAxis( a_axis );
                    }
                    if ( a_axis_dir < EsyNetworkCfgPort::AXDIR_NUM )
                    {
                        port_cfg.setPortAxisDir( ( EsyNetworkCfgPort::PortAxisDirection )( long )a_axis_dir );
                    }
                    if ( a_not_conn_ni )
                    {
                        port_cfg.setNetworkInterface( false );
                    }
                    else if ( a_conn_ni )
                    {
                        port_cfg.setNetworkInterface( true );
                    }
                }
                // Configure router
                else
                {
                    if ( a_pipe_cycle >= 0 )
                    {
                        router_cfg.setPipeCycle( a_pipe_cycle );
                    }
                    if ( a_phy_port_num > 0 )
                    {
                        router_cfg.resizePort( a_phy_port_num );
                    }
                }

            }
            else if ( a_template_ni )
            {
                EsyNetworkCfgNI& ni_cfg = net_cfg.templateNI();
                if ( a_ni_pipe_cycle >= 0 )
                {
                    ni_cfg.setPipeCycle( a_ni_pipe_cycle );
                }
                if ( a_ni_buf_size > 0 )
                {
                    ni_cfg.setBufferSize( a_ni_buf_size );
                }
                if ( a_ni_int_delay >= 0 )
                {
                    ni_cfg.setInterruptDelay( a_ni_int_delay );
                }
            }

            if ( a_update )
            {
                net_cfg.updateNetwork();
            }
        }
        // Configure router
        else if ( a_router_id >= 0 && a_router_id < net_cfg.routerCount() )
        {
            EsyNetworkCfgRouter& router_cfg = net_cfg.router( a_router_id );
            // Configure port
            if ( a_port_id >= 0 && a_port_id <= router_cfg.portNum() )
            {
                EsyNetworkCfgPort& port_cfg = router_cfg.port( a_port_id );
                if ( a_in_vc_num > 0 )
                {
                    port_cfg.setInputVcNumber( a_in_vc_num );
                }
                if ( a_out_vc_num > 0 )
                {
                    port_cfg.setOutputVcNumber( a_out_vc_num );
                }
                if ( a_in_buf_size > 0 )
                {
                    port_cfg.setInputBuffer( a_in_buf_size );
                }
                if ( a_out_buf_size > 0 )
                {
                    port_cfg.setOutputBuffer( a_out_buf_size );
                }
                if ( a_axis >= 0 )
                {
                    port_cfg.setPortAxis( a_axis );
                }
                if ( a_axis_dir < EsyNetworkCfgPort::AXDIR_NUM )
                {
                    port_cfg.setPortAxisDir( ( EsyNetworkCfgPort::PortAxisDirection )( long )a_axis_dir );
                }
                if ( a_not_conn_ni )
                {
                    port_cfg.setNetworkInterface( false );
                }
                else if ( a_conn_ni )
                {
                    port_cfg.setNetworkInterface( true );
                }
                if ( port_cfg.networkInterface() )
                {
                    if ( a_conn_router >= 0 && a_conn_router < net_cfg.niCount() )
                    {
                        port_cfg.setNeighborRouter( a_conn_router );
                    }
                    port_cfg.setNeighborPort( 0 );
                }
                else
                {
                    if ( a_conn_router >= 0 && a_conn_router < net_cfg.router( port_cfg.neighborRouter() ).portNum() )
                    {
                        port_cfg.setNeighborRouter( a_conn_router );
                    }
                    if ( a_conn_port >= 0 )
                    {
                        port_cfg.setNeighborPort( a_conn_port );
                    }
                }
            }
            // Configure router
            else
            {
                if ( a_pipe_cycle >= 0 )
                {
                    router_cfg.setPipeCycle( a_pipe_cycle );
                }
                if ( a_phy_port_num > 0 )
                {
                    router_cfg.resizePort( a_phy_port_num );
                }
            }
        }
        // Configure NI
        else if ( a_ni_id >= 0 && a_ni_id < net_cfg.niCount() )
        {
            EsyNetworkCfgNI& ni_cfg = net_cfg.ni( a_ni_id );
            if ( a_ni_pipe_cycle >= 0 )
            {
                ni_cfg.setPipeCycle( a_ni_pipe_cycle );
            }
            if ( a_ni_buf_size > 0 )
            {
                ni_cfg.setBufferSize( a_ni_buf_size );
            }
            if ( a_ni_int_delay >= 0 )
            {
                ni_cfg.setInterruptDelay( a_ni_int_delay );
            }
            if ( a_ni_conn_router >= 0 && a_ni_conn_router < net_cfg.niCount() )
            {
                ni_cfg.setConnectRouter( a_ni_conn_router );
            }
            if ( a_ni_conn_port >= 0 && a_ni_conn_port < net_cfg.router( ni_cfg.connectRouter() ).portNum() )
            {
                ni_cfg.setConnectPort( a_ni_conn_port );
            }
        }
        else
        {
            std::cout << "Please specify the router/port/NI to modify" << std::endl;
            std::cout << "  -router_id  ID of the router to modify." << std::endl;
            std::cout << "  -port_id    ID of the port to modify." << std::endl;
            std::cout << "  -ni_id      ID of the NI to modify." << std::endl;
        }

        std::cout << net_cfg;

        EsyXmlError xml_write_error = net_cfg.writeXml( a_netcfg_file_name );
        if ( xml_write_error.hasError() )
        {
            std::cerr << "Error: cannot write file " << a_netcfg_file_name << "." << std::endl;
            return 2;
        }
    }
    else
    {
        std::cout << "Please specify operation: " << std::endl;
        std::cout << "  -create  Create new network configuration." << std::endl;
        std::cout << "  -view    Print network configuration." << std::endl;
        std::cout << "  -modify  Modify network configuration." << std::endl;
        std::cout << "  -search  Search paramters of network or specified router/port/ni." << std::endl;
    }

    return 0;
}
