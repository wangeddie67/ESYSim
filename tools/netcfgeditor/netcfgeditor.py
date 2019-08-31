#!/usr/bin/env python3

import sys
import argparse

sys.path.append('../../libs/netcfg')
from netcfg import EsyNetCfg

def view( args ) :
    print( 'Print network configuration file.' )

    net_cfg = EsyNetCfg().readXml( args.file )
    print( net_cfg )

def create( args ) :
    print( 'Create new network configuration file.' )
    # argument check
    if args.pipe_cycle <= 0.0 :
        args.pipe_cycle = 1.0
    if args.phy_number <= 0 :
        args.phy_number = len( args.network_size ) * 2 + 1
    if args.in_vc_number <= 0 :
        args.in_vc_number = 1
    if args.out_vc_number <= 0 :
        args.out_vc_number = 1
    if args.in_buffer_size <= 0 :
        args.in_buffer_size = 10
    if args.out_buffer_size <= 0 :
        args.out_buffer_size = 10
    if args.ni_pipe_cycle <= 0.0 :
        args.a_ni_pipe_cycle = 1.0
    if args.ni_buffer_size <= 0 :
        args.ni_buffer_size = 10
    if args.ni_interrupt_delay <= 0.0 :
        args.ni_interrupt_delay = 100

    # Create new network configuration
    net_cfg = EsyNetCfg().createNetCfg( args.topology, args.net_size, args.pipe_cycle, \
                                        args.phy_port_num, args.in_vc_num, args.out_vc_num, \
                                        args.in_buf_size, args.out_buf_size, \
                                        args.ni_pipe_cycle, args.ni_buf_size, args.ni_int_delay )
    net_cfg.writeXml( args.file )
    # Print network configuration
    print( net_cfg )

def modify( args ) :
    print( 'Modify network configuration file.' )
    print( args )
    '''// Read in network configuration file
        EsyNetCfg net_cfg;
        EsyXmlError xml_error = net_cfg.readXml( a_netcfg_file_name );
        if ( xml_error.hasError() )
        {
            std::cout << "Error: cannot read file " << a_netcfg_file_name << "." << std::endl;
            return 2;
        }

        // Configure template router
        if ( a_template_router )
        {
            EsyNetCfgRouter& router_cfg = net_cfg.templateRouter();
            // Configure port
            if ( a_port_id > 0 && a_port_id <= router_cfg.portNum() )
            {
                EsyNetCfgPort& port_cfg = router_cfg.port( a_port_id );
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
            EsyNetCfgNI& ni_cfg = net_cfg.templateNI();
            std::cout << "ni_pipe_cycle = " << ni_cfg.pipeCycle() << std::endl;
            std::cout << "ni_buf_size = " << ni_cfg.bufferSize() << std::endl;
            std::cout << "ni_int_delay = " << ni_cfg.interruptDelay() << std::endl;
        }
        // Configure router
        else if ( a_router_id >= 0 && a_router_id < net_cfg.routerCount() )
        {
            EsyNetCfgRouter& router_cfg = net_cfg.router( a_router_id );
            // Configure port
            if ( a_port_id >= 0 && a_port_id < router_cfg.portNum() )
            {
                EsyNetCfgPort& port_cfg = router_cfg.port( a_port_id );
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
            EsyNetCfgNI& ni_cfg = net_cfg.ni( a_ni_id );
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
    }'''

