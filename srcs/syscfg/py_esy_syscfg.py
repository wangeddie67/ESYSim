#
# File name : py_esy_syscfg.py
# Function : Json interface of system configuration structure.
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
import esy_syscfg
import json
import argparse
from tabulate import tabulate

def addNetworkCfgOptions( parser : argparse.ArgumentParser ) :
    argument_group = parser.add_argument_group(
        description='Network configuration' )

    argument_group.add_argument( '--network_cfg_file', '-n',
        help='Network configuration file name',
        type=str,
        required=True )


def createNetCfgSwitch( ni_port : int,
                        out_bufs : List[int],
                        in_bufs : List[int],
                        out_period : float,
                        in_period : float,
                        ni_int_delay : float ) -> esy_syscfg.EsyNetCfg :
    # Check input arguments
    assert ni_port > 0, \
        'Switch should have at least one ports.'

    # create switch topology
    links = []
    for link_id in range( 0, ni_port ) :
        # link configuration to NI link
        to_ni_link_cfg = esy_syscfg.EsyNetCfgLink(
            esy_syscfg.EsyNetCfgLink.LINKDIR_SW_NE,
            -1, esy_syscfg.EsyNetCfgLink.LINKAXISDIR_DOWN,
            esy_syscfg.EsyNetCfgPort(
                0, link_id, False, out_bufs, out_period ),
            esy_syscfg.EsyNetCfgPort(
                link_id, 0, True, in_bufs, in_period ),
            ni_int_delay )
        links.append( to_ni_link_cfg )
        # link configuration from NI link
        from_ni_link_cfg = esy_syscfg.EsyNetCfgLink(
            esy_syscfg.EsyNetCfgLink.LINKDIR_NE_SW,
            -1, esy_syscfg.EsyNetCfgLink.LINKAXISDIR_UP,
            esy_syscfg.EsyNetCfgPort(
                link_id, 0, True, out_bufs, out_period ),
            esy_syscfg.EsyNetCfgPort(
                0, link_id, False, in_bufs, in_period ),
            ni_int_delay )
        links.append( from_ni_link_cfg )

    # create network configuration
    network_cfg = esy_syscfg.EsyNetCfg(
        esy_syscfg.EsyNetCfg.NT_SWITCH,
        [1], 1, ni_port, links )
    return network_cfg

def createNetCfgTorus( topology : int,
                       network_size : List[float],
                       ni_port : int,
                       axis_port : List[int],
                       out_bufs : List[int],
                       in_bufs : List[int],
                       out_period : float,
                       in_period : float,
                       ni_int_delay : float ) -> esy_syscfg.EsyNetCfg :
    # Check input arguments
    assert topology in [ esy_syscfg.EsyNetCfg.NT_RING, \
                         esy_syscfg.EsyNetCfg.NT_MESH_2D, \
                         esy_syscfg.EsyNetCfg.NT_MESH_DIA, \
                         esy_syscfg.EsyNetCfg.NT_TORUS_2D, \
                         esy_syscfg.EsyNetCfg.NT_TORUS_DIA ], \
        'This function can only create topology: mesh, torus.'
    assert len( network_size ) == len( axis_port ), \
        'The port on each axis should be speicifed.'
    assert ni_port > 0, \
        'Switch should have at least one ports.'

    # create mesh/torus topology
    temp_links = []
    port_id = 0
    # port to NI
    for link_id in range( 0, ni_port ) :
        # link configuration to NI link
        to_link_cfg = esy_syscfg.EsyNetCfgLink(
            esy_syscfg.EsyNetCfgLink.LINKDIR_SW_NE,
            -1, esy_syscfg.EsyNetCfgLink.LINKAXISDIR_DOWN,
            esy_syscfg.EsyNetCfgPort(
                0, port_id, False, out_bufs, out_period ),
            esy_syscfg.EsyNetCfgPort(
                port_id, 0, True, in_bufs, in_period ),
            ni_int_delay )
        temp_links.append( to_link_cfg )
        port_id += 1
    # port to axis
    for axis_id in range( 0, len( axis_port ) ) :
        axis_port_num = axis_port[ axis_id ]
        for link_id in range( 0, axis_port_num ) :
            # link configuration to another node
            to_link_cfg = esy_syscfg.EsyNetCfgLink(
                esy_syscfg.EsyNetCfgLink.LINKDIR_SW_NE,
                axis_id, esy_syscfg.EsyNetCfgLink.LINKAXISDIR_DOWN,
                esy_syscfg.EsyNetCfgPort(
                0, port_id, False, out_bufs, out_period ),
                esy_syscfg.EsyNetCfgPort(
                0, port_id, False, in_bufs, in_period ),
                ni_int_delay )
            temp_links.append( to_link_cfg )
            # link configuration to another node
            to_link_cfg = esy_syscfg.EsyNetCfgLink(
                esy_syscfg.EsyNetCfgLink.LINKDIR_SW_NE,
                axis_id, esy_syscfg.EsyNetCfgLink.LINKAXISDIR_UP,
                esy_syscfg.EsyNetCfgPort(
                0, port_id, False, out_bufs, out_period ),
                esy_syscfg.EsyNetCfgPort(
                0, port_id, False, in_bufs, in_period ),
                ni_int_delay )
            temp_links.append( to_link_cfg )
            port_id += 1

    # create network configuration
    network_cfg = esy_syscfg.EsyNetCfg( topology, network_size, temp_links )
    return network_cfg

