##
# File name : esy_netcfg.h
# Function : Declare network configuration structure.
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
# Copyright (C) 2019, Junshi Wang <wangeddie67@gmail.com>

from .esy_netcfg_ni import *
from .esy_netcfg_port import *
from .esy_netcfg_router import *
from typing import List
import xml.etree.ElementTree as ET
import copy

##
# @brief Structure of network configuration item.
class EsyNetCfg :
    ##
    # @name Network topology enumerate
    # @{
    topology_enum_switch    = 'Switch'
    topology_enum_ring      = 'Ring'
    topology_enum_2dmesh    = '2D-Mesh'
    topology_enum_2dtorus   = '2D-Torus'
    topology_enum_diamesh   = 'Dia-Mesh'
    topology_enum_diatorus  = 'Dia-Torus'
    topology_enum_irregular = 'Irregular'
    topology_enum_invalid   = 'Invalid'
    topology_enum = [ topology_enum_switch
                    , topology_enum_ring
                    , topology_enum_2dmesh
                    , topology_enum_2dtorus
                    , topology_enum_diamesh
                    , topology_enum_diatorus
                    , topology_enum_irregular ]
    ##
    # @}

    ##
    # @brief Constructs an empty item with default value.
    def __init__( self ) -> None :
        self.topology = self.topology_enum_invalid   # type: str
        self.size = [ 0 ]           # type: list[int]
        self.template_router = None # type: EsyNetCfgRouter
        self.template_ni = None     # type: EsyNetCfgNI
        self.router_list = []       # type: list[EsyNetCfgRouter]
        self.ni_list = []           # type: list[EsyNetCfgNI]

    ##
    # @brief Overload operator __str__ function.
    def __str__( self ) -> str:
        string = ''
        # topology
        string += 'Topology=' + self.topology + '\n'
        # size
        string += 'Size=' + str(self.size) + '\n'
        # Table start
        string += '    Router    |                    Port                     |               NI' + '\n'
        string += '              |       In  In   Out  Out Neig Neig      Axis |              Conn Conn        Int' + '\n'
        string += '  ID   Pipe   |  ID   VC  Buf  VC   Buf R/NI Port Axis Dir  |  ID   Pipe   Rout Port  Buf  Delay' + '\n'
        string += '--------------|---------------------------------------------|--------------------------------------' + '\n'
        # router configuration
        for router_cfg in self.router_list :
            # port
            port_i = 0
            for port_cfg in router_cfg.port_list :
                if port_i == 0 :
                    string += '{:4d} {:8f} |'.format( router_cfg.router_id, router_cfg.pipe_cycle )
                else :
                    string += '              |';

                string += '{:4d} {:4d} {:4d} {:4d} {:4d} {:4d} {:4d} '. \
                    format( port_cfg.port_id, port_cfg.input_vc_num, port_cfg.input_buffer, \
                        port_cfg.output_vc_num, port_cfg.output_buffer, \
                        port_cfg.neighbor_router, port_cfg.neighbor_port )
                if port_cfg.network_interface :
                    # ni
                    ni_cfg = self.ni_list[ port_cfg.neighbor_router ]
                    string += '  NI      |{:4d} {:8f} {:4d} {:4d} {:4d} {:8f} '. \
                        format( ni_cfg.ni_id, ni_cfg.pipe_cycle, ni_cfg.connect_router, ni_cfg.connect_port, \
                            ni_cfg.buffer_size, ni_cfg.interrupt_delay )
                else :
                    string += '{:4d} '.format( port_cfg.port_axis ) + port_cfg.port_axis_dir

                port_i += 1
                string += '\n'

        return string

    ##
    # @brief Return the maximum number of physical channels for all routers.
    def maxPcNum( self ) -> int :
        pcnum = 1
        for router_cfg in self.router_list :
            if router_cfg.portNum() > pcnum :
                pcnum = router_cfg.portNum()
        return pcnum
    ##
    # @brief Return the maximum number of input virtual channels for all ports of all routers.
    def maxInputVcNum( self ) -> int :
        vcnum = 1
        for router_cfg in self.router_list :
            if router_cfg.maxInputVcNum() > vcnum :
                vcnum = router_cfg.maxInputVcNum()
        return vcnum
    ##
    # @brief Return the maximum number of output virtual channels for all ports of all routers.
    def maxOutputVcNum( self ) -> int :
        vcnum = 1
        for router_cfg in self.router_list :
            if router_cfg.maxOutputVcNum() > vcnum :
                vcnum = router_cfg.maxOutputVcNum()
        return vcnum
    ##
    # @brief Return the maximum size of input buffer for all ports of all routers.
    def maxInputBuffer( self ) -> int :
        inputbuffer = 1
        for router_cfg in self.router_list :
            if router_cfg.maxInputBuffer() > inputbuffer :
                inputbuffer = router_cfg.maxInputBuffer()
        return inputbuffer
    ##
    # @brief Return the maximum number of output buffer for all ports of all routers.
    def maxOutputBuffer( self ) -> int :
        outputbuffer = 1
        for router_cfg in self.router_list :
            if router_cfg.maxOutputBuffer() > outputbuffer :
                outputbuffer = router_cfg.maxOutputBuffer()
        return outputbuffer

    ##
    # @name Tag string for XML file
    # @{
    __xml_tag_root            = 'networkcfg'

    __xml_tag_topology        = 'topology'
    __xml_tag_network_size    = 'size'
    __xml_tag_template_router = 'template_router_cfg'
    __xml_tag_template_ni     = 'template_ni_cfg'
    __xml_tag_router          = 'router_cfg'
    __xml_tag_ni              = 'ni_cfg'

    __xml_tag_data  = 'data'
    __xml_tag_size  = 'size'
    __xml_tag_index = 'index'
    ##
    # @}

    ##
    # @name XML function
    ##
    # @brief Read network configuration from XML file.
    # @param root  root of XML structure.
    def readXml( self, root : ET.Element ) -> None :
        # Loop all child nodes
        child_node_list = root.getchildren()
        for child in child_node_list :
            # Topology element
            if child.tag == self.__xml_tag_topology :
                self.topology = child.text
            # Network size element.
            elif child.tag == self.__xml_tag_network_size :
                self.size = []
                # Loop all item behind network size.
                for size_child in child.getchildren() :
                    self.size.append( int( size_child.text ) )
            # Template router element.
            elif child.tag == self.__xml_tag_template_router :
                self.template_router = EsyNetCfgRouter().readXml( child )
            # Template NI element.
            elif child.tag == self.__xml_tag_template_ni : 
                self.template_ni = EsyNetCfgNI().readXml( child )
            # Special router elements.
            elif child.tag == self.__xml_tag_router : 
                router_num = int( child.get( self.__xml_tag_size, 0 ) )
                self.router_list = []
                for i in range( 0, router_num ) :
                    self.router_list.append( EsyNetCfgRouter() )

                for router_child in child.getchildren() :
                    router_index = int( router_child.get( self.__xml_tag_index, 0 ) )
                    self.router_list[ router_index ].readXml( router_child )
            # Special NI elements.
            elif child.tag == self.__xml_tag_ni : 
                ni_num = int( child.get( self.__xml_tag_size, 0 ) )
                self.ni_list = []
                for i in range( 0, ni_num ) :
                    self.ni_list.append( EsyNetCfgNI() )

                for ni_child in child.getchildren() :
                    ni_index = int( ni_child.get( self.__xml_tag_index, 0 ) )
                    self.ni_list[ ni_index ].readXml( ni_child )

    ##
    # @brief Write network configuration to XML file.
    # @param root  root of XML structure.
    def writeXml( self, root : ET.Element ) -> None :
        # Topology element.
        child_item = ET.SubElement( root, self.__xml_tag_topology )
        child_item.text = self.topology
        # Network size element.
        child_item = ET.SubElement( root, self.__xml_tag_network_size )
        child_item.set( self.__xml_tag_size, str( len( self.size ) ) )
        for i in range( 0, len( self.size ) ) :
            # Size on diamension.
            size_child_item = ET.SubElement( child_item, self.__xml_tag_data )
            size_child_item.set( self.__xml_tag_index, str( i ) )
            size_child_item.text = str( self.size[ i ] )
        # Template router.
        child_item = ET.SubElement( root, self.__xml_tag_template_router )
        self.template_router.writeXml( child_item )
        # Template NI.
        child_item = ET.SubElement( root, self.__xml_tag_template_ni )
        self.template_ni.writeXml( child_item )
        # Special routers.
        child_item = ET.SubElement( root, self.__xml_tag_router )
        child_item.set( self.__xml_tag_size, str( len( self.router_list ) ) )
        for router_cfg in self.router_list :
            router_child_item = ET.SubElement( child_item, self.__xml_tag_data )
            router_child_item.set( self.__xml_tag_index, str( router_cfg.router_id ) )
            router_cfg.writeXml( router_child_item )
        # Special ni.
        child_item = ET.SubElement( root, self.__xml_tag_ni )
        child_item.set( self.__xml_tag_size, str( len( self.ni_list ) ) )
        for ni_cfg in self.ni_list :
            ni_child_item = ET.SubElement( child_item, self.__xml_tag_data )
            ni_child_item.set( self.__xml_tag_index, str( ni_cfg.ni_id ) )
            ni_cfg.writeXml( ni_child_item )

    __netcfg_extension = 'netcfg'

    ##
    # @brief Open XML network configuration file for reading.
    # @param fname  direction of network configuration file.
    # @return  XML error handler.
    def readXmlFile( self, fname : str ) -> None :
        # add extension
        if fname.endswith( self.__netcfg_extension ) :
            tname = fname
        else :
            tname = fname + '.' + self.__netcfg_extension
        # read XML file
        try:
            tree = ET.parse( tname )
        except Exception as ex :
            print( ex )
        # parse element tree
        root = tree.getroot()
        self.readXml( root )
    ##
    # @brief Open XML network configuration file for writing.
    # @param fname  direction of network configuration file.
    # @return  XML error handler.
    def writeXmlFile( self, fname : str ) -> None :
        # build element tree
        root = ET.Element( self.__xml_tag_root )
        self.writeXml( root )
        # add extension
        if fname.endswith( self.__netcfg_extension ) :
            tname = fname
        else :
            tname = fname + '.' + self.__netcfg_extension
        # write XML file
        tree = ET.ElementTree( root )
        try :
            tree.write( tname,
                        encoding='UTF-8',
                        xml_declaration='<?xml version=\"1.0\"?>' )
        except Exception as ex :
            print( ex )
            
    ##
    # @}

    ##
    # @brief Convert router coordinary to router id based on topology.
    # @param coord  router coordinary.
    # @return  router id.
    def coord2Seq( self, coord : List[int] ) -> int :
        seq_t = coord[ -1 ]
        for i in range( 1, len( self.size ) ) :
            seq_t = ( seq_t * self.size[ len( self.size ) - i - 1 ] + \
                coord[ len( self.size ) - i - 1 ] )
        return seq_t
    ##
    # @brief Convert router id to router coordinary based on topology.
    # @param seq  router id.
    # @return  router coordinary.
    def seq2Coord( self, seq : int ) -> List[int]:
        seq_t = seq
        coord = []
        for i in range( 0, len( self.size ) ) :
            coord.append( seq_t % self.size[ i ] )
            seq_t = seq_t // self.size[ i ]
        return coord
    ##
    # @brief Calculate the distance between two positions.
    # @param p1  position 1.
    # @param p2  position 2.
    # @return  Mahanten distance between positions.
    def coordDistance( self, p1 : int, p2 : int ) -> int :
        t_p1 = self.seq2Coord( p1 )
        t_p2 = self.seq2Coord( p2 )
        dist = 0
        for i in range( 0, len( self.size ) ) :
            dist = dist + abs( t_p1[ i ] - t_p2[ i ] );
        return dist;

    ##
    # @brief Generate configuration of routers and NIs based on general configurations and topology.
    def updateNetwork( self ) :
        # category of topology enum
        topology_enum_ring = [ self.topology_enum_2dtorus, self.topology_enum_diatorus, self.topology_enum_ring ]
        topology_enum_bus = [ self.topology_enum_2dmesh, self.topology_enum_diamesh ]

        self.router_list = []
        self.ni_list = []

        # Calculate router count;
        router_count = 1
        for size_t in self.size :
            router_count *= size_t

        # Generate router and NIs
        for router_id in range( 0, router_count ) :
            # Copy router configuration from template router.
            router_cfg = createRouter( router_id,
                                       self.template_router.pipe_cycle,
                                       self.template_router.pos,
                                       self.template_router.port_list )

            # Ger coordinary of the router.
            ax = self.seq2Coord( router_cfg.router_id )

            # generate NI structure
            for port_cfg in router_cfg.port_list :
                if port_cfg.network_interface :
                    ni_id = len( self.ni_list )
                    # generate ni structure
                    ni_cfg = createNI( ni_id,
                                       router_cfg.router_id,
                                       port_cfg.port_id,
                                       self.template_ni.pipe_cycle,
                                       self.template_ni.buffer_size,
                                       self.template_ni.interrupt_delay )
                    self.ni_list.append( ni_cfg )
                    # assign connection
                    port_cfg.neighbor_router = ni_id
                    port_cfg.neighbor_port = 0

            # Disable port for mesn topology
            if self.topology in topology_enum_bus :
                for port_cfg in router_cfg.port_list :
                    if port_cfg.network_interface :
                        continue
                    # The decrease direction port at the first router on the axis or the increase direction port at the last router on the axis,
                    # reset the vc number to 0 to close the port.
                    port_axis = port_cfg.port_axis
                    if ( ( ax[ port_axis ] == 0 and port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward ) \
                        or ( ax[ port_axis ]  == self.size[ port_axis ] - 1 and port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_upward ) ) :
                        port_cfg.input_vc_num = 0
                        port_cfg.output_vc_num = 0

            # Assign port.
            for port_cfg in router_cfg.port_list :
                port_axis = port_cfg.port_axis
                # Port connects to NI, do not change.
                if port_cfg.network_interface :
                    continue
                # If the topology is irregular, do not assign port.
                elif self.topology == self.topology_enum_irregular : 
                    port_cfg.neighbor_router = router_cfg.router_id
                    port_cfg.neighbor_port = port_cfg.port_id
                # Decrease port, connect to the previous router on the axis, port + 1.
                elif port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward :
                    neighbor_ax = copy.deepcopy( ax )
                    if ax[ port_axis ] > 0 :
                        neighbor_ax[ port_axis ] = neighbor_ax[ port_axis ] - 1
                        port_cfg.neighbor_router = self.coord2Seq( neighbor_ax )
                        port_cfg.neighbor_port = port_cfg.port_id + 1
                    # If the first router on the axis, loop to the last router.
                    elif ( ax[ port_axis ] == 0 ) and ( self.topology in topology_enum_ring ) :
                        neighbor_ax[ port_axis ] = self.size[ port_axis ] - 1
                        port_cfg.neighbor_router = self.coord2Seq( neighbor_ax );
                        port_cfg.neighbor_port = port_cfg.port_id + 1
                # Increase port, connect to the next router on the axis, port - 1.
                elif port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_upward :
                    neighbor_ax = copy.deepcopy( ax )
                    if ax[ port_axis ] < self.size[ port_axis ] - 1 :
                        neighbor_ax[ port_axis ] = neighbor_ax[ port_axis ] + 1;
                        port_cfg.neighbor_router = self.coord2Seq( neighbor_ax )
                        port_cfg.neighbor_port = port_cfg.port_id - 1
                    # If the last router on the axis, loop to the first router.
                    elif ( ax[ port_axis ] == self.size[ port_axis ] - 1 ) and ( self.topology in topology_enum_ring ) :
                        neighbor_ax[ port_axis ] = 0;
                        port_cfg.neighbor_router = self.coord2Seq( neighbor_ax )
                        port_cfg.neighbor_port = port_cfg.port_id - 1

            self.router_list.append( router_cfg )

    ##
    # @brief Generate configuration of NIs based on general configurations and topology.
    def updateNI( self ) :
        # NIs
        self.ni_list = []

        # Generate router and NIs
        for router_cfg in self.router_list :
            # generate NI structure
            for port_cfg in router_cfg.port_list :
                if port_cfg.network_interface :
                    ni_id = len( self.ni_list )
                    # generate ni structure
                    ni_cfg = EsyNetCfgNI( ni_id, router_cfg.router_id, port_cfg.port_id, \
                        self.template_ni.pipe_cycle, self.template_ni.buffer_size, self.template_ni.interrupt_delay )
                    self.ni_list.append( ni_cfg )
                    # assign connection
                    port_cfg.neighbor_router = ni_id
                    port_cfg.neighbor_port = 0

