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

sys.path.append( '../' )
import syscfg
import randgen
import trafficgen


def optionParser() :
    # argument parser
    parser = argparse.ArgumentParser(
        prog='esynet_main.py', 
        description='Esynet Network-on-Chip Simulator' )

    syscfg.addNetworkCfgOptions( parser )
    randgen.addOptions( parser )
    trafficgen.addOptions( parser )

    parser.add_argument( '--log_file', '-l',
        help='Log file name',
        type=str,
        default='' )

    args = parser.parse_args()
    return args

def printLog( string : str, log_file_obj = None ) :
    sys.stdout.write( string + '\n' )
    if log_file_obj :
        log_file_obj.write( string + '\n' )

def printArgument( args, log_file_obj = None ) :
    length = 0
    for a, b in args._get_kwargs() :
        if len( a ) > length :
            length = len( a )

    format_str = '  {{:{0}}} : {{}}'.format( length )

    printLog( 'Run arguments :', log_file_obj )
    for a, b in args._get_kwargs() :
        printLog( format_str.format( a, b ), log_file_obj )

# Main function
if __name__ == "__main__" :
    # parse option
    args = optionParser()

    # open log file
    log_file_obj = None
    if args.log_file != '':
        log_file_obj = open( args.log_file, 'w' )

    # print head
    printLog( 'Esynet Network-on-Chip Simulator.', log_file_obj )
    printLog( '', log_file_obj )
    printArgument( args, log_file_obj )
    printLog( '', log_file_obj )

    # network configuration
    printLog( 'Network Configuration :', log_file_obj )
    network_cfg = syscfg.readNetCfgJson( args.network_cfg_file )
    network_cfg_str = syscfg.printNetCfg(
        network_cfg, indent='  ', raw_format=False )
    printLog( network_cfg_str, log_file_obj )
    printLog( '', log_file_obj )

    # create options
    rand_obj = randgen.EsyRandGen( args.random_seed )

    # create traffic generator
    traffic_gen = trafficgen.buildTrafficGenerator(
        args, network_cfg, rand_obj )

    for i in range( 0, 100 ) :
        pac_list = traffic_gen.generatePacket( i )
        for pac in pac_list :
            print( pac.time(), pac.srcNi(), pac.dstNi(), pac.length() )

    trafficgen.closeTrafficGenerator( traffic_gen )

    # close log file
    if log_file_obj:
        log_file_obj.close()
