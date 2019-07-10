/*
 * File name : esy_netcfg.cc
 * Function : Implement network configuration structure.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>
 */

#include <iomanip>

#include "esy_conv.h"
#include "esy_netcfg.h"

#define NET_ARGU_DECLARATION_VERSION  "1.0"
#define NET_ARGU_DECLARATION_ENCODING  "UTF-8"

#define NET_ARGU_ROOT   "networkcfg"
#define NET_ARGU_TOPOLOGY   "topology"
#define NET_ARGU_NETWORK_SIZE   "size"
#define NET_ARGU_TEMPLATE_ROUTER    "template_router_cfg"
#define NET_ARGU_TEMPLATE_NI    "template_ni_cfg"
#define NET_ARGU_ROUTER "router_cfg"
#define NET_ARGU_NI "ni_cfg"

#define NET_ARGU_DATA   "data"
#define NET_ARGU_SIZE   "size"
#define NET_ARGU_INDEX  "index"

std::string EsyNetworkCfg::const_noc_topology[ NT_COUNT ] =
    { "Switch", "Ring", "2D-Mesh", "2D-Torus", "MD-Mesh", "MD-Torus", "Irregular" };
std::string EsyNetworkCfg::const_axis_dir[ AX_NUM ] = { "X", "Y", "Z" };

EsyNetworkCfg::EsyNetworkCfg( const EsyNetworkCfg & t )
    : m_topology( t.m_topology )
    , m_size( t.m_size )
    , m_template_router( t.m_template_router )
    , m_template_ni( t.m_template_ni )
    , m_router( t.m_router )
    , m_ni( t.m_ni )
{}

EsyNetworkCfg::EsyNetworkCfg( NoCTopology topology, const std::vector< long >& size
        , double pipe_cycle, long phy_port, long input_vc_num, long output_vc_num, long input_buffer, long output_buffer
        , double ni_pipe_cycle, long ni_buffer, double ni_interrupt_delay
)
    : m_topology( topology )
    , m_size( size )
    , m_template_router( pipe_cycle, phy_port, input_vc_num, output_vc_num, input_buffer, output_buffer )
    , m_template_ni( ni_pipe_cycle, ni_buffer, ni_interrupt_delay )
    , m_router()
    , m_ni()
{
    // Set topology and valid size.
    setTopology( topology );
    bool t_2d = ( topology == NT_MESH_2D ) || ( topology == NT_TORUS_2D )
        || ( topology == NT_MESH_DIA && dim() == 2 ) || ( topology == NT_TORUS_DIA && dim() == 2 );
    bool t_3d = ( topology == NT_MESH_DIA && dim() == 3 ) || ( topology == NT_TORUS_DIA && dim() == 3 );
    bool t_irregular = ( topology == NT_IRREGULAR );

    // Configure ports.
    for ( int t_port = 0; t_port < phy_port; t_port ++ )
    {
        EsyNetworkCfgPort& port = m_template_router.port( t_port );

        // Set parameters: input/output virtual channel, input/output buffer size.
        port.setInputVcNumber( input_vc_num );
        port.setOutputVcNumber( output_vc_num );
        port.setInputBuffer( input_buffer );
        port.setOutputBuffer( output_buffer );

        // NI port
        if ( t_port == 0 || m_topology == NT_SWITCH )
        {
            port.setPortAxis( 0 );
            port.setPortAxisDir( EsyNetworkCfgPort::AXDIR_DOWN );
            port.setPortDirection( EsyNetworkCfgPort::ROUTER_PORT_SOUTHWEST );
            port.setNetworkInterface( true );
            continue;
        }
        // If the topology is irregular topology, does not connect.
        else if ( t_irregular )
        {
            continue;
        }

        // Calculate the axis for the port. 0 is for NI. 1-2 is for axis dim-1-0, 3-4 is for axis dim-1-1, and so on.
        long t_dim = dim();
        long t_axis = t_dim - 1 - ( ( t_port - 1 ) / 2 ) % t_dim;
        port.setPortAxis( t_axis );
        // If port is even, increase direction; otherwise, decrease direction.
        port.setPortAxisDir( ( t_port % 2 == 1 ) ? EsyNetworkCfgPort::AXDIR_DOWN : EsyNetworkCfgPort::AXDIR_UP );
        // Set direction on GUI
        if ( t_2d )
        {
            if ( port.portAxis() == 0 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_NORTH : EsyNetworkCfgPort::ROUTER_PORT_SOUTH );
            }
            else
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_WEST : EsyNetworkCfgPort::ROUTER_PORT_EAST );
            }
        }
        // 3D topology, North, south, northwest, southeast, west, east;
        else if ( t_3d )
        {
            if ( port.portAxis() == 0 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_NORTH : EsyNetworkCfgPort::ROUTER_PORT_SOUTH );
            }
            else if ( port.portAxis() == 1 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_NORTHWEST : EsyNetworkCfgPort::ROUTER_PORT_SOUTHEAST );
            }
            else if ( port.portAxis() == 2 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_WEST : EsyNetworkCfgPort::ROUTER_PORT_EAST );
            }
        }
        // otherwise topology, North, south
        else
        {
            port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN ) ?
                EsyNetworkCfgPort::ROUTER_PORT_NORTH : EsyNetworkCfgPort::ROUTER_PORT_SOUTH );
        }

        // set Size of input and output buffer
        // Set port 0 as NI port.
        port.setNetworkInterface( false );
    }

    // Generate configuration for all router.
    updateNetwork();
}

