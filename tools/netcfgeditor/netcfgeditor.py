#!/usr/bin/env python3

import sys
import argparse

sys.path.append('../../libs')
import netcfg

def view( args ) :
    print( 'Print network configuration file.' )

    net_cfg = netcfg.createNetCfgFromFile( args.file )
    print( net_cfg )

def create( args ) :
    print( 'Create new network configuration file.' )
    # argument check
    assert args.pipe_cycle > 0.0, 'pipe_cycle must be positive.'
    assert args.phy_number > 0, 'phy_number must be positive.'
    assert args.in_vc_number > 0,  'in_vc_number must be positive.'
    assert args.out_vc_number > 0, 'out_vc_number must be positive.'
    assert args.in_buffer_size > 0,  'in_buffer_size must be positive.'
    assert args.out_buffer_size > 0, 'out_buffer_size must be positive.'
    assert args.ni_pipe_cycle > 0.0, 'ni_pipe_cycle must be positive.'
    assert args.ni_buffer_size > 0, 'ni_buffer_size must be positive.'
    assert args.ni_interrupt_delay > 0.0, 'ni_interrupt_delay must be positive.'

    # Create new network configuration
    net_cfg = netcfg.createNetCfg( args.topology, \
                                   args.network_size, \
                                   args.pipe_cycle, \
                                   args.phy_number, \
                                   args.in_vc_number, \
                                   args.out_vc_number, \
                                   args.in_buffer_size, \
                                   args.out_buffer_size, \
                                   args.ni_pipe_cycle, \
                                   args.ni_buffer_size, \
                                   args.ni_interrupt_delay )
    net_cfg.writeXmlFile( args.file )
    # Print network configuration
    print( net_cfg )

def modify( args ) :
    print( 'Modify network configuration file.' )

    net_cfg = netcfg.createNetCfgFromFile( args.file )

    # Configure template router or template NI
    if args.template_router or args.template_ni :
        # Configure template router
        if args.template_router :
            router_cfg = net_cfg.template_router
            # Configure port
            if args.port_id > 0 and args.port_id <= router_cfg.port_num :
                port_cfg = router_cfg.port_list[ args.port_id ]
                
                if args.in_vc_num > 0 :
                    port_cfg.in_vc_num = args.in_vc_num
                if args.out_vc_num > 0 :
                    port_cfg.out_vc_num = args.out_vc_num
                if args.in_buf_size > 0 :
                    port_cfg.in_buf_size = args.in_buf_size
                if args.out_buf_size > 0 :
                    port_cfg.out_buf_size = args.out_buf_size
                if args.axis >= 0 :
                    port_cfg.port_axis = args.axis
                if args.axid_dir in EsyNetCfgPort.__port_axis_dir_enum :
                    port_cfg.port_axis_dir = args.port_axis_dir
                if args.not_connect_ni :
                    port_cfg.network_interface = False
                elif args.connect_ni :
                    port_cfg.network_interface = True
            # Configure router
            else :
                if args.pipe_cycle >= 0 :
                    router_cfg.pipe_cycle = args.pipe_cycle
                if args.phy_port_num > 0 :
                    if args.phy_port_num <= len( router_cfg.port_list ) :
                        router_cfg.port_list = router_cfg.port_list[ 0: args.phy_port_num ]
                    else :
                        for i in range( len( router_cfg.port_list ), args.phy_port_num ) :
                            port_cfg = EsyNetCfg().createTemplatePort()
                            router_cfg.port_list.append( port_cfg )
        elif args.template_ni :
            ni_cfg = net_cfg.template_ni
            if args.ni_pipe_cycle >= 0 :
                ni_cfg.pipe_cycle = args.ni_pipe_cycle
            if args.ni_buf_size > 0 :
                ni_cfg.buffer_size = args.ni_buf_size
            if args.ni_interrupt_delay >= 0 :
                ni_cfg.ni_interrupt_delay = args.ni_interrupt_delay

        if args.update :
            net_cfg.updateNetwork() 
    #Configure router
    elif args.router_id >= 0 and args.router_id < len( net_cfg.router_list ) : 
        router_cfg = net_cfg.router_list[ args.router_id ]
        # Configure port
        if args.port_id >= 0 and args.port_id < len( router_cfg.port_list ) :
            port_cfg = router_cfg.port_list[ args.port_id ]
            if args.in_vc_num > 0 :
                port_cfg.in_vc_num = args.in_vc_num
            if args.out_vc_num > 0 :
                port_cfg.out_vc_num = args.out_vc_num
            if args.in_buf_size > 0 :
                port_cfg.in_buffer_size = args.in_buf_size
            if args.out_buf_size > 0 :
                port_cfg.out_buffer_size = args.out_buf_size
            if args.port_axis >= 0 :
                port_cfg.port_axis = args.port_axis
            if args.port_axis_dir in EsyNetCfgPort.__port_axis_dir_enum :
                port_cfg.port_axis_dir = args.port_axis_dir
            if args.not_connect_ni :
                port_cfg.network_interface = False
            elif args.connect_ni :
                port_cfg.network_interface = True
            else :
                if args.connect_router >= 0 and args.connect_router < len( net_cfg.router_list ) :
                    port_cfg.neighbor_router = args.connect_router
                if args.connect_port >= 0 and args.connect_port < len( net_cfg.router_list[ port_cfg.network_router ].port_list ) :
                    port_cfg.neighbor_port = args.connect_port
        # Configure router
        else :
            if args.pipe_cycle >= 0 :
                router_cfg.pipe_cycle = args.pipe_cycle
            if args.phy_port_num > 0 :
                if args.phy_port_num > 0 :
                    if args.phy_port_num <= len( router_cfg.port_list ) :
                        router_cfg.port_list = router_cfg.port_list[ 0: args.phy_port_num ]
                    else :
                        for i in range( len( router_cfg.port_list ), args.phy_port_num ) :
                            port_cfg = EsyNetCfg().createTemplatePort()
                            router_cfg.port_list.append( port_cfg )

        if args.update :
            net_cfg.updateNI()
    # Configure NI
    elif args.ni_id >= 0 and args.ni_id < len( net_cfg.ni_list ) :
        ni_cfg = net_cfg.ni_list[ args.ni_id ]
        if args.ni_pipe_cycle >= 0 :
            ni_cfg.pipe_cycle = args.ni_pipe_cycle
        if args.ni_buf_size > 0 :
            ni_cfg.buffer_size = args.ni_buf_size
        if args.ni_interrupt_delay >= 0 :
            ni_cfg.interrupt_delay = args.ni_interrupt_delay
        if args.ni_connect_router >= 0 and args.ni_connect_router < len( net_cfg.port_list ) :
            ni_cfg.connect_router = args.ni_connect_router
        if args.ni_connect_port >= 0 and args.ni_connect_port < len( net_cfg.port_list[ args.ni_connect_router ].port_ ) :
            ni_cfg.connect_router = args.ni_connect_router
        if args.ni_conn_port >= 0 and args.ni_conn_port < len( net_cfg.router_list[ ni_cfg.connectRouter() ].port_list ) :
            ni_cfg.connect_port = args.ni_conn_port
    else :
        assert False, 'Please specify the router/port/NI to modify.'

    print( str( net_cfg ) )
    
    net_cfg.writeXmlFile( args.file )

