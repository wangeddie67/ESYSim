#!/usr/bin/env python3

# File name : create_irregular.py
# Function : Create network configuration file for irregular topology, 
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
import re

sys.path.append( '../../build' )
import syscfg


def optionParser() :
    # argument parser
    parser = argparse.ArgumentParser(
        prog='create_irregular.py', 
        description='Create Network Configuration File '
                    'according to Topology File' )

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
    parser_topology_group.add_argument( '--topology',
        help='Topology string. '
             'First value is the number of node. '
             'Second value is the number of NI. '
             'Then, every two values specify one link with the source node and '
             'the destination node. '
             'Node is presented by \'R<id>\', for example, R0, R1, R2, etc.. '
             'NI is presented by \'N<id>\', for example, N0, N1, N2, etc..',
        nargs='+',
        type=str,
        default=[] )
    parser_topology_group.add_argument( '--topology_file',
        help='Topology file. '
             'First line is the number of node. '
             'Second line is the number of NI. '
             'Then, each line specifies one link with the source node and the '
             'destination node. '
             'Source and destination nodes are split by space, '
             'for example, \'R1 R2\\n\'. '
             'Node is presented by \'R<id>\', for example, R0, R1, R2, etc.. '
             'NI is presented by \'N<id>\', for example, N0, N1, N2, etc..',
        type=str,
        default='' )
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
    assert args.topology_file != '' or args.topology != [], \
        'Wrong argument: ' \
        'Either topology or topology file is necessary.'
    if args.topology != [] :
        assert len( args.topology ) >= 4 and len( args.topology ) % 2 == 0, \
            'Wrong argument (--topology) :' \
            'Topology format error. ' \
            'First value is the number of node. ' \
            'Second value is the number of NI. ' \
            'Then, every two values specify one link with the source node and ' \
            'the destination node. ' \
            'Node is presented by \'R<id>\', for example, R0, R1, R2, etc.. ' \
            'NI is presented by \'N<id>\', for example, N0, N1, N2, etc..'

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

    # regax
    link_re_pattern = re.compile( '(N|R)(\d+)\s+(N|R)(\d+)' )

    links = []
    if args.topology :
        node_count = int( args.topology[ 0 ] )
        ni_count = int( args.topology[ 1 ] )
        assert node_count > 0, 'Network must have at least 1 node'
        assert ni_count > 0, 'Network must have at least 1 NI'

        ni_check = [ False for i in range( 0, ni_count ) ]
        for i in range( 2, len( args.topology ), 2 ) :
            src_str = args.topology[ i ]
            dst_str = args.topology[ i + 1 ]
            src_is_ni = src_str[ 0 ] == 'N'
            src_id = int( src_str[ 1: ] )
            dst_is_ni = dst_str[ 0 ] == 'N'
            dst_id = int( dst_str[ 1: ] )

            if src_is_ni :
                assert src_id < ni_count, \
                    'Invalid source node : {0}'.format( src_str )
                assert not ni_check[ src_id ], \
                    'NI can only have one connection : {0}'.format( src_str )
                ni_check[ src_id ] = True
            else :
                assert src_id < node_count, \
                    'Invalid source node : {0}'.format( src_str )
            if dst_is_ni :
                assert dst_id < ni_count, \
                    'Invalid destination node : {0}'.format( dst_str )
                assert not ni_check[ dst_id ], \
                    'NI can only have one connection : {0}'.format( dst_str )
                ni_check[ dst_id ] = True
            else :
                assert dst_id < node_count, \
                    'Invalid destination node : {0}'.format( dst_str )
            assert not (src_is_ni and dst_is_ni), \
                'NI cannot connect with NI : {0}-{1}'.format( src_str, dst_str )

            links.append( [ src_id, src_is_ni, dst_id, dst_is_ni ] )
    elif args.topology_file != '' :
        file_obj = open( args.topology_file )
        node_count = int( file_obj.readline() )
        ni_count = int( file_obj.readline() )
        assert node_count > 0, 'Network must have at least 1 node'
        assert ni_count > 0, 'Network must have at least 1 NI'

        ni_check = [ False for i in range( 0, ni_count ) ]
        for line in file_obj :
            searchObj = re.search( link_re_pattern, line )
            if searchObj :
                src_str = searchObj.group( 1 ) + searchObj.group( 2 )
                dst_str = searchObj.group( 3 ) + searchObj.group( 4 )
                src_is_ni = searchObj.group( 1 ) == 'N'
                src_id = int( searchObj.group( 2 ) )
                dst_is_ni = searchObj.group( 3 ) == 'N'
                dst_id = int( searchObj.group( 4 ) )

                if src_is_ni :
                    assert src_id < ni_count, \
                        'Invalid source node : {0}'.format( src_str )
                    assert not ni_check[ src_id ], \
                        'NI can only have one connection : {0}'.format( src_str )
                    ni_check[ src_id ] = True
                else :
                    assert src_id < node_count, \
                        'Invalid source node : {0}'.format( src_str )
                if dst_is_ni :
                    assert dst_id < ni_count, \
                        'Invalid destination node : {0}'.format( dst_str )
                    assert not ni_check[ dst_id ], \
                        'NI can only have one connection : {0}'.format( dst_str )
                    ni_check[ dst_id ] = True
                else :
                    assert dst_id < node_count, \
                        'Invalid destination node : {0}'.format( dst_str )
                assert not (src_is_ni and dst_is_ni), \
                    'NI cannot connect with NI : {0}-{1}'.format( src_str, dst_str )

                links.append( [ src_id, src_is_ni, dst_id, dst_is_ni ] )

        file_obj.close()

    assert len( links ) > 0, 'Network has at least 1 link.'

    # print configuration
    if args.verbose == 1 :
        # print configuration
        print( '  topology = Irregular' )
        print( '  node_count = {0}'.format( node_count ) )
        print( '  ni_count   = {0}'.format( ni_count ) )
        print( '  links : ' )
        for src_id, src_is_ni, dst_id, dst_is_ni in links :
            link_str = ''
            if src_is_ni :
                link_str += 'N'
            else :
                link_str += 'R'
            link_str += str( src_id )
            link_str += ' -> '
            if dst_is_ni :
                link_str += 'N'
            else :
                link_str += 'R'
            link_str += str( dst_id )
            print( '    ' + link_str )

    # Create new network configuration
    network_cfg = syscfg.createNetCfgIrregular(
        node_size=node_count,
        ni_size=ni_count,
        links=links,
        out_bufs=args.out_bufs,
        in_bufs=args.in_bufs,
        out_period=args.out_period,
        in_period=args.in_period,
        ni_int_delay=args.ni_int_delay )

    syscfg.writeNetCfgJson( args.file, network_cfg )
    if args.verbose > 1 :
        print( syscfg.printNetCfg( network_cfg, raw_format=False ) )