void EsyNetworkCfg::readXml( TiXmlElement * root )
{
    // Loop all item behind root.
    for ( TiXmlNode * p_child = root->FirstChild(); p_child != NULL; p_child = p_child->NextSibling() )
    {
        TiXmlElement * t_element = p_child->ToElement();

        // Topology element
        if ( t_element->Value() == std::string( NET_ARGU_TOPOLOGY ) )
        {
            long t = EsyConvert( t_element->GetText() );
            m_topology = ( NoCTopology )t;
        }

        // Network size element.
        if ( t_element->Value() == std::string( NET_ARGU_NETWORK_SIZE ) )
        {
            m_size.resize( EsyConvert( t_element->Attribute( NET_ARGU_SIZE ) ) );
            // Loop all item behind network size.
            for ( TiXmlNode * p_size_child = t_element->FirstChild(); p_size_child != NULL; p_size_child = p_size_child->NextSibling() )
            {
                // Network size on specified diamension.
                TiXmlElement * t_size_element = p_size_child->ToElement();
                m_size[ EsyConvert( t_size_element->Attribute( NET_ARGU_INDEX ) ) ] = EsyConvert( t_size_element->GetText() );
            }
        }

        // Template router element.
        if ( t_element->Value() == std::string( NET_ARGU_TEMPLATE_ROUTER ) )
        {
            m_template_router.readXml( t_element );
        }

        // Template router element.
        if ( t_element->Value() == std::string( NET_ARGU_TEMPLATE_NI ) )
        {
            m_template_ni.readXml( t_element );
        }

        // Special router elements.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER ) )
        {
            m_router.resize( EsyConvert( t_element->Attribute( NET_ARGU_SIZE ) ) );
            // Loop for each special router.
            for ( TiXmlNode * p_router_child = t_element->FirstChild(); p_router_child != NULL; p_router_child = p_router_child->NextSibling() )
            {
                TiXmlElement * t_router_element = p_router_child->ToElement();
                m_router[ EsyConvert( t_router_element->Attribute( NET_ARGU_INDEX ) ) ].readXml( t_router_element );
            }
        }

        // Special router elements.
        if ( t_element->Value() == std::string( NET_ARGU_NI ) )
        {
            m_ni.resize( EsyConvert( t_element->Attribute( NET_ARGU_SIZE ) ) );
            // Loop for each special router.
            for ( TiXmlNode * p_router_child = t_element->FirstChild(); p_router_child != NULL; p_router_child = p_router_child->NextSibling() )
            {
                TiXmlElement * t_ni_element = p_router_child->ToElement();
                m_ni[ EsyConvert( t_ni_element->Attribute( NET_ARGU_INDEX ) ) ].readXml( t_ni_element );
            }
        }
    }
}

