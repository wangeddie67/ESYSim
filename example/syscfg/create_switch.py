#!/usr/bin/env python3

# File name : create_switch.py
# Function : Create network configuration file for switch topology.
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
        prog='create_switch.py', 
        description='Create Network Configuration File for Switch Topology' )

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
    parser_topology_group.add_argument( '--ni_port', 
        help='Number of ports connects with NI',
        type=int,
        default=1 )
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
    assert args.ni_port > 0, \
        'Wrong argument (--ni_port): '\
        'Each node should connect with at least one NI.'

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

    # print configuration
    if args.verbose == 1 :
        print( '  topology = Switch' )
        print( '  ni_port  = {0}'.format( args.ni_port ) )

    # Create new network configuration
    network_cfg = syscfg.createNetCfgSwitch(
        ni_port=args.ni_port,
        out_bufs=args.out_bufs,
        in_bufs=args.in_bufs,
        out_period=args.out_period,
        in_period=args.in_period,
        ni_int_delay=args.ni_int_delay )

    syscfg.writeNetCfgJson( args.file, network_cfg )
    if args.verbose > 1 :
        print( syscfg.printNetCfg( network_cfg, raw_format=False ) )