def createNetCfgIrregular( node_size : int,
                           ni_size : int,
                           links : List,
                           out_bufs : List[int],
                           in_bufs : List[int],
                           out_period : float,
                           in_period : float,
                           ni_int_delay : float ) -> esy_syscfg.EsyNetCfg :
    # Check input arguments
    assert node_size > 0, \
        'Network has at least 1 node.'
    assert ni_size > 0, \
        'Network has at least 1 NI.'
    assert len( links ) > 0, \
        'Network has at least 1 link.'

    # create irregular topology
    node_port = [ 0 for i in range(0, node_size) ]
    link_cfg_list = []
    for src_id, src_is_ni, dst_id, dst_is_ni in links :
        if src_is_ni :
            src_port = 0
        else :
            src_port = node_port[ src_id ]
            node_port[ src_id ] += 1
        if dst_is_ni :
            dst_port = 0
        else :
            dst_port = node_port[ dst_id ]
            node_port[ dst_id ] += 1

        # link configuration to another node
        to_link_cfg = esy_syscfg.EsyNetCfgLink(
            esy_syscfg.EsyNetCfgLink.LINKDIR_SW_NE,
            -1, esy_syscfg.EsyNetCfgLink.LINKAXISDIR_DOWN,
            esy_syscfg.EsyNetCfgPort(
                src_id, src_port, src_is_ni, out_bufs, out_period ),
            esy_syscfg.EsyNetCfgPort(
                dst_id, dst_port, dst_is_ni, in_bufs, in_period ),
            ni_int_delay )
        link_cfg_list.append( to_link_cfg )
        # link configuration to another node
        to_link_cfg = esy_syscfg.EsyNetCfgLink(
            esy_syscfg.EsyNetCfgLink.LINKDIR_SW_NE,
            -1, esy_syscfg.EsyNetCfgLink.LINKAXISDIR_UP,
            esy_syscfg.EsyNetCfgPort(
                dst_id, dst_port, dst_is_ni, out_bufs, out_period ),
            esy_syscfg.EsyNetCfgPort(
                src_id, src_port, src_is_ni, in_bufs, in_period ),
            ni_int_delay )
        link_cfg_list.append( to_link_cfg )

    # create network configuration
    network_cfg = esy_syscfg.EsyNetCfg(
        esy_syscfg.EsyNetCfg.NT_IRREGULAR, [ node_size ],
        node_size, ni_size, link_cfg_list )
    return network_cfg


