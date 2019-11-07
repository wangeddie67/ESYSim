#!/usr/bin/env python3

# File name : create_tree.py
# Function : Create network configuration file for tree topology, 
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
# Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>

import sys
import argparse
from typing import List

sys.path.append( '../../build' )
import syscfg


def optionParser() :
    # argument parser
    parser = argparse.ArgumentParser(
        prog='create_tree.py', 
        description='Create Network Configuration File for Tree Topology' )

    parser.add_argument( '--file', '-f',
        help='Network configuration file name',
        required=True )
    parser.add_argument( '--verbose', '-v',
        help='Print information',
        type=int,
        default=0 )
    # topology group
    parser_topology_group = parser.add_argument_group(
        'arguments to specify topology' )
    parser_topology_group.add_argument( '--network_size',
        help='Size of network in diamension',
        nargs='+',
        type=int,
        default=[1] )
    parser_topology_group.add_argument( '--ni_port', 
        help='Number of ports connects with NI',
        type=int,
        default=1 )
    parser_topology_group.add_argument( '--axis_port', 
        help='Number of ports on each axis. '
             'The number of elements should be as same as '
             'the number of elements of option `--network_size`.',
        nargs='+',
        type=int,
        default=[1] )
    # link group
    parser_link_group = parser.add_argument_group( 
        'arguments to specify link configuration' )
    parser_link_group.add_argument( '--out_bufs',
        help='Size of buffers for virtual channels in the output port. '
             'The number of elements presents the number of virtual channels.',
        nargs='+',
        type=int,
        default=[10] )
    parser_link_group.add_argument( '--in_bufs',
        help='Size of buffers for virtual channels in the input port. '
             'The number of elements presents the number of virtual channels.',
        nargs='+',
        type=int,
        default=[10] )
    parser_link_group.add_argument( '--out_period', 
        help='Pipeline cycle at the output port.'
             'Flit leaves the output port by this period', 
        type=float, 
        default=1.0 )
    parser_link_group.add_argument( '--in_period', 
        help='Pipeline cycle at the input port.'
             'Flit leaves the input port by this period', 
        type=float, 
        default=1.0 )
    parser_link_group.add_argument( '--ni_int_delay',
        help='Delay to response NI interrupt',
        type=float,
        default=10.0 )

    args = parser.parse_args()
    return args

# Main function
if __name__ == "__main__" :
    # parse option
    args = optionParser()

    print( 'Create new network configuration file.' )

    # argument check
    for axis_size in args.network_size :
        assert axis_size > 0, \
            'Wrong argument (--network_size): ' \
            'Value should be positive integar (>0).'
    assert args.ni_port > 0, \
        'Wrong argument (--ni_port): '\
        'Each router should connect with at least one NI.'
    assert len( args.axis_port ) == len( args.network_size ), \
        'Wrong argument (--axis_port): ' \
        'Port on all axises ({:1}) should be specified.' \
            .format( len( args.network_size ) )
    for axis_port in args.axis_port :
        assert axis_port >= 0, \
            'Wrong argument (--axis_port): ' \
            'Value should be non-negative integar (>=0).'

    for buf in args.out_bufs :
        assert buf > 0, \
            'Wrong argument (--out_bufs): ' \
            'Value should be positive integar (>0).'
    for buf in args.in_bufs :
        assert buf > 0, \
            'Wrong argument (--in_bufs): ' \
            'Value should be positive integar (>0).'
    assert args.out_period > 0.0, \
        'Wrong argument (--out_period): ' \
        'Value should be positive float number (>0.0).'
    assert args.in_period > 0.0, \
        'Wrong argument (--in_period): ' \
        'Value should be positive float number (>0.0).'
    assert args.ni_int_delay > 0.0, \
        'Wrong argument (--ni_int_delay): ' \
        'Value should be positive float number (>0.0).'

    topology = topology_dict[ args.topology ]

    # print configuration
    if args.verbose == 1 :
        # print configuration
        if topology in [ esy_syscfg.EsyNetCfg.NT_MESH_2D, \
            esy_syscfg.EsyNetCfg.NT_MESH_DIA ] :
            print( '  topology     = Mesh' )
        elif topology in [ esy_syscfg.EsyNetCfg.NT_RING, \
            esy_syscfg.EsyNetCfg.NT_TORUS_2D, \
            esy_syscfg.EsyNetCfg.NT_TORUS_DIA ] :
            print( '  topology     = Torus' )
        print( '  network_size = {0}'.format( args.network_size ) )
        print( '  ni_port      = {0}'.format( args.ni_port ) )
        print( '  axis_port    = {0}'.format( args.axis_port ) )

    # Create new network configuration
    network_cfg = syscfg.createNetCfgTorus(
        topology=topology,
        network_size=args.network_size,
        ni_port=args.ni_port,
        axis_port=args.axis_port,
        out_bufs=args.out_bufs,
        in_bufs=args.in_bufs,
        out_period=args.out_period,
        in_period=args.in_period,
        ni_int_delay=args.ni_int_delay )

    syscfg.writeNetCfgJson( args.file, network_cfg )
    if args.verbose > 1 :
        print( syscfg.printNetCfg( network_cfg, raw_format=False ) )
