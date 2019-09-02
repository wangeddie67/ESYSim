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

from .esy_netcfg_port import *
from typing import List
import xml.etree.ElementTree as ET
import copy

##
# @brief Structure of router configuration item.
class EsyNetCfgRouter :
    ##
    # @brief Constructs an empty item with default value.
    def __init__( self ) -> None :
        self.router_id = 0      # type: int
        self.pipe_cycle = 1.0   # type: float
        self.pos = [ 0.0, 0.0 ] # type: list[float]
        self.port_list = []     # type: list[EsyNetCfgPort]

    ##
    # @brief Return the maximum number of virtual channels for all ports.
    def maxVcNum( self ) -> int :
        if self.maxInputVcNum() > self.maxOutputVcNum() :
            return self.maxInputVcNum()
        else :
            return self.maxOutputVcNum()
    ##
    # @brief Return the maximum number of input virtual channels for all ports.
    def maxInputVcNum( self ) -> int:
        vcnum = 1
        for port in self.port_list :
            if port.input_vc_num > vcnum :
                vcnum = port.input_vc_num
        return vcnum
    ##
    # @brief Return the maximum number of output virtual channels for all ports.
    def maxOutputVcNum( self ) -> int :
        vcnum = 1
        for port in self.port_list :
            if port.output_vc_num > vcnum :
                vcnum = port.output_vc_num
        return vcnum
    ##
    # @brief Return the maximum size of input buffer for all ports.
    def maxInputBuffer( self ) -> int :
        inputbuffer = 1
        for port in self.port_list :
            if port.input_buffer > inputbuffer :
                inputbuffer = port.input_buffer
        return inputbuffer
    ##
    # @brief Return the maximum size of output buffer for all ports.
    def maxOutputBuffer( self ) -> int :
        outputbuffer = 1
        for port in self.port_list :
            if port.output_buffer > outputbuffer :
                output_buffer = port.output_buffer
        return output_buffer
    ##
    # @brief Return the total number of input virtual channels for all ports.
    def totalInputVc( self ) -> int :
        inputvc = 0
        for port in self.port_list :
            inputvc += port.input_vc_num
        return inputvc
    ##
    # @brief Return the total number of output virtual channels for all ports.
    def totalOutputVc( self ) -> int :
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
    def portName( self, port_id : int ) -> str :
        assert port_id >= 0 and port_id < len( self.port_list ), 'port_id should be an integer within (0,' + str( len( self.port_list ) - 1) + ')'
        port_cfg = self.port_list[ port_id ]    # type: EsyNetCfgPort
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
    def readXml( self, root : ET.Element ) -> None :
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
                        self.pos[ 0 ] = float( pos_child.text )
                    elif pos_child.tag == self.__xml_tag_router_pos_y :
                        self.pos[ 1 ] = float( pos_child.text )
            # ports
            elif child.tag == self.__xml_tag_router_port :
                port_num = int( child.get( self.__xml_tag_size, 0 ) )
                self.port_list = []
                for i in range( 0, port_num ) :
                    self.port_list.append( EsyNetCfgPort() )

                for port_child in child.getchildren() :
                    port_index = int( port_child.get( self.__xml_tag_index, 0 ) )
                    self.port_list[ port_index ].readXml( port_child )

    ##
    # @brief Write router configuration to XML file.
    # @param root  root of XML structure.
    def writeXml( self, root : ET.Element ) -> None :
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
        child_item.set( self.__xml_tag_size, str( len( self.port_list ) ) )
        for port in self.port_list :
            port_child_item = ET.SubElement( child_item, self.__xml_tag_data )
            port_child_item.set( self.__xml_tag_index, str( port.port_id ) )
            port.writeXml( port_child_item )

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
def createTemplateRouter( pipe_cycle : float, \
                          phy_port : int, \
                          input_vc_num : int, \
                          output_vc_num : int, \
                          input_buffer : int, \
                          output_buffer : int ) -> EsyNetCfgRouter :
    router_cfg = EsyNetCfgRouter()
    router_cfg.pipe_cycle = pipe_cycle
    for port_id in range( 0, phy_port ) :
        port_cfg = createTemplatePort( port_id,
                                       input_vc_num,
                                       output_vc_num,
                                       input_buffer,
                                       output_buffer )
        router_cfg.port_list.append( port_cfg )
    return router_cfg
##
# @brief Construct an item for a router.
# @param router_id   Router index.
# @param pipe_cycle  Pipeline cycle.
# @param pos         Position in graphic.
# @param port_list   List of port configuration.
def createRouter( router_id : int, \
                  pipe_cycle : float, \
                  pos : List[float], \
                  port_list : List[EsyNetCfgPort] ) -> EsyNetCfgRouter :
    router_cfg = EsyNetCfgRouter()
    router_cfg.router_id = router_id
    router_cfg.pipe_cycle = float( pipe_cycle )
    router_cfg.pos = pos
    router_cfg.port_list = copy.deepcopy( port_list )
    return router_cfg