topology_dict = { esy_syscfg.EsyNetCfg.NT_SWITCH    : "Switch",
                  esy_syscfg.EsyNetCfg.NT_RING      : "Ring",
                  esy_syscfg.EsyNetCfg.NT_MESH_2D   : "2D-Mesh",
                  esy_syscfg.EsyNetCfg.NT_TORUS_2D  : "2D-Torus",
                  esy_syscfg.EsyNetCfg.NT_MESH_DIA  : "Dia-Mesh",
                  esy_syscfg.EsyNetCfg.NT_TORUS_DIA : "Dia-Torus",
                  esy_syscfg.EsyNetCfg.NT_IRREGULAR : "Irregular" }

linkdir_dict = { esy_syscfg.EsyNetCfgLink.LINKDIR_E_W   : "E ->W ",
                 esy_syscfg.EsyNetCfgLink.LINKDIR_N_S   : "N ->S ",
                 esy_syscfg.EsyNetCfgLink.LINKDIR_NE_SW : "NE->SW",
                 esy_syscfg.EsyNetCfgLink.LINKDIR_NW_SE : "NW->SE",
                 esy_syscfg.EsyNetCfgLink.LINKDIR_S_N   : "S ->N ",
                 esy_syscfg.EsyNetCfgLink.LINKDIR_SE_NW : "SE->NW",
                 esy_syscfg.EsyNetCfgLink.LINKDIR_SW_NE : "SW->NE",
                 esy_syscfg.EsyNetCfgLink.LINKDIR_W_E   : "W ->E " }

linkaxisdir_dict = { esy_syscfg.EsyNetCfgLink.LINKAXISDIR_DOWN : "DOWN",
                     esy_syscfg.EsyNetCfgLink.LINKAXISDIR_UP   : "UP  " }

def printNetCfg( net_cfg : esy_syscfg.EsyNetCfg, 
                 raw_format=True,
                 indent='' ) -> str :

    from functools import reduce

    # Get the fields in the link table
    link_field_array = []
    for link_cfg in net_cfg.links() :
        # output port :R or NI
        if link_cfg.outputPort().isNi() :
            format_str = 'NI.{:d}'
        else :
            format_str = ' R.{:d}.{:d}'
        outport_str = format_str.format( link_cfg.outputPort().node(),
                                         link_cfg.outputPort().port() )

        # buffer of output virtual channel
        outbuf_str = str( link_cfg.outputPort().bufs() )

        # input port :R or NI
        if link_cfg.inputPort().isNi() :
            format_str = 'NI.{:d}'
        else :
            format_str = ' R.{:d}.{:d}'
        inport_str = format_str.format( link_cfg.inputPort().node(),
                                        link_cfg.inputPort().port() )

        # buffer of input virtual channel
        inbuf_str = str( link_cfg.inputPort().bufs() )

        # link direction
        dir_str = linkdir_dict[ link_cfg.linkDir() ]

        # link axis
        if link_cfg.linkAxis() >= 0 :
            axis_str = '{:d}.{:s}'.format( link_cfg.linkAxis(),
                linkaxisdir_dict[ link_cfg.linkAxisDir() ] )
        else :
            axis_str = 'L.{:s}'.format(
                linkaxisdir_dict[ link_cfg.linkAxisDir() ] )

        # frequency
        freq_str = ' {:8f} {:8f}'.format(
            link_cfg.outputPort().outPeriod(),
            link_cfg.inputPort().outPeriod() )

        # NI interrupt delay
        if link_cfg.inputPort().isNi() :
            niint_str = str( link_cfg.niIntDelay() )
        else :
            niint_str = ''

        link_field_array.append( [ link_cfg,
                                   [ outport_str, outbuf_str,
                                     inport_str, inbuf_str,
                                     dir_str, axis_str, freq_str,
                                     niint_str ] ] )

    head_str_list = [ 'Out port', 'Out Bufs', 'In port', 'In Bufs',
                      'Dir', 'Axis', 'Frequency', 'NI Int' ]

    pstr = ''
    # link table in raw format
    if raw_format:
        pstr += indent + 'topology={:1}\n' \
            .format( topology_dict[ net_cfg.topology() ] )
        pstr += indent + 'size={:1}\n'.format( str(net_cfg.size() ) )
        pstr += '\n'
        pstr += indent + 'link list :\n'

        table = map( lambda x: x[1], link_field_array )
        table_lines = \
            tabulate( table, headers=head_str_list, tablefmt='presto' )
        for line in table_lines.split( '\n' ) :
            pstr += indent * 2 + line + '\n'

    # link table in node format
    else :
        pstr += indent + 'topology={0}\n' \
            .format( topology_dict[ net_cfg.topology() ] )
        pstr += indent + 'size={0}\n'.format( str(net_cfg.size() ) )
        pstr += indent + 'node_count={0}\n'.format( net_cfg.nodeCount() )
        pstr += indent + 'ni_count={0}\n'.format( net_cfg.niCount() )
        pstr += '\n'
        pstr += indent + 'node list :\n'

        for node_id in range( 0, net_cfg.nodeCount() ) :
            pstr += indent + 'node {0}\n'.format( node_id )
            output_links = filter(
                lambda x : x[0].outputPort().isNi() == False and \
                    x[0].outputPort().node() == node_id, link_field_array )
            table = map( lambda x: x[1], output_links )
            table_lines = \
                tabulate( table, headers=head_str_list, tablefmt='presto' )
            for line in table_lines.split( '\n' ) :
                pstr += indent * 2 + line + '\n'
    return pstr