void EsyNetworkCfg::writeXml( TiXmlElement * root )
{
    // Topology element.
    TiXmlElement * t_item = new TiXmlElement( NET_ARGU_TOPOLOGY );
    t_item->LinkEndChild( new TiXmlText( EsyConvert( m_topology ).c_str() ) );
    root->LinkEndChild( t_item );

    // Network size element.
    TiXmlElement * t_size_item = new TiXmlElement( NET_ARGU_NETWORK_SIZE );
    t_size_item->SetAttribute( NET_ARGU_SIZE, (int)m_size.size() );
    for ( std::size_t i = 0; i < m_size.size(); i ++ )
    {
        // Size on diamension.
        TiXmlElement * t_data_item = new TiXmlElement( NET_ARGU_DATA );
        t_data_item->SetAttribute( NET_ARGU_INDEX, (int)i );
        t_data_item->LinkEndChild( new TiXmlText( EsyConvert( m_size[ i ] ).c_str() ) ) ;
        t_size_item->LinkEndChild( t_data_item );
    }
    root->LinkEndChild( t_size_item );

    // Template router.
    TiXmlElement * t_template_router_item = new TiXmlElement( NET_ARGU_TEMPLATE_ROUTER );
    m_template_router.writeXml( t_template_router_item );
    root->LinkEndChild( t_template_router_item );

    // Template router.
    TiXmlElement * t_template_ni_item = new TiXmlElement( NET_ARGU_TEMPLATE_NI );
    m_template_router.writeXml( t_template_ni_item );
    root->LinkEndChild( t_template_ni_item );

    // Special routers.
    TiXmlElement * t_router_list_item = new TiXmlElement( NET_ARGU_ROUTER );
    t_router_list_item->SetAttribute( NET_ARGU_SIZE, (int)m_router.size() );
    for ( std::size_t i = 0; i < m_router.size(); i ++ )
    {
        TiXmlElement * t_router_item = new TiXmlElement( NET_ARGU_DATA );
        t_router_item->SetAttribute( NET_ARGU_INDEX, (int)i );
        m_router[ i ] .writeXml( t_router_item );
        t_router_list_item->LinkEndChild( t_router_item );
    }
    root->LinkEndChild( t_router_list_item );

    // Special ni.
    TiXmlElement * t_ni_list_item = new TiXmlElement( NET_ARGU_NI );
    t_ni_list_item->SetAttribute( NET_ARGU_SIZE, (int)m_ni.size() );
    for ( std::size_t i = 0; i < m_ni.size(); i ++ )
    {
        TiXmlElement * t_ni_item = new TiXmlElement( NET_ARGU_DATA );
        t_ni_item->SetAttribute( NET_ARGU_INDEX, (int)i );
        m_ni[ i ] .writeXml( t_ni_item );
        t_ni_list_item->LinkEndChild( t_ni_item );
    }
    root->LinkEndChild( t_ni_list_item );
}

EsyXmlError EsyNetworkCfg::readXml( const std::string & fname )
{
    // Open and parse file.
    std::string tname = fname + "." + NETCFG_EXTENSION;
    TiXmlDocument t_doc( tname.c_str() );
    if ( !t_doc.LoadFile() )
    {
        return EsyXmlError( t_doc.ErrorDesc(), fname, EsyConvert( t_doc.ErrorRow() ), EsyConvert( t_doc.ErrorCol() ), "EsyNetworkCfg", "readXml" );
    }

    // Check root element.
    TiXmlElement * t_head = t_doc.RootElement();
    if ( t_head->Value() != std::string( NET_ARGU_ROOT ) )
    {
        return EsyXmlError( t_head->Value(), fname, "EsyNetworkCfg", "readXml" );
    }

    // Read network configuration.
    readXml( t_head );

    // Release memory.
    t_doc.Clear();

    return EsyXmlError();
}