def optionParser() :

    # argument parser
    parser = argparse.ArgumentParser( prog='netcfgeditor.py', 
                                      description='Network Configuration Editor' )
#    argu.insertOpenFile( "-network_cfg_file_name", "Network configuration file name", &a_netcfg_file_name, "", 1, NETCFG_EXTENSION );
    subparsers = parser.add_subparsers( title='comamnds', 
                                        description='valid subcommands', 
                                        dest='command' )

    # subcommand: create
    parser_create = subparsers.add_parser( 'create',
                                           help='Create new network configuration file' )
    parser_create.add_argument( '--file',
                                help='Network configuration file name',
                                required=True )
    # topology group
    parser_create_topology_group = parser_create.add_argument_group( 'arguments to specify topology' )
    parser_create_topology_group.add_argument( '--topology',
                                               choices=netcfg.EsyNetCfg.topology_enum,
                                               help='Code for topology',
                                               default='Switch' )
    parser_create_topology_group.add_argument( '--network_size',
                                               help='Size of network in diamension',
                                               nargs='+',
                                               type=int,
                                               default=[1] )
    # router group
    parser_create_router_group = parser_create.add_argument_group( 'arguments to specify router configuration' )
    parser_create_router_group.add_argument( '--pipe_cycle', 
                                             help='Pipeline cycle of routers', 
                                             type=float, 
                                             default=1.0 )
    parser_create_router_group.add_argument( '--phy_number', 
                                             help='Number of physical port of a router', 
                                             type=int,
                                             default=5 )
    parser_create_router_group.add_argument( '--in_vc_number',
                                             help='Number of input virtual channel',
                                             type=int,
                                             default=1 )
    parser_create_router_group.add_argument( '--out_vc_number',
                                             help='Number of output virtual channel',
                                             type=int,
                                             default=1 )
    parser_create_router_group.add_argument( '--in_buffer_size',
                                             help='Size of input buffer of each VC',
                                             type=int,
                                             default=10 )
    parser_create_router_group.add_argument( '--out_buffer_size',
                                             help='Size of output buffer of each VC',
                                             type=int,
                                             default=10 )
    # NI group
    parser_create_ni_group = parser_create.add_argument_group( 'arguments to specify NI configuration' )
    parser_create_ni_group.add_argument( '--ni_pipe_cycle',
                                         help='Pipeline cycle of NIs',
                                         type=float,
                                         default=1.0 )
    parser_create_ni_group.add_argument( '--ni_buffer_size',
                                         help='Buffer size of NI',
                                         type=int,
                                         default=10 )
    parser_create_ni_group.add_argument( '--ni_interrupt_delay',
                                         help='Interruption delay of NI',
                                         type=float,
                                         default=100 )

    # subcommand: view
    parser_view = subparsers.add_parser( 'view',
                                         help='Print network configuration file' )
    parser_view.add_argument( '--file',
                              help='Network configuration file name',
                              required=True )

    # subcommand: modify
    parser_modify = subparsers.add_parser( 'modify',
                                           help='Modify network configuration file' )
    parser_modify.add_argument( '--file',
                                help='Network configuration file name',
                                required=True )
    parser_modify.add_argument( '--update',
                                help='Update network configuration',
                                action='store_true',
                                default=False )
    # select component
    parser_modify_select_group = parser_modify.add_argument_group( 'arguments to specify component to modify' )
    parser_modify_select_group.add_argument( '--router_id',
                                             help='ID of the router to modify',
                                             type=int,
                                             default=-1 )
    parser_modify_select_group.add_argument( '--template_router',
                                             help='Modify template router',
                                             action='store_true',
                                             default=False )
    parser_modify_select_group.add_argument( '--port_id',
                                             help='ID of the port to modify',
                                             type=int,
                                             default=-1 )
    parser_modify_select_group.add_argument( '--ni_id',
                                             help='ID of the NI to modify',
                                             type=int,
                                             default=-1 )
    parser_modify_select_group.add_argument( '--template_ni',
                                             help='Modify template NI',
                                             action='store_true',
                                             default=False )
    # router group
    parser_modify_router_group = parser_modify.add_argument_group( 'arguments to modify router configuration' )
    parser_modify_router_group.add_argument( '--pipe_cycle',
                                             help='Pipeline cycle of routers',
                                             type=float,
                                             default=1.0 )
    parser_modify_router_group.add_argument( '--phy_number',
                                             help='Number of physical port of a router',
                                             type=int,
                                             default=5 )
    # port group
    parser_modify_port_group = parser_modify.add_argument_group( 'arguments to modify port configuration' )
    parser_modify_port_group.add_argument( '--in_vc_number',
                                           help='Number of input virtual channel',
                                           type=int,
                                           default=1 )
    parser_modify_port_group.add_argument( '--out_vc_number',
                                           help='Number of output virtual channel',
                                           type=int,
                                           default=1 )
    parser_modify_port_group.add_argument( '--in_buffer_size',
                                           help='Size of input buffer of each VC',
                                           type=int,
                                           default=10 )
    parser_modify_port_group.add_argument( '--out_buffer_size',
                                           help='Size of output buffer of each VC',
                                           type=int,
                                           default=10 )
    parser_modify_port_group.add_argument( '--axis',
                                           help='Axis of the port',
                                           type=int,
                                           default=0 )
    parser_modify_port_group.add_argument( '--axis_dir',
                                           choices=['Up', 'Down'],
                                           help='Direction of axis',
                                           default='Up' )
    parser_modify_port_group.add_argument( '--connect_router',
                                           help='Connected router or NI',
                                           type=int,
                                           default=-1 )
    parser_modify_port_group.add_argument( '--connect_port',
                                           help='Connected port',
                                           type=int,
                                           default=-1 )
    parser_modify_port_group.add_argument( '--connect_ni',
                                           help='Connect to NI',
                                           action='store_true',
                                           default=False )
    parser_modify_port_group.add_argument( '--not_connect_ni',
                                           help='Not connect to NI',
                                           action='store_true',
                                           default=False )
    # NI group
    parser_modify_ni_group = parser_modify.add_argument_group( 'arguments to modify NI configuration' )
    parser_modify_ni_group.add_argument( '--ni_pipe_cycle',
                                         help='Pipeline cycle of NIs',
                                         type=float,
                                         default=1.0 )
    parser_modify_ni_group.add_argument( '--ni_buffer_size',
                                         help='Buffer size of NI',
                                         type=int,
                                         default=10 )
    parser_modify_ni_group.add_argument( '--ni_interrupt_delay',
                                         help='Interruption delay of NI',
                                         type=float,
                                         default=100 )
    parser_modify_ni_group.add_argument( '--ni_connect_router',
                                         help='Connected router of NI',
                                         type=int,
                                         default=-1 )
    parser_modify_ni_group.add_argument( '--ni_connect_port',
                                         help='Connected port of NI',
                                         type=int,
                                         default=-1 )

    args = parser.parse_args()
    if args.command not in [ 'create', 'view', 'modify' ] :
        parser.print_usage()
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
