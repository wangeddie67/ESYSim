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

from esy_netcfg_ni import EsyNetCfgNI
from esy_netcfg_port import EsyNetCfgPort
from esy_netcfg_router import EsyNetCfgRouter
import xml.etree.ElementTree as ET
import copy

##
# @brief Structure of network configuration item.
class EsyNetCfg :
#{
#public:
    ##
    # @name network topology enumerate
    # @{
    topo_enum_switch    = 'Switch'
    topo_enum_ring      = 'Ring'
    topo_enum_2dmesh    = '2D-Mesh'
    topo_enum_2dtorus   = '2D-Torus'
    topo_enum_mdmesh    = 'Dia-Mesh'
    topo_enum_mdtorus   = 'Dia-Torus'
    topo_enum_irregular = 'Irregular'
    __topo_enum = [ topo_enum_switch \
                  , topo_enum_ring \
                  , topo_enum_2dmesh \
                  , topo_enum_2dtorus \
                  , topo_enum_mdmesh \
                  , topo_enum_mdtorus \
                  , topo_enum_irregular ]
    ##
    # @}

    ##
    # @brief Constructs an empty item with default value.
    def __init__( self ) :
        self.topology = self.topo_enum_switch
        self.size = [ 0 ]
        self.template_router = None
        self.template_ni = None
        self.router_list = []
        self.ni_list = []

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
    def createNetCfg( self, topology, size, pipe_cycle, phy_port, \
                      input_vc_num, output_vc_num, \
                      input_buffer, output_buffer, \
                      ni_pipe_cycle, ni_buffer_size, ni_interrupt_delay ) :
        assert topology in self.__topo_enum, 'topology must be one of ' + str( self.__topo_enum )
        assert isinstance( size, list ), 'size should be a list of integer numbers'
        for it in size :
            assert isinstance( it, int ), 'size should be a list of integer numbers'
        assert isinstance( pipe_cycle, (int, float) ), 'pipe_cycle must be a float-point number or an integer'
        assert isinstance( phy_port, int ), 'phy_port must be an integer'
        assert isinstance( input_vc_num, int ), 'input_vc_num must be an integer'
        assert isinstance( output_vc_num, int ), 'output_vc_num must be an integer'
        assert isinstance( input_buffer, int ), 'input_buffer must be an integer'
        assert isinstance( output_buffer, int ), 'output_buffer must be an integer'
        assert isinstance( ni_pipe_cycle, (int, float) ), 'ni_pipe_cycle must be a float-point number or an integer'
        assert isinstance( ni_buffer_size, int ), 'ni_buffer_size must be an integer'
        assert isinstance( ni_interrupt_delay, float ), 'ni_interrupt_delay must be a float-point number or an integer'
        # Set topology and valid size.
        self.topology = topology
        # If the topology is 2D mesh/torus, resize diamension to 2D.
        if self.topology == self.topo_enum_2dmesh or self.topology == self.topo_enum_2dtorus :
            if len( size ) == 0 :
                self.size = [ 1, 1 ]
            elif len( size ) == 1:
                self.size = [ size[ 0 ], 1 ]
            else :
                self.size = size
            for i in range( 0, 2 ) :
                if self.size[ i ] == 0 :
                    self.size[ i ] = 1
        # If the topology is multiple diamension mesh/torus, do nothing.
        elif self.topology == self.topo_enum_mdmesh or self.topology == self.topo_enum_mdtorus :
            self.size = size
            if len( self.size ) == 0 :
                self.size = [ 1 ]
            for i in range( 0, len( self.size ) ) :
                if self.size[ i ] == 0 :
                    self.size[ i ] = 1
        # If the topology is ring, set diamension to 1 and set size to the number of routers.
        elif ( self.topology == self.topo_enum_ring ) :
            self.size = size
            self.size = [ self.size[ 0 ] ]
        # If the topology is switch, set diamension to 1 and set size to 1.
        elif ( self.topology == self.topo_enum_switch ) :
            self.size = [ 1 ]

        t_2d = ( topology == self.topo_enum_2dmesh ) \
             or ( topology == self.topo_enum_2dtorus ) \
             or ( topology == self.topo_enum_mdmesh and len( self.size ) == 2 ) \
             or ( topology == self.topo_enum_mdtorus and len( self.size ) == 2 )
        t_3d = ( topology == self.topo_enum_mdmesh and len( self.size ) == 3 ) \
             or ( topology == self.topo_enum_mdtorus and len( self.size ) == 3 )
        t_irregular = ( topology == self.topo_enum_irregular )

        self.size = size
        self.template_router = EsyNetCfgRouter().createTemplateRouter( \
            pipe_cycle, phy_port, input_vc_num, output_vc_num, input_buffer, output_buffer )
        self.template_ni = EsyNetCfgNI().createTemplateNI( \
            ni_pipe_cycle, ni_buffer_size, ni_interrupt_delay )

        # Configure ports.
        for port_cfg in self.template_router.port_list :
            # NI port
            if port_cfg.port_id == 0 or self.topology == self.topo_enum_switch :
                port_cfg.port_axis = 0
                port_cfg.port_axis_dir = EsyNetCfgPort.port_axis_dir_enum_down
                port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_southwest
                port_cfg.network_interface = True
                continue
            # If the topology is irregular topology, does not connect.
            elif t_irregular :
                continue

            # Calculate the axis for the port. 0 is for NI. 1-2 is for axis dim-1-0, 3-4 is for axis dim-1-1, and so on.
            t_dim = len( self.size )
            t_axis = t_dim - 1 - ( ( port_cfg.port_id - 1 ) / 2 ) % t_dim
            port_cfg.port_axis = t_axis
            # If port is even, increase direction; otherwise, decrease direction.
            if port_cfg.port_id % 2 == 1 :
                port_cfg.port_axis_dir = EsyNetCfgPort.port_axis_dir_enum_down
            else :
                port_cfg.port_axis_dir = EsyNetCfgPort.port_axis_dir_enum_up
            # Set direction on GUI
            if t_2d :
                if port_cfg.port_axis == 0 :
                    if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_north
                    else :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_south
                elif port_cfg.port_axis == 1 :
                    if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_west
                    else :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_east
            # 3D topology, North, south, northwest, southeast, west, east;
            elif t_3d :
                if port_cfg.port_axis == 0 :
                    if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_north
                    else :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_south
                elif port_cfg.port_axis == 1 :
                    if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_northwest
                    else :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_southeast
                elif port_cfg.port_axis == 2 :
                    if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_west
                    else :
                        port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_east
            # otherwise topology, North, south
            else :
                if port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_north
                else :
                    port_cfg.port_dir = EsyNetCfgPort.port_dir_enum_south

            # set Size of input and output buffer
            # Set port 0 as NI port.
            port_cfg.network_interface = False

        # Generate configuration for all router.
        self.updateNetwork()

    ##
    # @brief Overload operator __str__ function.
    def __str__( self ):
        string = ''
        # topology
        string += 'Topology=' + self.topology + '\n'
        # size
        string += 'Size=' + self.size + '\n'
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
            string += '\n'

        return string

    ##
    # @brief Return the maximum number of physical channels for all routers.
    def maxPcNum( self ) :
        pcnum = 1
        for router_cfg in self.router_list :
            if router_cfg.portNum() > pcnum :
                pcnum = router_cfg.portNum()
        return pcnum
    ##
    # @brief Return the maximum number of input virtual channels for all ports of all routers.
    def maxInputVcNum( self ) :
        vcnum = 1
        for router_cfg in self.router_list :
            if router_cfg.maxInputVcNum() > vcnum :
                vcnum = router_cfg.maxInputVcNum()
        return vcnum
    ##
    # @brief Return the maximum number of output virtual channels for all ports of all routers.
    def maxOutputVcNum( self ) :
        vcnum = 1
        for router_cfg in self.router_list :
            if router_cfg.maxOutputVcNum() > vcnum :
                vcnum = router_cfg.maxOutputVcNum()
        return vcnum
    ##
    # @brief Return the maximum size of input buffer for all ports of all routers.
    def maxInputBuffer( self ) :
        inputbuffer = 1
        for router_cfg in self.router_list :
            if router_cfg.maxInputBuffer() > inputbuffer :
                inputbuffer = router_cfg.maxInputBuffer()
        return inputbuffer
    ##
    # @brief Return the maximum number of output buffer for all ports of all routers.
    def maxOutputBuffer( self ) :
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
    def readXml( self, root ) :
        assert isinstance( root, ET.Element ), 'root must an entity of xml.etree.ElementTree.Element'
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
                    self.size.append( int( child.text ) )
            # Template router element.
            elif child.tag == self.__xml_tag_template_router :
                self.template_router = EsyNetCfgRouter().readXml( child )
            # Template NI element.
            elif child.tag == self.__xml_tag_template_ni : 
                self.template_ni = EsyNetCfgNI().readXml( child )
            # Special router elements.
            elif child.tag == self.__xml_tag_router : 
                router_num = child.get( self.__xml_tag_size, 0 )
                self.router_list = []
                for i in range( 0, router_num ) :
                    self.router_list.append( EsyNetCfgRouter() )

                for router_child in child.getchildren() :
                    router_index = router_child.get( self.__xml_tag_index, 0 )
                    self.router_list[ router_index ].readXml( router_child )
            # Special NI elements.
            elif child.tag == self.__xml_tag_ni : 
                ni_num = child.get( self.__xml_tag_size, 0 )
                self.ni_list = []
                for i in range( 0, ni_num ) :
                    self.ni_list.append( EsyNetCfgNI() )

                for ni_child in child.getchildren() :
                    ni_index = ni_child.get( self.__xml_tag_index, 0 )
                    self.ni_list[ ni_index ].readXml( ni_child )

    ##
    # @brief Write network configuration to XML file.
    # @param root  root of XML structure.
    def writeXml( self, root ) :
        assert isinstance( root, ET.Element ), 'root must an entity of xml.etree.ElementTree.Element'
        # Topology element.
        child_item = ET.SubElement( root, self.__xml_tag_topology )
        child_item.text = self.topology
        # Network size element.
        child_item = ET.SubElement( root, self.__xml_tag_network_size )
        child_item.set( self.__xml_tag_size, len( self.port_list ) )
        for i in range( 0, len( self.size ) ) :
            # Size on diamension.
            size_child_item = ET.SubElement( child_item, self.__xml_tag_data )
            size_child_item.set( self.__xml_tag_index, i )
            size_child_item.text = str( self.size[ i ] )
        # Template router.
        child_item = ET.SubElement( root, self.__xml_tag_template_router )
        self.template_router.writeXml( child_item )
        # Template NI.
        child_item = ET.SubElement( root, self.__xml_tag_template_ni )
        self.template_ni.writeXml( child_item )
        # Special routers.
        child_item = ET.SubElement( root, self.__xml_tag_router )
        child_item.set( self.__xml_tag_size, len( self.router_list ) )
        for router_cfg in self.router_list :
            router_child_item = ET.SubElement( child_item, self.__xml_tag_data )
            router_child_item.set( self.__xml_tag_index, router_cfg.router_id )
            router_cfg.writeXml( child_item )
        # Special ni.
        child_item = ET.SubElement( root, self.__xml_tag_ni )
        child_item.set( self.__xml_tag_size, len( self.ni_list ) )
        for ni_cfg in self.ni_list :
            ni_child_item = ET.SubElement( child_item, self.__xml_tag_data )
            ni_child_item.set( self.__xml_tag_index, ni_cfg.ni_id )
            ni_cfg.writeXml( child_item )

    __netcfg_extension = 'netcfg'

    ##
    # @brief Open XML network configuration file for reading.
    # @param fname  direction of network configuration file.
    # @return  XML error handler.
    def readXmlFile( self, fname ) :
        assert isinstance( fname, str ), 'fname must a string'
        if fname.endswith( self.__netcfg_extension ) :
            tname = fname
        else :
            tname = fname + '.' + self.__netcfg_extension
        tree = ET.parse( tname )
        root = tree.getroot()

        self.readXml( root )
    ##
    # @brief Open XML network configuration file for writing.
    # @param fname  direction of network configuration file.
    # @return  XML error handler.
    def writeXmlFile( self, fname ) :
        assert isinstance( fname, str ), 'fname must a string'
        tree = ET.ElementTree()
        root = tree.getroot()

        self.writeXml( root )

        if fname.endswith( self.__netcfg_extension ) :
            tname = fname
        else :
            tname = fname + '.' + self.__netcfg_extension
        tree.write( tname, xml_declaration="<?xml version=\"1.0\"?>" )
    ##
    # @}

    ##
    # @brief Convert router coordinary to router id based on topology.
    # @param coord  router coordinary.
    # @return  router id.
    def coord2Seq( self, coord ) :
        seq_t = coord[ -1 ]
        for i in range( 1, len( self.size ) ) :
            seq_t = ( seq_t * self.size[ len( self.size ) - i - 1 ] + \
                coord[ len( self.size ) - i - 1 ] )
        return seq_t
    ##
    # @brief Convert router id to router coordinary based on topology.
    # @param seq  router id.
    # @return  router coordinary.
    def seq2Coord( self, seq ) :
        seq_t = seq
        coord = []
        for i in range( 0, len( self.size ) ) :
            coord.append( seq_t % self.size[ i ] )
            seq_t = seq_t / self.size[ i ]
        return coord
    ##
    # @brief Calculate the distance between two positions.
    # @param p1  position 1.
    # @param p2  position 2.
    # @return  Mahanten distance between positions.
    def coordDistance( self, p1, p2 ) :
        t_p1 = self.seq2Coord( p1 )
        t_p2 = self.seq2Coord( p2 )
        dist = 0
        for i in range( 0, len( self.size ) ) :
            dist = dist + abs( t_p1[ i ] - t_p2[ i ] );
        return dist;
    #/**
     #* @brief Check the router id is valid or not.
     #* @param id router id to check.
     #* @return  true, if the router id is valid.
     #*/
    #bool validRouterId( long id ) const { return ( ( id >= 0 ) && ( id < routerCount() ) ); }
    #/**
     #* @}
     #*/

    ##
    # @brief Generate configuration of routers and NIs based on general configurations and topology.
    def updateNetwork( self ) :
        self.router_list = []
        self.ni_list = []

        # Calculate router count;
        router_count = 1
        for size_t in self.size :
            router_count *= size_t

        # Generate router and NIs
        for router_id in range( 0, router_count ) :
            # Copy router configuration from template router.
            router_cfg = EsyNetCfgRouter( router_id, \
                self.template_router.pipe_cycle, self.template_router.pos, self.template_router.port_list )

            # Ger coordinary of the router.
            ax = self.seq2Coord(router_cfg.router_id )

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

            # Disable port for mesn topology
            if self.topology == '2D-Mesh' or self.topology == 'Dia-Mesh' :
                for port_cfg in router_cfg.port_list :
                    if port_cfg.network_interface :
                        continue
                    # The decrease direction port at the first router on the axis or the increase direction port at the last router on the axis,
                    # reset the vc number to 0 to close the port.
                    port_axis = port_cfg.port_axis
                    if ( ( ax[ port_axis ] == 0 and port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down ) \
                        or ( ax[ port_axis ]  == self.size[ port_axis ] - 1 and port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_up ) ) :
                        port_cfg.input_vc_num = 0
                        port_cfg.output_vc_num = 0

            # Assign port.
            for port_cfg in router_cfg.port_list :
                port_axis = port_cfg.port_axis
                # Port connects to NI, do not change.
                if port_cfg.network_interface :
                    continue
                # If the topology is irregular, do not assign port.
                elif self.topology == 'Irregular' : 
                    port_cfg.neighbor_router = router_cfg.router_id
                    port_cfg.neighbor_port = port_cfg.port_id
                # Decrease port, connect to the previous router on the axis, port + 1.
                elif port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_down :
                    neighbor_ax = copy.deepcopy( ax )
                    if ax[ port_axis ] > 0 :
                        neighbor_ax[ port_axis ] = neighbor_ax[ port_axis ] - 1
                        port_cfg.neighbor_router = self.coord2Seq( neighbor_ax )
                        port_cfg.neighbor_port = port_cfg.port_id + 1
                    # If the first router on the axis, loop to the last router.
                    elif ( ax[ port_axis ] == 0 ) and \
                        ( self.topology == '2D-Torus' or self.topology == 'Dia-Torus' or self.topology == 'Ring' ) :
                        neighbor_ax[ port_axis ] = self.size[ port_axis ] - 1
                        port_cfg.neighbor_router = self.coord2Seq( neighbor_ax );
                        port_cfg.neighbor_port = port_cfg.port_id + 1
                # Increase port, connect to the next router on the axis, port - 1.
                elif port_cfg.port_axis_dir == EsyNetCfgPort.port_axis_dir_enum_up :
                    neighbor_ax = copy.deepcopy( ax )
                    if ax[ port_axis ] < self.size[ port_axis ] - 1 :
                        neighbor_ax[ port_axis ] = neighbor_ax[ port_axis ] + 1;
                        port_cfg.neighbor_router = self.coord2Seq( neighbor_ax )
                        port_cfg.neighbor_port = port_cfg.port_id - 1
                    # If the last router on the axis, loop to the first router.
                    elif ( ax[ port_axis ] == self.size[ port_axis ] - 1 ) and \
                        ( self.topology == '2D-Torus' or self.topology == 'Dia-Torus' or self.topology == 'Ring' ) :
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
