##
# File name : esy_netcfg_router.h
# Function : Declare network configuration structure for routers.
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

from esy_netcfg_port import EsyNetCfgPort
import xml.etree.ElementTree as ET

##
# @brief Structure of router configuration item.
class EsyNetCfgRouter :
    ##
    # @brief Constructs an empty item with default value.
    def __init__( self ) :
        self.router_id = 0
        self.pipe_cycle = 1.0
        self.pos = [ 0.0, 0.0 ]
        self.port_list = []

    ##
    # @brief Function to set parameters
    # @{
    ##
    # @brief Construct an item for a template router.
    # @param pipe_cycle    Pipeline cycle.
    # @param phy_port      Number of physical port.
    # @param input_vc_num  Number of input virtual channel.
    # @param output_vc_num Number of output virtual channel.
    # @param input_buffer  Size of input buffer.
    # @param output_buffer Size of output buffer.
    def createTemplateRouter( self, pipe_cycle, phy_port, \
                              input_vc_num, output_vc_num, \
                              input_buffer, output_buffer ) :
        assert isinstance( pipe_cycle, (int, float) ), 'pipe_cycle must be a float-point number or an integer'
        assert isinstance( phy_port, int ), 'phy_port must be an integer'
        assert isinstance( input_vc_num, int ), 'input_vc_num must be an integer'
        assert isinstance( output_vc_num, int ), 'output_vc_num must be an integer'
        assert isinstance( input_buffer, int ), 'input_buffer must be an integer'
        assert isinstance( output_buffer, int ), 'output_buffer must be an integer'
        self.pipe_cycle = float( pipe_cycle )
        for i in range( 0, phy_port ) :
            port_cfg = EsyNetCfgPort().createTemplatePort( input_vc_num, output_vc_num, input_buffer, output_buffer )
            self.port_list.append( port_cfg )
    ##
    # @brief Construct an item for a router.
    # @param router_id   Router index.
    # @param pipe_cycle  Pipeline cycle.
    # @param pos         Position in graphic.
    # @param port_list   List of port configuration.
    def createRouter( self, router_id, pipe_cycle, pos, port_list ) :
        assert isinstance( router_id, int ), 'router_id must be an integer'
        assert isinstance( pipe_cycle, (int, float) ), 'pipe_cycle must be a float-point number or an integer'
        assert isinstance( pos, list ), 'pos should be a list of float-point numbers'
        for it in pos :
            assert isinstance( it, (int, float) ), 'pos should be a list of float-point numbers'
        assert isinstance( pos, list ), 'port_list should be a list of EsyNetCfgPort'
        for it in port_list :
            assert isinstance( it, EsyNetCfgPort ), 'port_list should be a list of EsyNetCfgPort'
        self.router_id = router_id
        self.pipe_cycle = float( pipe_cycle )
        self.pos = pos
        self.port_list = port_list

    ##
    # @brief Return the maximum number of virtual channels for all ports.
    def maxVcNum( self ) :
        if self.maxInputVcNum() > self.maxOutputVcNum() :
            return self.maxInputVcNum()
        else :
            return self.maxOutputVcNum()
    ##
    # @brief Return the maximum number of input virtual channels for all ports.
    def maxInputVcNum( self ) :
        vcnum = 1
        for port in self.port_list :
            if port.input_vc_num > vcnum :
                vcnum = port.input_vc_num
        return vcnum
    ##
    # @brief Return the maximum number of output virtual channels for all ports.
    def maxOutputVcNum( self ) :
        vcnum = 1
        for port in self.port_list :
            if port.output_vc_num > vcnum :
                vcnum = port.output_vc_num
        return vcnum
    ##
    # @brief Return the maximum size of input buffer for all ports.
    def maxInputBuffer( self ) :
        inputbuffer = 1
        for port in self.port_list :
            if port.input_buffer > inputbuffer :
                inputbuffer = port.input_buffer
        return inputbuffer
    ##
    # @brief Return the maximum size of output buffer for all ports.
    def maxOutputBuffer( self ) :
        outputbuffer = 1
        for port in self.port_list :
            if port.output_buffer > outputbuffer :
                output_buffer = port.output_buffer
        return output_buffer
    ##
    # @brief Return the total number of input virtual channels for all ports.
    def totalInputVc( self ) :
        inputvc = 0
        for port in self.port_list :
            inputvc += port.input_vc_num
        return inputvc
    ##
    # @brief Return the total number of output virtual channels for all ports.
    def totalOutputVc( self ) :
        outputvc = 0
        for port in self.port_list :
            outputvc += port.output_vc_num
        return outputvc

    ##
    # @brief Return the name of port.
    #
    # Format: \<port direction\>.\<port id\>. e.g. L.0, N.1, S.2, E.3, W.4. etc.
    #
    # @param i port id.
    # @return  name of port.
    def portName( self, port_id ) :
        assert port_id >= 0 and port_id < len( self.port_list ), 'port_id should be an integer within (0,' + str( len( self.port_list ) - 1) + ')'
        port_cfg = self.port_list[ port_id ]
        if port_cfg.network_interface :
            return 'L.' + str( port_id )
        else :
            return port_cfg.portDirection2ShortStr( port_cfg.port_dir ) + str( port_id )

    ##
    # @name Tag string for XML file
    # @{
    __xml_tag_router_id    = 'id'
    __xml_tag_router_pos   = 'position'
    __xml_tag_router_pos_x = 'x'
    __xml_tag_router_pos_y = 'y'
    __xml_tag_router_pipe  = 'pipe_cycle'
    __xml_tag_router_port  = 'port_cfg'
    __xml_tag_data  = 'data'
    __xml_tag_size  = 'size'
    __xml_tag_index = 'index'
    ##
    # @}

    ##
    # @name XML function
    ##
    # @brief Read router configuration from XML file.
    # @param root  root of XML structure.
    def readXml( self, root ) :
        assert isinstance( root, ET.Element ), 'root must an entity of xml.etree.ElementTree.Element'
        # Loop all child nodes
        child_node_list = root.getchildren()
        for child in child_node_list :
            # router id
            if child.tag == self.__xml_tag_router_id :
                self.router_id = int( child.text )
            # pipe cycle
            elif child.tag == self.__xml_tag_router_pipe :
                self.pipe_cycle = float( child.text )
            # position in GUI
            elif child.tag == self.__xml_tag_router_pos :
                for pos_child in child.getchildren() :
                    if pos_child.tag == self.__xml_tag_router_pos_x :
                        self.pos[ 0 ] = float( child.text )
                    elif pos_child.tag == self.__xml_tag_router_pos_y :
                        self.pos[ 1 ] = float( child.text )
            # ports
            elif child.tag == self.__xml_tag_router_port :
                port_num = child.get( self.__xml_tag_size, 0 )
                self.port_list = []
                for i in range( 0, port_num ) :
                    self.port_list.append( EsyNetCfgPort() )

                for port_child in child.getchildren() :
                    port_index = port_child.get( self.__xml_tag_index, 0 )
                    self.port_list[ port_index ].readXml( port_child )

    ##
    # @brief Write router configuration to XML file.
    # @param root  root of XML structure.
    def writeXml( self, root ) :
        assert isinstance( root, ET.Element ), 'root must an entity of xml.etree.ElementTree.Element'
        # router id
        child_item = ET.SubElement( root, self.__xml_tag_router_id )
        child_item.text = str( self.router_id )
        # pipe cycle
        child_item = ET.SubElement( root, self.__xml_tag_router_pipe )
        child_item.text = str( self.pipe_cycle )
        # position in GUI
        child_item = ET.SubElement( root, self.__xml_tag_router_pos )
        pos_child_item = ET.SubElement( child_item, self.__xml_tag_router_pos_x )
        pos_child_item.text = str( self.pos[ 0 ] )
        pos_child_item = ET.SubElement( child_item, self.__xml_tag_router_pos_y )
        pos_child_item.text = str( self.pos[ 1 ] )
        # ports
        child_item = ET.SubElement( root, self.__xml_tag_router_port )
        child_item.set( self.__xml_tag_size, len( self.port_list ) )
        for port in self.port_list :
            port_child_item = ET.SubElement( root, self.__xml_tag_data )
            port_child_item.set( self.__xml_tag_index, port.port_id )
            port.writeXml( port_child_item )
