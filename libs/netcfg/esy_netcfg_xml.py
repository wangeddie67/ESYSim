import sys
sys.path.append('../../build/libs/netcfg')
import py_esy_netcfg as netcfg

import xml.etree.ElementTree as ET

__xml_tag_vector = 'vector'

__xml_tag_netcfg = 'netcfg'
__xml_tag_topology = 'topology'
__xml_tag_size     = 'size'
__xml_tag_links    = 'links'

__xml_tag_link = 'link'
__xml_tag_link_dir      = 'dir'
__xml_tag_link_axis     = 'axis'
__xml_tag_link_axis_dir = 'axis_dir'

__xml_tag_port = 'port'
__xml_tag_port_id        = 'id'
__xml_tag_port_port      = 'port'
__xml_tag_port_ni        = 'ni'
__xml_tag_port_bufs      = 'bufs'
__xml_tag_port_outperiod = 'outperiod'

def readNetCfgPortFromXml( root : ElementTree ) :
    pass

def readNetCfgLinkFromXml( root : ElementTree ) :
    pass

def readNetCfgFromXml( root : ElementTree ) -> py_esy_netcfg.EsyNetCfg :
    pass

def readXml( xml_file : str ) -> py_esy_netcfg.EsyNetCfg :
    tree = ET.parse( xml_file )
    root = tree.getroot()

    esy_netcfg = readNetCfgFromXml( root )
    return esy_netcfg


def writeNetCfgPortToXml( root : ElementTree, portcfg : py_esy_netcfg.EsyNetCfgPort ) :
    port_root = ET.SubElement( root, __xml_tag_port )

    id_element = ET.SubElement( port_root, __xml_tag_port_id )
    id_element.text = str( portcfg.id() )
    port_element = ET.SubElement( port_root, __xml_tag_port_port )
    port_element.text = str( portcfg.port() )
    ni_element = ET.SubElement( port_root, __xml_tag_port_ni )
    ni_element.text = str( portcfg.isNi() )
    period_element = ET.SubElement( port_root, __xml_tag_port_outperiod )
    period_element.text = str( portcfg.outPeriod() )
    bufs_element = ET.SubElement( port_root, __xml_tag_port_bufs )
    for buf in portcfg.bufs() :
        bufs_element = ET.SubElement( bufs_element, __xml_tag_vector )
        bufs_element.text = str( buf )

def writeNetCfgLinkToXml( root : ElementTree, linkcfg : py_esy_netcfg.EsyNetCfgLink ) :
    link_root = ET.SubElement( root, __xml_tag_link )

    dir_element = ET.SubElement( link_root, __xml_tag_link_dir )
    dir_element.text = str( linkcfg.dir() )
    axis_element = ET.SubElement( link_root, __xml_tag_link_axis )
    axis_element.text = str( linkcfg.axis() )
    axis_dir_element = ET.SubElement( link_root, __xml_tag_link_axis_dir )
    axis_dir_element.text = str( linkcfg.axisDir() )

    writeNetCfgPortToXml( link_root, linkcfg.inputPort() )
    writeNetCfgPortToXml( link_root, linkcfg.outputPort() )

def writeNetCfgToXml( root : ElementTree, netcfg : py_esy_netcfg.EsyNetCfg ) :
    topo_element = ET.SubElement( root, __xml_tag_topology )
    topo_element.text = netcfg.topology()
    size_element = ET.SubElement( root, __xml_tag_size )
    for t_size in netcfg.size() :
        size_element = ET.SubElement( size_element, __xml_tag_vector )
        size_element.text = int( t_size )

    links_element = ET.SubElement( root, __xml_tag_links )
    for link in netcfg.links() :
        writeNetCfgLinkToXml( links_element, link )

def writeXml( xml_file : str, netcfg : py_esy_netcfg.EsyNetCfg ) :
    tree = ET.ElementTree( __xml_tag_netcfg )
    root = tree.getroot()
    writeNetCfgToXml( root, netcfg )
    tree.write( xml_file, encoding='utf-8', xml_declaration=True )