def search( args ) :
    print( 'Search parameters of specified network/router/port/NI.' )
    print( args )
    '''0EsyNetCfg net_cfg;
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
                EsyNetCfgRouter& router_cfg = net_cfg.templateRouter();
                // Configure port
                if ( a_port_id > 0 && a_port_id <= router_cfg.portNum() )
                {
                    EsyNetCfgPort& port_cfg = router_cfg.port( a_port_id );
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
                    if ( a_axis_dir < EsyNetCfgPort::AXDIR_NUM )
                    {
                        port_cfg.setPortAxisDir( ( EsyNetCfgPort::PortAxisDirection )( long )a_axis_dir );
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
                EsyNetCfgNI& ni_cfg = net_cfg.templateNI();
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
            EsyNetCfgRouter& router_cfg = net_cfg.router( a_router_id );
            // Configure port
            if ( a_port_id >= 0 && a_port_id <= router_cfg.portNum() )
            {
                EsyNetCfgPort& port_cfg = router_cfg.port( a_port_id );
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
                if ( a_axis_dir < EsyNetCfgPort::AXDIR_NUM )
                {
                    port_cfg.setPortAxisDir( ( EsyNetCfgPort::PortAxisDirection )( long )a_axis_dir );
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

            if ( a_update )
            {
                net_cfg.updateNI();
            }
        }
        // Configure NI
        else if ( a_ni_id >= 0 && a_ni_id < net_cfg.niCount() )
        {
            EsyNetCfgNI& ni_cfg = net_cfg.ni( a_ni_id );
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
        }'''

def optionParser() :

    # argument parser
    parser = argparse.ArgumentParser( prog='netcfgeditor.py', description='Network Configuration Editor' )