EsyXmlError EsyNetworkCfg::writeXml( const std::string & fname )
{
    // Document structure.
    TiXmlDocument t_doc;
    // Declaration.
    TiXmlDeclaration * t_decl = new TiXmlDeclaration( NET_ARGU_DECLARATION_VERSION, NET_ARGU_DECLARATION_ENCODING, "" );
    t_doc.LinkEndChild( t_decl );

    // Root element.
    TiXmlElement * t_root = new TiXmlElement( NET_ARGU_ROOT );
    t_doc.LinkEndChild( t_root );

    // Network configuration.
    writeXml( t_root );

    // Write to file.
    std::string tname = fname + "." + NETCFG_EXTENSION;
    t_doc.SaveFile( tname.c_str() );

    // Relase memory.
    t_doc.Clear();

    return EsyXmlError();
}

void EsyNetworkCfg::setTopology( NoCTopology topology )
{
    m_topology = topology;
    // If the topology is 2D mesh/torus, resize diamension to 2D.
    if ( m_topology == NT_MESH_2D || m_topology == NT_TORUS_2D )
    {
        resizeDim( 2 );
        for ( int i = 0; i < dim(); i ++ )
        {
            if ( size( i ) == 0 )
            {
                setSize( i, 1 );
            }
        }
    }
    // If the topology is multiple diamension mesh/torus, do nothing.
    else if ( m_topology == NT_MESH_DIA || m_topology == NT_TORUS_DIA )
    {
        if ( dim() == 0 )
        {
            resizeDim( 1 );
        }
        for ( int i = 0; i < dim(); i ++ )
        {
            if ( size( i ) == 0 )
            {
                setSize( i, 1 );
            }
        }
    }
    // If the topology is ring, set diamension to 1 and set size to the number of routers.
    else if ( m_topology == NT_RING )
    {
        resizeDim( 1 );
    }
    // If the topology is switch, set diamension to 1 and set size to 1.
    else if ( m_topology == NT_SWITCH )
    {
        resizeDim( 1 );
        setSize( AX_X, 1 );
    }
}

long EsyNetworkCfg::coord2Seq( const std::vector< long > & coord )
{
    long seq_t = coord[ m_size.size() - 1 ];
    for ( std::size_t i = 1; i < m_size.size(); i ++ )
    {
        seq_t = ( seq_t * m_size[ m_size.size() - i - 1 ] ) +
            coord[ m_size.size() - i - 1 ];
    }
    return seq_t;
}

std::vector< long > EsyNetworkCfg::seq2Coord( long seq )
{
    long seq_t = seq;
    std::vector< long > coord;
    for ( std::size_t i = 0; i < m_size.size(); i ++ )
    {
        coord.push_back( seq_t % m_size[ i ] );
        seq_t = seq_t / m_size[ i ];
    }
    return coord;
}

int EsyNetworkCfg::coordDistance( long p1, long p2 )
{
    std::vector< long > t_p1 = seq2Coord( p1 );
    std::vector< long > t_p2 = seq2Coord( p2 );
    int dist = 0;
    for ( std::size_t i = 0; i < m_size.size(); i ++ )
    {
        dist = dist + abs( t_p1[ i ] - t_p2[ i ] );
    }
    return dist;
}

