##
# File name : esy_netcfg_port.py
# Function : network configuration structure for ports.
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
# @brief Structure of port configuration item.
class EsyNetCfgPort :

    ##
    # @name Port direction enumerate
    # @{
    port_dir_enum_north     = 'North'
    port_dir_enum_south     = 'South'
    port_dir_enum_west      = 'West'
    port_dir_enum_east      = 'East'
    port_dir_enum_northwest = 'NorthWest'
    port_dir_enum_northeast = 'NorthEast'
    port_dir_enum_southwest = 'SouthWest'
    port_dir_enum_southeast = 'SouthEast'
    __port_dir_enum = [ port_dir_enum_north \
                      , port_dir_enum_south \
                      , port_dir_enum_west \
                      , port_dir_enum_east \
                      , port_dir_enum_northwest \
                      , port_dir_enum_northeast \
                      , port_dir_enum_southwest \
                      , port_dir_enum_southeast ]
    __port_dir_short_dict = { port_dir_enum_north     : 'N' \
                            , port_dir_enum_south     : 'S' \
                            , port_dir_enum_west      : 'W' \
                            , port_dir_enum_east      : 'E' \
                            , port_dir_enum_northwest : 'NW' \
                            , port_dir_enum_northeast : 'NE' \
                            , port_dir_enum_southwest : 'SW' \
                            , port_dir_enum_southeast : 'SE' }
    ##
    # @}

    ##
    # @name Port axis direction enumerate
    # @{
    port_axis_dir_enum_up   = 'Upward'
    port_axis_dir_enum_down = 'Downward'
    __port_axis_dir_enum = [ port_axis_dir_enum_up \
                           , port_axis_dir_enum_down ]
    ##
    # @}

    ##
    # @brief Constructs an empty item with default value.
    def __init__( self ) :
        self.port_id = 0
        self.input_vc_num = 1
        self.output_vc_num = 1
        self.port_dir = self.port_dir_enum_southwest
        self.port_axis = 0
        self.port_axis_dir = self.port_axis_dir_enum_down
        self.neighbor_router = -1
        self.neighbor_port = -1
        self.input_buffer = 12
        self.output_buffer = 12
        self.network_interface = False

    ##
    # @brief Function to set parameters
    # @{
    ##
    # @brief Construct an item for a template port.
    # @param input_vc_num  Number of input virtual channel.
    # @param output_vc_num Number of output virtual channel.
    # @param input_buffer  Size of input buffer.
    # @param output_buffer Size of output buffer.
    def createTemplatePort( self, input_vc_num, output_vc_num, \
                            input_buffer, output_buffer ) :
        assert isinstance( input_vc_num, int ), 'input_vc_num must be an integer'
        assert isinstance( output_vc_num, int ), 'output_vc_num must be an integer'
        assert isinstance( input_buffer, int ), 'input_buffer must be an integer'
        assert isinstance( output_buffer, int ), 'output_buffer must be an integer'
        self.input_vc_num = input_vc_num
        self.output_vc_num = output_vc_num
        self.input_buffer = input_buffer
        self.output_buffer = output_buffer
    ##
    # @brief Construct an item for a port
    # @param port_id           Port index.
    # @param input_vc_num      Number of input virtual channel.
    # @param output_vc_num     Number of output virtual channel.
    # @param dir               Port direction in GUI.
    # @param axis              Axis of the port.
    # @param axis_dir          Direction of port on axis.
    # @param neighbor_router   Neighbor router.
    # @param neighbor_port     Neighbor port.
    # @param input_buffer      Size of input buffer.
    # @param output_buffer     Size of output buffer.
    # @param ni                Network interface flag of connection.
    def createPort( self, port_id, input_vc_num, output_vc_num, \
                    port_dir, port_axis, port_axis_dir, \
                    neighbor_router, neighbor_port, \
                    input_buffer, output_buffer, ni ) :
        assert isinstance( port_id, int ), 'port_id must be an integer'
        assert isinstance( input_vc_num, int ), 'input_vc_num must be an integer'
        assert isinstance( output_vc_num, int ), 'output_vc_num must be an integer'
        assert port_dir in self.__port_dir_enum, 'port_dir must be one of ' + str( self.__port_dir_enum )
        assert isinstance( port_axis, int ), 'port_axis must be an integer'
        assert port_axis_dir in self.__port_axis_dir_enum, 'port_dir must be one of ' + str( self.__port_axis_dir_enum )
        assert isinstance( neighbor_router, int ), 'neighbor_router must be an integer'
        assert isinstance( neighbor_port, int ), 'neighbor_port must be an integer'
        assert isinstance( input_buffer, int ), 'input_buffer must be an integer'
        assert isinstance( output_buffer, int ), 'output_buffer must be an integer'
        assert isinstance( ni, (int, bool) ), 'output_buffer must be True or False'
        self.port_id = port_id
        self.input_vc_num = input_vc_num
        self.port_dir = port_dir
        self.port_axis = port_axis
        self.port_axis_dir = port_axis_dir
        self.neighbor_router = neighbor_router
        self.neighbor_port = neighbor_port
        self.input_buffer = input_buffer
        self.output_buffer = output_buffer
        self.network_interface = bool( ni )
    ##
    # @}

    ##
    # @brief Access the short name of specified direction.
    # @brief dir code of direction.
    def portDirection2ShortStr( self, dir ) :
        return self.__port_dir_short_dict[ dir ]

    ##
    # @brief Return total virtual channel. Sum of input and output virtual channel.
    def totalVcNumber( self ) :
        return self.input_vc_num + self.output_vc_num

    ##
    # @name Tag string for XML file
    # @{
    __xml_tag_port_id                = 'id'
    __xml_tag_port_input_vc          = 'input_vc'
    __xml_tag_port_output_vc         = 'output_vc'
    __xml_tag_port_direction         = 'port_dir'
    __xml_tag_port_axis              = 'port_axis'
    __xml_tag_port_axis_dir          = 'port_axis_dir'
    __xml_tag_port_neighbor_id       = 'neighbor_id'
    __xml_tag_port_neighbor_port     = 'neighbor_port'
    __xml_tag_port_input_buffer      = 'input_buffer'
    __xml_tag_port_output_buffer     = 'output_buffer'
    __xml_tag_port_network_interface = 'ni'
    ##
    # @}

    ##
    # @name XML function
    ##
    # @brief Read port configuration from XML file.
    # @param root  root of XML structure.
    def readXml( self, root ) :
        assert isinstance( root, ET.Element ), 'root must an entity of xml.etree.ElementTree.Element'
        # Loop all child nodes
        child_node_list = root.getchildren()
        for child in child_node_list :
            # NI id
            if child.tag == self.__xml_tag_port_id :
                self.port_id = int( child.text )
            # Number of input virtual channel.
            elif child.tag == self.__xml_tag_port_input_vc :
                self.input_vc_num = int( child.text )
            # Number of output virtual channel.
            elif child.tag == self.__xml_tag_port_output_vc :
                self.output_vc_num = int( child.text )
            # Port direction in GUI.
            elif child.tag == self.__xml_tag_port_direction :
                self.port_dir = child.text
                assert self.port_dir in self.__port_dir_enum, 'Invalid value for port direction :' + self.port_dir
            # Port axis.
            elif child.tag == self.__xml_tag_port_axis :
                self.port_axis = int( child.text )
            # Direction on port axis.
            elif child.tag == self.__xml_tag_port_axis_dir :
                self.port_axis_dir = child.text
                assert self.port_axis_dir in self.__port_axis_dir_enum, 'Invalid value for port axis direction :' + self.port_axis_dir
            # Neighbor router id.
            elif child.tag == self.__xml_tag_port_neighbor_id :
                self.neighbor_router = int( child.text )
            # Neighbor router port.
            elif child.tag == self.__xml_tag_port_neighbor_port :
                self.neighbor_port = int( child.text )
            # Size of input buffer.
            elif child.tag == self.__xml_tag_port_input_buffer :
                self.input_buffer = int( child.text )
            # Size of output buffer.
            elif child.tag == self.__xml_tag_port_output_buffer :
                self.output_buffer = int( child.text )
            # Network interface.
            elif child.tag == self.__xml_tag_port_network_interface :
                self.network_interface = int( child.text )

    ##
    # @brief Write port configuration to XML file.
    # @param root  root of XML structure.
    def writeXml( self, root ) :
        assert isinstance( root, ET.Element ), 'root must an entity of xml.etree.ElementTree.Element'
        # Port id
        child_item = ET.SubElement( root, self.__xml_tag_port_id )
        child_item.text = str( self.port_id )
        # Number of input virtual channel.
        child_item = ET.SubElement( root, self.__xml_tag_port_input_vc )
        child_item.text = str( self.input_vc_num )
        # Number of output virtual channel.
        child_item = ET.SubElement( root, self.__xml_tag_port_output_vc )
        child_item.text = str( self.output_vc_num )
        # Port direction in GUI.
        child_item = ET.SubElement( root, self.__xml_tag_port_direction )
        child_item.text = self.port_dir
        # Axis of port.
        child_item = ET.SubElement( root, self.__xml_tag_port_axis )
        child_item.text = str( self.port_axis )
        # Direction on axis.
        child_item = ET.SubElement( root, self.__xml_tag_port_axis_dir )
        child_item.text = self.port_axis_dir
        # Neighbor router id.
        child_item = ET.SubElement( root, self.__xml_tag_port_neighbor_id )
        child_item.text = str( self.neighbor_router )
        # Neighbor router port.
        child_item = ET.SubElement( root, self.__xml_tag_port_neighbor_port )
        child_item.text = str( self.neighbor_port )
        # Size of input buffer.
        child_item = ET.SubElement( root, self.__xml_tag_port_input_buffer )
        child_item.text = str( self.input_buffer )
        # Size of output buffer.
        child_item = ET.SubElement( root, self.__xml_tag_port_output_buffer )
        child_item.text = str( self.output_buffer )
        # Network interface, 0 or 1.
        child_item = ET.SubElement( root, self.__xml_tag_port_network_interface )
        child_item.text = str( int( self.network_interface ) )