#    argu.insertOpenFile( "-network_cfg_file_name", "Network configuration file name", &a_netcfg_file_name, "", 1, NETCFG_EXTENSION );
    subparsers = parser.add_subparsers( title='comamnds', description='valid subcommands', dest='command' )

    # subcommand: create
    parser_create = subparsers.add_parser( 'create', help='Create new network configuration file' )
    parser_create.add_argument( '--file', help='Network configuration file name', required=True )
    # topology group
    parser_create_topology_group = parser_create.add_argument_group( 'arguments to specify topology' )
    parser_create_topology_group.add_argument( '--topology', choices=['Switch', 'Ring', '2DMesh', '2DTorus', 'DiaMesh', 'DiaTorus', 'Irregular'], help='Code for topology', default='Switch' )
    parser_create_topology_group.add_argument( '--network_size', help='Size of network in diamension', nargs='+', type=int, default=[1] )
    # router group
    parser_create_router_group = parser_create.add_argument_group( 'arguments to specify router configuration' )
    parser_create_router_group.add_argument( '--pipe_cycle', help='Pipeline cycle of routers', type=float, default=1.0 )
    parser_create_router_group.add_argument( '--phy_number', help='Number of physical port of a router', type=int, default=5 )
    parser_create_router_group.add_argument( '--in_vc_number', help='Number of input virtual channel', type=int, default=1 )
    parser_create_router_group.add_argument( '--out_vc_number', help='Number of output virtual channel', type=int, default=1 )
    parser_create_router_group.add_argument( '--in_buffer_size', help='Size of input buffer of each VC', type=int, default=10 )
    parser_create_router_group.add_argument( '--out_buffer_size', help='Size of output buffer of each VC', type=int, default=10 )
    # NI group
    parser_create_ni_group = parser_create.add_argument_group( 'arguments to specify NI configuration' )
    parser_create_ni_group.add_argument( '--ni_pipe_cycle', help='Pipeline cycle of NIs', type=float, default=1.0 )
    parser_create_ni_group.add_argument( '--ni_buffer_size', help='Buffer size of NI', type=int, default=10 )
    parser_create_ni_group.add_argument( '--ni_interrupt_delay', help='Interruption delay of NI', type=float, default=100 )

    # subcommand: view
    parser_view = subparsers.add_parser( 'view', help='Print network configuration file' )
    parser_view.add_argument( '--file', help='Network configuration file name', required=True )

    # subcommand: modify
    parser_modify = subparsers.add_parser( 'modify', help='Modify network configuration file' )
    parser_modify.add_argument( '--file', help='Network configuration file name', required=True )
    parser_modify.add_argument( '--update', help='Update network configuration', action='store_true', default=False )
    # select component
    parser_modify_select_group = parser_modify.add_argument_group( 'arguments to specify component to modify' )
    parser_modify_select_group.add_argument( '--router_id', help='ID of the router to modify', type=int, default=-1 )
    parser_modify_select_group.add_argument( '--template_router', help='Modify template router', action='store_true', default=False )
    parser_modify_select_group.add_argument( '--port_id', help='ID of the port to modify', type=int, default=-1 )
    parser_modify_select_group.add_argument( '--ni_id', help='ID of the NI to modify', type=int, default=-1 )
    parser_modify_select_group.add_argument( '--template_ni', help='Modify template NI', action='store_true', default=False )
    # router group
    parser_modify_router_group = parser_modify.add_argument_group( 'arguments to modify router configuration' )
    parser_modify_router_group.add_argument( '--pipe_cycle', help='Pipeline cycle of routers', type=float, default=1.0 )
    parser_modify_router_group.add_argument( '--phy_number', help='Number of physical port of a router', type=int, default=5 )
    # port group
    parser_modify_port_group = parser_modify.add_argument_group( 'arguments to modify port configuration' )
    parser_modify_port_group.add_argument( '--in_vc_number', help='Number of input virtual channel', type=int, default=1 )
    parser_modify_port_group.add_argument( '--out_vc_number', help='Number of output virtual channel', type=int, default=1 )
    parser_modify_port_group.add_argument( '--in_buffer_size', help='Size of input buffer of each VC', type=int, default=10 )
    parser_modify_port_group.add_argument( '--out_buffer_size', help='Size of output buffer of each VC', type=int, default=10 )
    parser_modify_port_group.add_argument( '--axis', help='Axis of the port', type=int, default=0 )
    parser_modify_port_group.add_argument( '--axis_dir', choices=['Up', 'Down'], help='Direction of axis', default='Up' )
    parser_modify_port_group.add_argument( '--connect_router', help='Connected router or NI', type=int, default=-1 )
    parser_modify_port_group.add_argument( '--connect_port', help='Connected port', type=int, default=-1 )
    parser_modify_port_group.add_argument( '--connect_ni', help='Connect to NI', action='store_true', default=False )
    parser_modify_port_group.add_argument( '--not_connect_ni', help='Not connect to NI', action='store_true', default=False )
    # NI group
    parser_modify_ni_group = parser_modify.add_argument_group( 'arguments to modify NI configuration' )
    parser_modify_ni_group.add_argument( '--ni_pipe_cycle', help='Pipeline cycle of NIs', type=float, default=1.0 )
    parser_modify_ni_group.add_argument( '--ni_buffer_size', help='Buffer size of NI', type=int, default=10 )
    parser_modify_ni_group.add_argument( '--ni_interrupt_delay', help='Interruption delay of NI', type=float, default=100 )
    parser_modify_ni_group.add_argument( '--ni_connect_router', help='Connected router of NI', type=int, default=-1 )
    parser_modify_ni_group.add_argument( '--ni_connect_port', help='Connected port of NI', type=int, default=-1 )

    # subcommand: search
    parser_search = subparsers.add_parser( 'search', help='Search parameters of specified network/router/port/NI' )
    parser_search.add_argument( '--file', help='Network configuration file name', required=True )
    # select component
    parser_search_select_group = parser_search.add_argument_group( 'arguments to specify component to search' )
    parser_search_select_group.add_argument( '--router_id', help='ID of the router to modify', type=int, default=-1 )
    parser_search_select_group.add_argument( '--template_router', help='Modify template router', action='store_true', default=False )
    parser_search_select_group.add_argument( '--port_id', help='ID of the port to modify', type=int, default=-1 )
    parser_search_select_group.add_argument( '--ni_id', help='ID of the NI to modify', type=int, default=-1 )
    parser_search_select_group.add_argument( '--template_ni', help='Modify template NI', action='store_true', default=False )

    args = parser.parse_args()
    return args

# Main function
if __name__ == "__main__" :
    # parse option
    args = optionParser()

    if args.command == 'create' :
        create( args )
    elif args.command == 'view' :
        view( args )
    elif args.command == 'modify' :
        modify( args )
    elif args.command == 'search' :
        search( args )