void EsyNetworkCfg::updateNetwork()
{
    // Clear routers and NIs
    m_router.clear();
    m_ni.clear();

    // Calculate router count;
    long router_count = 1;
    for ( int i = 0; i < m_size.size(); i ++ )
    {
        router_count *= m_size[ i ];
    }

    // Generate router and NIs
    for ( long id = 0; id < router_count; id ++ )
    {
        // Copy router configuration from template router.
        EsyNetworkCfgRouter t_router_cfg( id, m_template_router.pipeCycle(), m_template_router.pos(), m_template_router.port() );

        // Ger coordinary of the router.
        std::vector< long > t_ax = seq2Coord( t_router_cfg.routerId() );

        // generate NI structure
        for ( long t_port_index = 0; t_port_index < t_router_cfg.portNum(); t_port_index ++ )
        {
            if ( t_router_cfg.port( t_port_index ).networkInterface() )
            {
                long ni_id = m_ni.size();
                // generate ni structure
                EsyNetworkCfgNI t_ni_cfg( ni_id, id, t_port_index
                    , m_template_ni.pipeCycle(), m_template_ni.bufferSize(), m_template_ni.interruptDelay() );
                m_ni.push_back( t_ni_cfg );
                // assign connection
                EsyNetworkCfgPort& t_port = t_router_cfg.port( t_port_index );
                t_port.setNeighborRouter( ni_id );
                t_port.setNeighborPort( 0 );
                continue;
            }
        }

        // Disable port for mesn topology
        if ( m_topology == NT_MESH_2D || m_topology == NT_MESH_DIA )
        {
            for ( long t_port_index = 0; t_port_index < t_router_cfg.portNum(); t_port_index ++ )
            {
                EsyNetworkCfgPort& t_port = t_router_cfg.port( t_port_index );
                // The decrease direction port at the first router on the axis or the increase direction port at the last router on the axis,
                // reset the vc number to 0 to close the port.
                long t_port_axis = t_port.portAxis();
                if ( ( t_ax[ t_port_axis ] == 0 && t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN )
                    || ( t_ax[ t_port_axis ]  == m_size[ t_port_axis ] - 1 && t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_UP )  )
                {
                    t_port.setInputVcNumber( 0 );
                    t_port.setOutputVcNumber( 0 );
                }
            }
        }

        // Assign port.
        for ( long t_port_index = 0; t_port_index < t_router_cfg.portNum(); t_port_index ++ )
        {
            EsyNetworkCfgPort& t_port = t_router_cfg.port( t_port_index );
            long t_port_axis = t_port.portAxis();
            // Port connects to NI, connect to the same port.
            if ( t_port.networkInterface() )
            {
            }
            // If the topology is irregular, do not assign port.
            else if ( topology() == NT_IRREGULAR )
            {
                t_port.setNeighborRouter( id );
                t_port.setNeighborPort( t_port_index );
            }
            // Decrease port, connect to the previous router on the axis, port + 1.
            else if ( t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DOWN )
            {
                std::vector< long > t_neighbor_ax = t_ax;
                if ( t_ax[ t_port_axis ] > 0 )
                {
                    t_neighbor_ax[ t_port_axis ] = t_neighbor_ax[ t_port_axis ] - 1;
                    t_port.setNeighborRouter( coord2Seq( t_neighbor_ax ) );
                    t_port.setNeighborPort( t_port_index + 1 );
                }
                // If the first router on the axis, loop to the last router.
                else if ( ( t_ax[ t_port_axis ] == 0 )
                    && ( m_topology == NT_TORUS_2D || m_topology == NT_TORUS_DIA || m_topology == NT_RING ) )
                {
                    t_neighbor_ax[ t_port_axis ] = m_size[ t_port_axis ] - 1;
                    t_port.setNeighborRouter( coord2Seq( t_neighbor_ax ) );
                    t_port.setNeighborPort( t_port_index + 1 );
                }
            }
            // Increase port, connect to the next router on the axis, port - 1.
            else if ( t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_UP )
            {
                std::vector< long > t_neighbor_ax = t_ax;
                if ( t_ax[ t_port_axis ] < m_size[ t_port_axis ] - 1 )
                {
                    t_neighbor_ax[ t_port_axis ] = t_neighbor_ax[ t_port_axis ] + 1;
                    t_port.setNeighborRouter( coord2Seq( t_neighbor_ax ) );
                    t_port.setNeighborPort( t_port_index - 1 );
                }
                // If the last router on the axis, loop to the first router.
                else if ( ( t_ax[ t_port_axis ] == m_size[ t_port_axis ] - 1 )
                    && ( m_topology == NT_TORUS_2D || m_topology == NT_TORUS_DIA || m_topology == NT_RING ) )
                {
                    t_neighbor_ax[ t_port_axis ] = 0;
                    t_port.setNeighborRouter( coord2Seq( t_neighbor_ax ) );
                    t_port.setNeighborPort( t_port_index - 1 );
                }
            }
        }

        m_router.push_back( t_router_cfg );
    }
}

long int EsyNetworkCfg::maxPcNum() const
{
    long pcnum = 1;
    for ( long i = 0; i < routerCount(); i ++ )
    {
        if ( m_router[ i ].portNum() > pcnum )
        {
            pcnum = m_router[ i ].portNum();
        }
    }
    return pcnum;
}