__json_tag_topology = 'topology'
__json_tag_size     = 'size'
__json_tag_links    = 'links'

__json_tag_link_dir          = 'dir'
__json_tag_link_axis         = 'axis'
__json_tag_link_axis_dir     = 'axis_dir'
__json_tag_link_input_port   = 'input_port'
__json_tag_link_output_port  = 'output_port'
__json_tag_link_ni_int_delay = 'ni_int_delay'

__json_tag_link_port_id        = 'id'
__json_tag_link_port_port      = 'port'
__json_tag_link_port_ni        = 'ni'
__json_tag_link_port_bufs      = 'bufs'
__json_tag_link_port_outperiod = 'outperiod'

__netcfg_extension = '.netcfg'

def jsonDictToNetCfgPort( portcfg_dict : dict ) -> esy_syscfg.EsyNetCfgPort :
    portcfg = esy_syscfg.EsyNetCfgPort(
        portcfg_dict[ __json_tag_link_port_id ],
        portcfg_dict[ __json_tag_link_port_port ],
        portcfg_dict[ __json_tag_link_port_ni ],
        portcfg_dict[ __json_tag_link_port_bufs ],
        portcfg_dict[ __json_tag_link_port_outperiod ] )
    return portcfg

def jsonDictToNetCfgLink( linkcfg_dict : dict ) -> esy_syscfg.EsyNetCfgLink :
    t_input_port  = jsonDictToNetCfgPort(
        linkcfg_dict[ __json_tag_link_input_port ] )
    t_output_port = jsonDictToNetCfgPort(
        linkcfg_dict[ __json_tag_link_output_port ] )

    linkcfg = esy_syscfg.EsyNetCfgLink(
        linkcfg_dict[ __json_tag_link_dir ],
        linkcfg_dict[ __json_tag_link_axis ],
        linkcfg_dict[ __json_tag_link_axis_dir ],
        t_output_port,
        t_input_port,
        linkcfg_dict[ __json_tag_link_ni_int_delay ] )
    return linkcfg

