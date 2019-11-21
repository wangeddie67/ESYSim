#
# File name : py_esy_traffic_gen.py
# Function : Python wrapper of traffic generator.
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

from typing import List
import argparse
from esy_packetgen import EsyPacketGeneratorFun, \
                          EsyPacketGenerator, \
                          EsyNetworkPacket

profile_dict = { "Uniform"     : EsyPacketGeneratorFun.TP_UNIFORM,
                 "Transpose1"  : EsyPacketGeneratorFun.TP_TRANSPOSE1,
                 "Transpose2"  : EsyPacketGeneratorFun.TP_TRANSPOSE2,
                 "Bitreversal" : EsyPacketGeneratorFun.TP_BITREVERSAL,
                 "Butterfly"   : EsyPacketGeneratorFun.TP_BUTTERFLY,
                 "Shuffle"     : EsyPacketGeneratorFun.TP_SHUFFLE,
                 "Trace"       : EsyPacketGeneratorFun.TP_TRACE, }

def addOptions( parser : argparse.ArgumentParser ) :
    argument_group = parser.add_argument_group(
        description='Traffic generator' )

    argument_group.add_argument( '--traffic_profile',
        help='Traffic profile',
        choices=profile_dict.keys(),
        default="Uniform" )
    argument_group.add_argument( '--traffic_pir',
        help='Packet injection rate (packet/cycle/node)',
        type=float,
        default=0.0 )
    argument_group.add_argument( '--traffic_packet_size',
        help='Number of flits in a packet',
        type=int,
        default=5 )

    argument_group.add_argument( '--traffic_trace_file',
        help='File name to trace packets '
             '(only valid when traffic profile is \'trace\')',
        type=str,
        default='' )

def str2ProfileEnum( string : str) :
    return profile_dict[ string ]

def buildTrafficGenerator( args, network_cfg, rand_obj ) -> EsyPacketGenerator :
    # create traffic generator
    traffic_profile = profile_dict[ args.traffic_profile ]
    traffic_gen = EsyPacketGenerator( network_cfg.niCount(),
                                     network_cfg.size(),
                                     traffic_profile, 
                                     args.traffic_pir,
                                     args.traffic_packet_size,
                                     -1,
                                     rand_obj )

    if args.traffic_profile == 'Trace' :
        assert args.traffic_trace_file != '', \
            'File name must be specified if traffic profile is Trace.'
        traffic_gen.openReadFile( args.traffic_trace_file )
    else :
        if args.traffic_trace_file != '' :
            traffic_gen.openWriteFile( args.traffic_trace_file )

    return traffic_gen

def closeTrafficGenerator( traffic_gen : EsyPacketGenerator ) :
    traffic_gen.closeReadFile()
    traffic_gen.closeWriteFile()

