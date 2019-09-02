##
# File name : esy_netcfg_ni.h
# Function : Network configuration structure for network interface.
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

import xml.etree.ElementTree as ET

##
# @brief Structure of NI configuration item.
class EsyNetCfgNI :
    ##
    # @brief Constructor an empty item with default values.
    def __init__( self ) -> None :
        self.ni_id = 0                  # type: str
        self.connect_router = 0         # type: int
        self.connect_port = 0           # type: int
        self.pipe_cycle = 1.0           # type: float
        self.buffer_size = 10           # type: int
        self.interrupt_delay = 200.0    # type: float

    ##
    # @name Tag string for XML file
    # @{
    __xml_tag_ni_id              = 'id'
    __xml_tag_ni_connect_router  = 'connect_router'
    __xml_tag_ni_connect_port    = 'connect_port'
    __xml_tag_ni_pipe_cycle      = 'pipe_cycle'
    __xml_tag_ni_buffer_size     = 'buffer_size'
    __xml_tag_ni_interrupt_delay = 'interrupt_delay'
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
            # NI id
            if child.tag == self.__xml_tag_ni_id :
                self.ni_id = int( child.text )
            # Connect router
            elif child.tag == self.__xml_tag_ni_connect_router :
                self.connect_router = int( child.text )
            # Connect port
            elif child.tag == self.__xml_tag_ni_connect_port :
                self.connect_port = int( child.text )
            # Pipe cycle
            elif child.tag == self.__xml_tag_ni_pipe_cycle :
                self.pipe_cycle = float( child.text )
            # Size of eject buffer
            elif child.tag == self.__xml_tag_ni_buffer_size :
                self.buffer_size = int( child.text )
            # Delay of interrupt response.
            elif child.tag == self.__xml_tag_ni_interrupt_delay :
                self.interrupt_delay = float( child.text )

    ##
    # @brief Write router configuration to XML file.
    # @param root  root of XML structure.
    def writeXml( self, root : ET.Element ) -> None :
        # NI id
        child_item = ET.SubElement( root, self.__xml_tag_ni_id )
        child_item.text = str( self.ni_id )
        # Connect router
        child_item = ET.SubElement( root, self.__xml_tag_ni_connect_router )
        child_item.text = str( self.connect_router )
        # Connect port
        child_item = ET.SubElement( root, self.__xml_tag_ni_connect_port )
        child_item.text = str( self.connect_port )
        # Pipe cycle
        child_item = ET.SubElement( root, self.__xml_tag_ni_pipe_cycle )
        child_item.text = str( self.pipe_cycle )
        # Size of ejection size
        child_item = ET.SubElement( root, self.__xml_tag_ni_buffer_size )
        child_item.text = str( self.buffer_size )
        # Delay of interrupt response
        child_item = ET.SubElement( root, self.__xml_tag_ni_interrupt_delay )
        child_item.text = str( self.interrupt_delay )
    ##
    # @}

##
# @brief Function to set parameters
# @{
##
# @brief Construct an item for a template NI.
# @param pipe_cycle      Pipeline cycle.
# @param buffer_size     Size of ejection buffer.
# @param interrupt_delay Delay to response NI interrupt.
def createTemplateNI( pipe_cycle : float,
                      buffer_size : int,
                      interrupt_delay : float ) -> EsyNetCfgNI :
    net_cfg = EsyNetCfgNI()
    net_cfg.pipe_cycle      = float( pipe_cycle )
    net_cfg.buffer_size     = buffer_size
    net_cfg.interrupt_delay = float( interrupt_delay )
    return net_cfg
##
# @brief Construct an item for a NI
# @param ni_id           Network interface id.
# @param router          Connect router id.
# @param port            Connect port id.
# @param pipe_cycle      Pipeline cycle.
# @param buffer_size     Size of ejection buffer.
# @param interrupt_delay Delay to response NI interrupt.
def createNI( ni_id : int,
              connect_router : int,
              connect_port : int,
              pipe_cycle : float,
              buffer_size : int,
              interrupt_delay : float ) -> EsyNetCfgNI :
    net_cfg = EsyNetCfgNI()
    net_cfg.ni_id           = ni_id
    net_cfg.connect_router  = connect_router
    net_cfg.connect_port    = connect_port
    net_cfg.pipe_cycle      = float( pipe_cycle )
    net_cfg.buffer_size     = buffer_size
    net_cfg.interrupt_delay = float( interrupt_delay )
    return net_cfg
##
# @}