def jsonDictToNetCfg( networkcfg_dict : dict ) -> esy_syscfg.EsyNetCfg :
    link_dict_list = networkcfg_dict[ __json_tag_links ]
    linkcfg_list = []
    max_node_id = 0
    max_ni_id = 0
    for link_dict in link_dict_list :
        linkcfg = jsonDictToNetCfgLink( link_dict )
        linkcfg_list.append( linkcfg )

        if linkcfg.inputPort().isNi() :
            if linkcfg.inputPort().node() > max_ni_id :
                max_ni_id = linkcfg.inputPort().node()
        else :
            if linkcfg.inputPort().node() > max_node_id :
                max_node_id = linkcfg.inputPort().node()

        if linkcfg.outputPort().isNi() :
            if linkcfg.outputPort().node() > max_ni_id :
                max_ni_id = linkcfg.outputPort().node()
        else :
            if linkcfg.outputPort().node() > max_node_id :
                max_node_id = linkcfg.outputPort().node()

    networkcfg = esy_syscfg.EsyNetCfg( networkcfg_dict[ __json_tag_topology ], 
                                       networkcfg_dict[ __json_tag_size ],
                                       max_node_id + 1,
                                       max_ni_id + 1,
                                       linkcfg_list )
    return networkcfg

def readNetCfgJson( xml_file : str ) -> esy_syscfg.EsyNetCfg :
    if xml_file.endswith( __netcfg_extension ) :
        xml_file_with_extension = xml_file
    else :
        xml_file_with_extension = xml_file + __netcfg_extension

    with open( xml_file_with_extension, 'r', encoding='utf-8' ) as f:
        networkcfg_dict = json.load( f )

    networkcfg = jsonDictToNetCfg( networkcfg_dict )
    return networkcfg


def netCfgPortToJsonDict( portcfg : esy_syscfg.EsyNetCfgPort ) -> dict:
    portcfg_dict = {}
    portcfg_dict[ __json_tag_link_port_id ]        = portcfg.node()
    portcfg_dict[ __json_tag_link_port_port ]      = portcfg.port()
    portcfg_dict[ __json_tag_link_port_ni ]        = portcfg.isNi()
    portcfg_dict[ __json_tag_link_port_outperiod ] = portcfg.outPeriod()
    portcfg_dict[ __json_tag_link_port_bufs ]      = portcfg.bufs()
    return portcfg_dict

def netCfgLinkToJsonDict( linkcfg : esy_syscfg.EsyNetCfgLink ) -> dict:
    linkcfg_dict = {}
    linkcfg_dict[ __json_tag_link_dir ]          = linkcfg.linkDir()
    linkcfg_dict[ __json_tag_link_axis ]         = linkcfg.linkAxis()
    linkcfg_dict[ __json_tag_link_axis_dir ]     = linkcfg.linkAxisDir()
    linkcfg_dict[ __json_tag_link_ni_int_delay ] = linkcfg.niIntDelay()
    linkcfg_dict[ __json_tag_link_input_port ] = \
        netCfgPortToJsonDict( linkcfg.inputPort() )
    linkcfg_dict[ __json_tag_link_output_port ] = \
        netCfgPortToJsonDict( linkcfg.outputPort() )
    return linkcfg_dict

def netCfgToJsonDict( networkcfg : esy_syscfg.EsyNetCfg ) -> dict:
    networkcfg_dict = {}
    networkcfg_dict[ __json_tag_topology ] = networkcfg.topology()
    networkcfg_dict[ __json_tag_size ] = networkcfg.size()

    link_dict_list = []
    for link in networkcfg.links() :
        linkcfg_dict = netCfgLinkToJsonDict( link )
        link_dict_list.append( linkcfg_dict )
    networkcfg_dict[ __json_tag_links ] = link_dict_list

    return networkcfg_dict

def writeNetCfgJson( xml_file : str, networkcfg : esy_syscfg.EsyNetCfg ) :
    networkcfg_dict = netCfgToJsonDict( networkcfg )
    json_str = json.dumps( networkcfg_dict,
                           sort_keys=True,
                           indent=4,
                           separators=(',', ': ') )

    if xml_file.endswith( __netcfg_extension ) :
        xml_file_with_extension = xml_file
    else :
        xml_file_with_extension = xml_file + __netcfg_extension

    with open( xml_file_with_extension, 'w', encoding='utf-8' ) as f:
        f.write(json_str)
