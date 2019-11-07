#!/usr/bin/env python3

# File name : view_cfg_file.py
# Function : Print network configuration file.
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
        prog='view_cfg_file.py', 
        description='View Network Configuration File' )

    parser.add_argument( '--file', '-f',
        help='Network configuration file name',
        required=True )
    parser.add_argument( '--raw_format',
        help='Print network configuration structure in raw format. '
             'By default, print network configuration in the center of router',
        action='store_true',
        default=False )

    args = parser.parse_args()
    return args

# Main function
if __name__ == "__main__" :
    # parse option
    args = optionParser()

    # print function
    print( 'Print network configuration file {0}.'.format( args.file ) )

    net_cfg = syscfg.readNetCfgJson( args.file )
    print( syscfg.printNetCfg( net_cfg, raw_format=args.raw_format ) )

