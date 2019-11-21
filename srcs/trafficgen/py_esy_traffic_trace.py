#
# File name : py_esy_traffic_trace.py
# Function : Traffic 
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

import esy_traffic_pb2 as esy_traffic
import struct
import pandas as pd

def readTrafficTrace( file_name : str ) :
    trace = []

    file_obj = open( file_name, 'rb' )
    max_time_str = file_obj.read( int( 64 / 8 ) )
    max_time = struct.unpack( 'Q', max_time_str )[0]

    pac_bp = esy_traffic.NetworkPacket()
    while True:
        pac_size_str = file_obj.read( 1 )
        if not pac_size_str :
            break
        pac_size = struct.unpack( 'B', pac_size_str )[ 0 ]
        pac_str = file_obj.read( pac_size )
        if not pac_str :
            break

        if pac_bp.ParseFromString( pac_str ) :
            trace.append( [ pac_bp.time,
                            pac_bp.src_ni,
                            pac_bp.dst_ni,
                            pac_bp.length] )
        else :
            break

    file_obj.close()

    return max_time, trace

def readTrafficTraceToDataFrame( file_name : str ):
    max_time, trace = readTrafficTrace( file_name )
    df = pd.DataFrame( trace, columns=['Time', 'Src', 'Dst', 'Length'] )
    return max_time, df

def trafficStatistic( trace : list,
                      ni_count : int, 
                      max_time : int,
                      src = -1,
                      dst = -1):
    # initialize data buffer
    total_flit = 0
    # anaysis data
    for pac_time, pac_src, pac_dst, pac_length in trace :
        if ( src == -1 or ( src >= 0 and pac_src == src ) ) or \
            ( dst == -1 or ( dst >= 0 and pac_dst == dst ) ) :
            total_flit += pac_length

    res_dict = {}
    res_dict[ 'total_flit'     ] = total_flit
    res_dict[ 'total_fir'      ] = total_flit / max_time

    return res_dict

def trafficVsNode( trace : list,
                   ni_count : int, 
                   src = -1,
                   dst = -1):
    # initialize data buffer
    node_series = [ i for i in range( 0, ni_count ) ]
    src_vs_node = [ 0 for i in range( 0, ni_count ) ]
    dst_vs_node = [ 0 for i in range( 0, ni_count ) ]
    # anaysis data
    for pac_time, pac_src, pac_dst, pac_length in trace :
        if ( src == -1 or ( src >= 0 and pac_src == src ) ) or \
            ( dst == -1 or ( dst >= 0 and pac_dst == dst ) ) :
            src_vs_node[ pac_src ] += pac_length
            dst_vs_node[ pac_dst ] += pac_length

    return node_series, src_vs_node, dst_vs_node

def trafficVsTime( trace : list,
                   max_time : int,
                   src = -1,
                   dst = -1 ):
    # initialize data buffer
    time_series = [ i for i in range( 0, max_time + 1 ) ]
    src_vs_time = [ 0 for i in range( 0, max_time + 1 ) ]
    dst_vs_time = [ 0 for i in range( 0, max_time + 1 ) ]
    # anaysis data
    for pac_time, pac_src, pac_dst, pac_length in trace :
        if ( src == -1 or ( src >= 0 and pac_src == src ) ) or \
            ( dst == -1 or ( dst >= 0 and pac_dst == dst ) ) :
            src_vs_time[ pac_time ] += pac_length
            dst_vs_time[ pac_time ] += pac_length

    return time_series, src_vs_time, dst_vs_time