long int EsyNetworkCfg::maxInputVcNum() const
{
    long vcnum = 1;
    for ( long i = 0; i < routerCount(); i ++ )
    {
        if ( m_router[ i ].maxInputVcNum() > vcnum )
        {
            vcnum = m_router[ i ].maxInputVcNum();
        }
    }
    return vcnum;
}

long int EsyNetworkCfg::maxOutputVcNum() const
{
    long vcnum = 1;
    for ( long i = 0; i < routerCount(); i ++ )
    {
        if ( m_router[ i ].maxOutputVcNum() > vcnum )
        {
            vcnum = m_router[ i ].maxOutputVcNum();
        }
    }
    return vcnum;
}

long int EsyNetworkCfg::maxInputBuffer() const
{
    long inputbuffer = 1;
    for ( long i = 0; i < routerCount(); i ++ )
    {
        if ( m_router[ i ].maxInputBuffer() > inputbuffer )
        {
            inputbuffer = m_router[ i ].maxInputBuffer();
        }
    }
    return inputbuffer;
}

long int EsyNetworkCfg::maxOutputBuffer() const
{
    long outputbuffer = 1;
    for ( long i = 0; i < routerCount(); i ++ )
    {
        if ( m_router[ i ].maxOutputBuffer() > outputbuffer )
        {
            outputbuffer = m_router[ i ].maxOutputBuffer();
        }
    }
    return outputbuffer;
}

std::ostream& operator<<( std::ostream & os, const EsyNetworkCfg & net_cfg )
{
    os << "Topology=" << net_cfg.topologyStr() << std::endl;
    os << "Size={" << net_cfg.size( 0 );
    for ( int i = 1; i < net_cfg.dim(); i ++ )
    {
        os << "," << net_cfg.size( i );
    }
    os << "}" << std::endl;
    os << std::endl;

    os << "    Router    |                    Port                     |               NI" << std::endl;
    os << "              |       In  In   Out  Out Neig Neig      Axis |              Conn Conn        Int" << std::endl;
    os << "  ID   Pipe   |  ID   VC  Buf  VC   Buf R/NI Port Axis Dir  |  ID   Pipe   Rout Port  Buf  Delay" << std::endl;
    os << "--------------|---------------------------------------------|--------------------------------------" << std::endl;

    for ( int i = 0; i < net_cfg.routerCount(); i ++ )
    {
        const EsyNetworkCfgRouter& router_cfg = net_cfg.router( i );
        for ( int i = 0; i < router_cfg.portNum(); i ++ )
        {
            if ( i == 0 )
            {
                os << setw(4) << router_cfg.routerId() << " ";
                os << setw(8) << fixed << router_cfg.pipeCycle() << " |";
            }
            else
            {
                os << "              |";
            }

            const EsyNetworkCfgPort& port_cfg = router_cfg.port( i );
            os << setw(4) << i << " ";
            os << setw(4) << port_cfg.inputVcNumber() << " ";
            os << setw(4) << port_cfg.inputBuffer() << " ";
            os << setw(4) << port_cfg.outputVcNumber() << " ";
            os << setw(4) << port_cfg.outputBuffer() << " ";
            os << setw(4) << port_cfg.neighborRouter() << " ";
            os << setw(4) << port_cfg.neighborPort() << " ";
            if ( router_cfg.port( i ).networkInterface() )
            {
                os << "  NI" << "      |";
                const EsyNetworkCfgNI& ni_cfg = net_cfg.ni( port_cfg.neighborRouter() );
                os << setw(4) << ni_cfg.niId() << " ";
                os << setw(8) << fixed << ni_cfg.pipeCycle() << " ";
                os << setw(4) << ni_cfg.connectRouter() << " ";
                os << setw(4) << ni_cfg.connectPort() << " ";
                os << setw(4) << ni_cfg.bufferSize() << " ";
                os << setw(8) << fixed << ni_cfg.interruptDelay() << " ";
            }
            else
            {
                os << setw(4) << port_cfg.portAxis() << " ";
                os << setw(8) << port_cfg.portAxisDirectionStr();
            }
            os << std::endl;
        }
    }

    return os;
}