##
# @brief Constructs an regular network configuration.
# @param topology Network topology.
# @param size Network size in diamensions.
# @param pipe_cycle Pipeline cycle.
# @param phy_port Number of physical channel for each router.
# @param input_vc_num Number of input virtual channel for each physical channel.
# @param output_vc_num Number of output virtual channel for each physical channel.
# @param input_buffer Size of input buffer.
# @param output_buffer Size of output buffer.
# @param ni_pipe_cycle Pipeline cycle of NI.
# @param ni_buffer Size of eject buffer in NI.
# @param ni_interrupt_delay Delay to response NI interrupt.
def createNetCfg( topology : str, \
                  size : List[int], \
                  pipe_cycle : float, \
                  phy_port : int, \
                  input_vc_num : int, \
                  output_vc_num : int, \
                  input_buffer : int, \
                  output_buffer : int, \
                  ni_pipe_cycle : float, \
                  ni_buffer_size : int, \
                  ni_interrupt_delay : float ) -> EsyNetCfg :
    # category of topology enum
    topology_enum_1d = [ EsyNetCfg.topology_enum_ring, EsyNetCfg.topology_enum_irregular ]
    topology_enum_2d = [ EsyNetCfg.topology_enum_2dmesh, EsyNetCfg.topology_enum_2dtorus ]
    topology_enum_dia = [ EsyNetCfg.topology_enum_diamesh, EsyNetCfg.topology_enum_diatorus ]

    net_cfg = EsyNetCfg()
    # Set topology and valid size.
    net_cfg.topology = topology
    # If the topology is ring, set diamension to 1 and set size to the number of routers.
    if net_cfg.topology in topology_enum_1d :
        if len( size ) == 0 :
            net_cfg.size = [ 1 ]
        else :
            net_cfg.size = size[ 0 ]
    # If the topology is 2D mesh/torus, resize diamension to 2D.
    elif net_cfg.topology in topology_enum_2d :
        if len( size ) == 0 :
            net_cfg.size = [ 1, 1 ]
        elif len( size ) == 1:
            net_cfg.size = [ size[ 0 ], 1 ]
        else:
            net_cfg.size = [ size[ 0 ], size[ 1 ] ]
        for i in range( 0, 2 ) :
            if net_cfg.size[ i ] == 0 :
                net_cfg.size[ i ] = 1
    # If the topology is multiple diamension mesh/torus, do nothing.
    elif net_cfg.topology in topology_enum_dia :
        if len( size ) == 0 :
            net_cfg.size = [ 1 ]
        else :
            net_cfg.size = size
        for i in range( 0, len( net_cfg.size ) ) :
            if net_cfg.size[ i ] == 0 :
                net_cfg.size[ i ] = 1
    # If the topology is switch, set diamension to 1 and set size to 1.
    elif net_cfg.topology == net_cfg.topology_enum_switch :
        net_cfg.size = [ 1 ]

    t_2d = ( topology in topology_enum_2d ) \
            or ( ( topology in topology_enum_dia ) and len( net_cfg.size ) == 2 )
    t_3d = ( topology in topology_enum_dia ) and len( net_cfg.size ) == 3
    t_irregular = ( topology == net_cfg.topology_enum_irregular )

    net_cfg.template_router = createTemplateRouter( pipe_cycle,
                                                    phy_port,
                                                    input_vc_num,
                                                    output_vc_num,
                                                    input_buffer,
                                                    output_buffer )
    net_cfg.template_ni = createTemplateNI( ni_pipe_cycle, 
                                            ni_buffer_size, 
                                            ni_interrupt_delay )

    # Configure ports.
    for port_cfg in net_cfg.template_router.port_list :
        # NI port
        if port_cfg.port_id == 0 or net_cfg.topology == EsyNetCfg.topology_enum_switch :
            port_cfg.port_axis = 0
            port_cfg.port_axis_dir = EsyNetCfgPort.port_axis_dir_enum_downward
            port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_southwest
            port_cfg.network_interface = True
            continue
        # If the topology is irregular topology, does not connect.
        elif t_irregular :
            continue

        # Calculate the axis for the port. 0 is for NI. 1-2 is for axis dim-1-0, 3-4 is for axis dim-1-1, and so on.
        t_dim = len( net_cfg.size )
        t_axis = t_dim - 1 - ( ( port_cfg.port_id - 1 ) // 2 ) % t_dim
        port_cfg.port_axis = t_axis
        # If port is even, increase direction; otherwise, decrease direction.
        if port_cfg.port_id % 2 == 1 :
            port_cfg.port_axis_dir = EsyNetCfgPort.port_axis_dir_enum_downward
        else :
            port_cfg.port_axis_dir = EsyNetCfgPort.port_axis_dir_enum_upward
        # Set direction on GUI
        if t_2d :
            if port_cfg.port_axis == 0 :
                if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_north
                else :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_south
            elif port_cfg.port_axis == 1 :
                if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_west
                else :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_east
        # 3D topology, North, south, northwest, southeast, west, east;
        elif t_3d :
            if port_cfg.port_axis == 0 :
                if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_north
                else :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_south
            elif port_cfg.port_axis == 1 :
                if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_northwest
                else :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_southeast
            elif port_cfg.port_axis == 2 :
                if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_west
                else :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_east
        # otherwise topology, North, south
        else :
            if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_downward :
                port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_north
            else :
                port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_south

        # set Size of input and output buffer
        # Set port 0 as NI port.
        port_cfg.network_interface = False

    # Generate configuration for all router.
    net_cfg.updateNetwork()
    
    return net_cfg

def createNetCfgFromFile( file_path : str ) -> EsyNetCfg :
    net_cfg = EsyNetCfg()
    net_cfg.readXmlFile( file_path )
    return net_cfg
