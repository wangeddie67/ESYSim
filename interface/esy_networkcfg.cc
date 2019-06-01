/*
 Copyright @ UESTC

 ESY-series Many-core System-on-chip Simulation Environment is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.

 ESY-series Many-core System-on-chip Simulation Environment is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/.
 */

#include "esy_conv.h"
#include "esy_networkcfg.h"
#include "tinyxml.h"

#define readItemLong( item, value, var ) \
if ( item->Value() == string( value ) ) \
{ \
    var = EsyConvert( item->GetText() ); \
}
#define readItemString( item, value, var ) \
if ( item->Value() == string( value ) ) \
{ \
    var = string( item->GetText() ); \
}
#define readItemEnum( item, value, var, type ) \
if ( item->Value() == string( value ) ) \
{ \
    long t = EsyConvert( item->GetText() ); \
    var = (type)t; \
}

#define writeItem( root, value, var ) \
TiXmlElement * t_##var##_item = new TiXmlElement( value );\
t_##var##_item->LinkEndChild( \
    new TiXmlText( EsyConvert( var ).c_str() ) ); \
root->LinkEndChild( t_##var##_item ); \

#define NET_ARGU_DECLARATION_VERSION  "1.0"
#define NET_ARGU_DECLARATION_ENCODING  "UTF-8"

#define NET_ARGU_ROOT   "networkcfg"
#define NET_ARGU_TOPOLOGY   "topology"
#define NET_ARGU_NETWORK_SIZE   "size"
#define NET_ARGU_DATA_WIDTH "data_width"
#define NET_ARGU_TEMPLATE_ROUTER    "template_router_cfg"
#define NET_ARGU_ROUTER "router_cfg"
#define NET_ARGU_ROUTER_ID  "id"
#define NET_ARGU_ROUTER_POS "position"
#define NET_ARGU_ROUTER_POS_X   "x"
#define NET_ARGU_ROUTER_POS_Y   "y"
#define NET_ARGU_ROUTER_PIPE    "pipe_cycle"
#define NET_ARGU_ROUTER_PORT    "port_cfg"
#define NET_ARGU_ROUTER_PORT_INPUT_VC   "input_vc"
#define NET_ARGU_ROUTER_PORT_OUTPUT_VC  "output_vc"
#define NET_ARGU_ROUTER_PORT_DIRECTION  "port_dir"
#define NET_ARGU_ROUTER_PORT_AXIS   "port_axis"
#define NET_ARGU_ROUTER_PORT_AXIS_DIRECTION "port_axis_dir"
#define NET_ARGU_ROUTER_PORT_NEIGHBOR_ID    "neighbor_id"
#define NET_ARGU_ROUTER_PORT_NEIGHBOR_PORT  "neighbor_port"
#define NET_ARGU_ROUTER_PORT_INPUT_BUFFER   "input_buffer"
#define NET_ARGU_ROUTER_PORT_OUTPUT_BUFFER  "output_buffer"
#define NET_ARGU_ROUTER_PORT_NETWORK_INTERFACE  "ni"

#define NET_ARGU_DATA   "data"
#define NET_ARGU_SIZE   "size"
#define NET_ARGU_INDEX  "index"

#define DEFAULT_TOPOLOGY  NT_SWITCH
#define DEFAULT_NETWORK_DIM  1
#define DEFAULT_NETWORK_SIZE  1
#define DEFAULT_ROUTER_COUNT 1
#define DEFAULT_NETWORK_DATA_WIDTH 32
#define DEFAULT_ROUTER_ID    0
#define DEFAULT_PIPE_CYCLE  1.0
#define DEFAULT_PHY_NUM  0
#define DEFAULT_POSITION  0.0
#define DEFAULT_INPUT_VC_NUM  1
#define DEFAULT_OUTPUT_VC_NUM  1
#define DEFAULT_PORT_DIRECTION  ROUTER_PORT_SOUTHEAST
#define DEFAULT_PORT_AXIS 0
#define DEFAULT_PORT_AXIS_DIRECTION AXDIR_DEC
#define DEFAULT_INPUT_BUFFER  12
#define DEFAULT_OUTPUT_BUFFER  12
#define DEFAULT_NEIGHBOR_ID  -1
#define DEFAULT_NEIGHBOR_PORT  -1
#define DEFAULT_NETWORK_INTERFACE  false

string EsyNetworkCfgPort::const_port_direction[ ROUTER_PORT_NUM ] =
    { "North", "South", "West", "East", "NorthWest", "NorthEast", "SouthWest", "SouthEast" };

string EsyNetworkCfgPort::const_port_direction_short[ ROUTER_PORT_NUM ] =
    { "N", "S", "W", "E", "NW", "NE", "SW", "SE" };

string EsyNetworkCfg::const_noc_topology[ NT_COUNT ] =
    { "Switch", "Ring", "2D-Mesh", "2D-Torus", "MD-Mesh", "MD-Torus", "Irregular" };

string EsyNetworkCfg::const_axis_dir[ AX_NUM ] = { "X", "Y" };

EsyNetworkCfgPort::EsyNetworkCfgPort()
    : m_input_vc_num( DEFAULT_INPUT_VC_NUM )
    , m_output_vc_num( DEFAULT_OUTPUT_VC_NUM )
    , m_port_dir( DEFAULT_PORT_DIRECTION )
    , m_port_axis( DEFAULT_PORT_AXIS )
    , m_port_axis_dir( DEFAULT_PORT_AXIS_DIRECTION )
    , m_neighbor_router( DEFAULT_NEIGHBOR_ID )
    , m_neighbor_port( DEFAULT_NEIGHBOR_PORT )
    , m_input_buffer( DEFAULT_INPUT_BUFFER )
    , m_output_buffer( DEFAULT_OUTPUT_BUFFER )
    , m_network_interface( DEFAULT_NETWORK_INTERFACE )
{}

EsyNetworkCfgPort::EsyNetworkCfgPort( int input_vc_num, int output_vc_num
        , RouterPortDirection dir, long axis, RouterPortAxisDirection axis_dir
        , int input_buffer, int output_buffer, bool ni )
    : m_input_vc_num( input_vc_num )
    , m_output_vc_num( output_vc_num )
    , m_port_dir( dir )
    , m_port_axis( axis )
    , m_port_axis_dir( axis_dir )
    , m_neighbor_router( -1 )
    , m_neighbor_port( -1 )
    , m_input_buffer( input_buffer )
    , m_output_buffer( output_buffer )
    , m_network_interface( ni )
{}

EsyNetworkCfgPort::EsyNetworkCfgPort( const EsyNetworkCfgPort & t )
    : m_input_vc_num( t.m_input_vc_num )
    , m_output_vc_num( t.m_output_vc_num )
    , m_port_dir( t.m_port_dir )
    , m_port_axis( t.m_port_axis )
    , m_port_axis_dir( t.m_port_axis_dir )
    , m_neighbor_router( t.m_neighbor_router )
    , m_neighbor_port( t.m_neighbor_port )
    , m_input_buffer( t.m_input_buffer )
    , m_output_buffer( t.m_output_buffer )
    , m_network_interface( t.m_network_interface )
{}

void EsyNetworkCfgPort::readXml( TiXmlElement * root )
{
    // Loop all item behind root.
    for ( TiXmlNode * p_child = root->FirstChild(); p_child != NULL; p_child = p_child->NextSibling() )
    {
        TiXmlElement * t_element = p_child->ToElement();

        // Number of input virtual channel.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_INPUT_VC, m_input_vc_num )

        // Number of output virtual channel.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_OUTPUT_VC, m_output_vc_num )

        // Port direction in GUI.
        readItemEnum( t_element, NET_ARGU_ROUTER_PORT_DIRECTION, m_port_dir, RouterPortDirection )

        // Port axis.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_AXIS, m_port_axis )

        // Direction on port axis.
        readItemEnum( t_element, NET_ARGU_ROUTER_PORT_AXIS_DIRECTION, m_port_axis_dir, RouterPortAxisDirection )

        // Neighbor router id.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_NEIGHBOR_ID, m_neighbor_router )

        // Neighbor router port.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_NEIGHBOR_PORT, m_neighbor_port )

        // Size of input buffer.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_INPUT_BUFFER, m_input_buffer )

        // Size of output buffer.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_OUTPUT_BUFFER, m_output_buffer )

        // Network interface.
        readItemLong( t_element, NET_ARGU_ROUTER_PORT_NETWORK_INTERFACE, m_network_interface )
    }
}

void EsyNetworkCfgPort::writeXml( bool full, TiXmlElement * root )
{
    // Number of input virtual channel.
    writeItem( root, NET_ARGU_ROUTER_PORT_INPUT_VC, m_input_vc_num  )

    // Number of output virtual channel.
    writeItem( root, NET_ARGU_ROUTER_PORT_OUTPUT_VC, m_output_vc_num )

    // Port direction in GUI.
    writeItem( root, NET_ARGU_ROUTER_PORT_DIRECTION, m_port_dir )

    // Axis of port.
    writeItem( root, NET_ARGU_ROUTER_PORT_AXIS, m_port_axis )

    // Direction on axis.
    writeItem( root, NET_ARGU_ROUTER_PORT_AXIS_DIRECTION, m_port_axis_dir )

    // If write down full configuration, write down neighbor router, port.
    if ( full )
    {
        // Neighbor router id.
        writeItem( root, NET_ARGU_ROUTER_PORT_NEIGHBOR_ID, m_neighbor_router )

        // Neighbor router port.
        writeItem( root, NET_ARGU_ROUTER_PORT_NEIGHBOR_PORT, m_neighbor_port )
    }

    // Size of input buffer.
    writeItem( root, NET_ARGU_ROUTER_PORT_INPUT_BUFFER, m_input_buffer )

    // Size of output buffer.
    writeItem( root, NET_ARGU_ROUTER_PORT_OUTPUT_BUFFER, m_output_buffer )

    // Network interface.
    writeItem( root, NET_ARGU_ROUTER_PORT_NETWORK_INTERFACE, m_network_interface )
}

bool EsyNetworkCfgPort::operator ==( const EsyNetworkCfgPort & t ) const
{
    if ( m_input_vc_num == t.inputVcNumber() && m_output_vc_num == t.outputVcNumber() &&
         m_port_dir == t.portDirection() && m_neighbor_router == t.neighborRouter() &&
         m_neighbor_port == t.neighborPort() && m_input_buffer == t.inputBuffer() &&
         m_output_buffer == t.outputBuffer() && m_network_interface == t.networkInterface() )
    {
        return true;
    }
    return false;
}

EsyNetworkCfgRouter::EsyNetworkCfgRouter()
    : m_id( DEFAULT_ROUTER_ID )
    , m_pos( DEFAULT_POSITION, DEFAULT_POSITION )
    , m_port( DEFAULT_PHY_NUM, EsyNetworkCfgPort() )
    , m_pipe_cycle( DEFAULT_PIPE_CYCLE )
{}

EsyNetworkCfgRouter::EsyNetworkCfgRouter( const EsyNetworkCfgRouter & t )
    : m_id( t.m_id )
    , m_pos( t.m_pos )
    , m_port( t.m_port )
    , m_pipe_cycle( t.m_pipe_cycle )
{}

std::string EsyNetworkCfgRouter::portName( long i ) const
{
    std::string t_str;
    if ( m_port[ i ].networkInterface() )
    {
        t_str = std::string( "L." ) + (std::string)EsyConvert( i );
    }
    else
    {
        t_str = m_port[ i ].portDirectionStrShort() + "." + (std::string)EsyConvert( i );
    }
    return t_str;
}

void EsyNetworkCfgRouter::readXml( TiXmlElement * root )
{
    // Loop all item behind root.
    for ( TiXmlNode * p_child = root->FirstChild(); p_child != NULL; p_child = p_child->NextSibling() )
    {
        TiXmlElement * t_element = p_child->ToElement();

        // Router id element.
        readItemLong( t_element, NET_ARGU_ROUTER_ID, m_id )

        // Position in GUI.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_POS ) )
        {
            for ( TiXmlNode * p_pos_child = t_element->FirstChild(); p_pos_child != NULL; p_pos_child = p_pos_child->NextSibling() )
            {
                // Position in one axis in GUI.
                TiXmlElement * t_pos_element = p_pos_child->ToElement();
                readItemLong( t_pos_element, NET_ARGU_ROUTER_POS_X, m_pos.first )
                readItemLong( t_pos_element, NET_ARGU_ROUTER_POS_Y, m_pos.second )
            }
        }

        // Ports.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER_PORT ) )
        {
            // Size of port.
            m_port.resize( EsyConvert( t_element->Attribute( NET_ARGU_SIZE ) ) );

            for ( TiXmlNode * p_port_child = t_element->FirstChild(); p_port_child != NULL; p_port_child = p_port_child->NextSibling() )
            {
                // Port configuration.
                TiXmlElement * t_port_element = p_port_child->ToElement();
                m_port[ EsyConvert( t_port_element->Attribute( NET_ARGU_INDEX ) ) ].readXml( t_port_element );
            }
        }

        // Pipeline cycle.
        readItemLong( t_element, NET_ARGU_ROUTER_PIPE, m_pipe_cycle )
    }
}

void EsyNetworkCfgRouter::writeXml( TiXmlElement * root )
{
    // Router id.
    writeItem( root, NET_ARGU_ROUTER_ID, m_id )

    // Position in GUI.
    TiXmlElement * t_pos_item = new TiXmlElement( NET_ARGU_ROUTER_POS );
    // Position in x-axis in GUI.
    TiXmlElement * t_pos_x_item = new TiXmlElement( NET_ARGU_ROUTER_POS_X );
    t_pos_x_item->LinkEndChild( new TiXmlText( EsyConvert( m_pos.first ).c_str() ) );
    t_pos_item->LinkEndChild( t_pos_x_item );
    // Position in y-axis in GUI.
    TiXmlElement * t_pos_y_item = new TiXmlElement( NET_ARGU_ROUTER_POS_Y );
    t_pos_y_item->LinkEndChild( new TiXmlText( EsyConvert( m_pos.second ).c_str() ) );
    t_pos_item->LinkEndChild( t_pos_y_item );
    root->LinkEndChild( t_pos_item );

    // Ports
    TiXmlElement * t_port_list_item = new TiXmlElement( NET_ARGU_ROUTER_PORT );
    t_port_list_item->SetAttribute( NET_ARGU_SIZE, (int)m_port .size() );
    for ( std::size_t i = 0; i < m_port .size(); i ++ )
    {
        // Port configuration.
        TiXmlElement * t_port_item = new TiXmlElement( NET_ARGU_DATA );
        t_port_item->SetAttribute( NET_ARGU_INDEX, (int)i );
        m_port[ i ].writeXml( true, t_port_item );
        t_port_list_item->LinkEndChild( t_port_item );
    }
    root->LinkEndChild( t_port_list_item );

    // Pipeline cycle
    writeItem( root, NET_ARGU_ROUTER_PIPE, m_pipe_cycle )
}

long int EsyNetworkCfgRouter::maxVcNum() const
{
    long vcnum = 1;
    for ( long i = 0; i < portNum(); i ++ )
    {
        if ( m_port[ i ].inputVcNumber() > vcnum )
        {
            vcnum = m_port[ i ].inputVcNumber();
        }
        if ( m_port[ i ].outputVcNumber() > vcnum )
        {
            vcnum = m_port[ i ].outputVcNumber();
        }
    }
    return vcnum;
}

long int EsyNetworkCfgRouter::maxInputBuffer() const
{
    long inputbuffer = 1;
    for ( long i = 0; i < portNum(); i ++ )
    {
        if ( m_port[ i ].inputBuffer() > inputbuffer )
        {
            inputbuffer = m_port[ i ].inputBuffer();
        }
    }
    return inputbuffer;
}

long int EsyNetworkCfgRouter::maxOutputBuffer() const
{
    long outputbuffer = 1;
    for ( long i = 0; i < portNum(); i ++ )
    {
        if ( m_port[ i ].outputBuffer() > outputbuffer )
        {
            outputbuffer = m_port[ i ].outputBuffer();
        }
    }
    return outputbuffer;
}

long int EsyNetworkCfgRouter::totalInputVc() const
{
    long inputvc = 0;
    for ( long i = 0; i < portNum(); i ++ )
    {
        inputvc += m_port[ i ].inputVcNumber();
    }
    return inputvc;
}

long int EsyNetworkCfgRouter::totalOutputVc() const
{
    long outputvc = 0;
    for ( long i = 0; i < portNum(); i ++ )
    {
        outputvc += m_port[ i ].outputVcNumber();
    }
    return outputvc;
}

bool EsyNetworkCfgRouter::operator ==( const EsyNetworkCfgRouter & t ) const
{
    if ( m_id == t.routerId() && m_port == t.port() && m_pipe_cycle == t.pipeCycle() )
    {
        return true;
    }
    return false;
}

EsyNetworkCfg::EsyNetworkCfg()
    : m_topology( DEFAULT_TOPOLOGY )
    , m_size( DEFAULT_NETWORK_DIM, DEFAULT_NETWORK_SIZE )
    , m_router_count( routerCount() )
    , m_data_width( DEFAULT_NETWORK_DATA_WIDTH )
    , m_template_router()
    , m_router( DEFAULT_ROUTER_COUNT, EsyNetworkCfgRouter() )
{}

EsyNetworkCfg::EsyNetworkCfg( const EsyNetworkCfg & t )
    : m_topology( t.m_topology )
    , m_size( t.m_size )
    , m_router_count( t.m_router_count )
    , m_data_width( t.m_data_width )
    , m_template_router( t.m_template_router )
    , m_router( t.m_router )
{}

EsyNetworkCfg::EsyNetworkCfg( NoCTopology topology, const std::vector< long >& size, long data_width
    , double pipe_cycle, long phy_port
    , long input_vc_num, long output_vc_num, long input_buffer, long output_buffer
)
    : m_topology( topology )
    , m_size( size )
    , m_router_count( setRouterCount() )
    , m_data_width( data_width )
    , m_template_router()
    , m_router()
{
    // Set topology and valid size.
    setTopology( topology );
    bool t_2d = ( topology == NT_MESH_2D ) || ( topology == NT_TORUS_2D )
        || ( topology == NT_MESH_DIA && dim() == 2 ) || ( topology == NT_TORUS_DIA && dim() == 2 );
    bool t_3d = ( topology == NT_MESH_DIA && dim() == 3 ) || ( topology == NT_TORUS_DIA && dim() == 3 );

    // Configure tempalte router.
    m_template_router.setPipeCycle( pipe_cycle );
    m_template_router.resizePort( phy_port );
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
            port.setPortAxisDir( EsyNetworkCfgPort::AXDIR_DEC );
            port.setPortDirection( EsyNetworkCfgPort::ROUTER_PORT_SOUTHWEST );
            port.setNetworkInterface( true );
            continue;
        }

        // Calculate the axis for the port. 0 is for NI. 1-2 is for axis dim-1-0, 3-4 is for axis dim-1-1, and so on.
        long t_dim = dim();
        long t_axis = t_dim - 1 - ( ( t_port - 1 ) / 2 ) % t_dim;
        port.setPortAxis( t_axis );
        // If port is even, increase direction; otherwise, decrease direction.
        port.setPortAxisDir( ( t_port % 2 == 1 ) ? EsyNetworkCfgPort::AXDIR_DEC : EsyNetworkCfgPort::AXDIR_INC );
        // Set direction on GUI
        if ( t_2d )
        {
            if ( port.portAxis() == 0 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_NORTH : EsyNetworkCfgPort::ROUTER_PORT_SOUTH );
            }
            else
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_WEST : EsyNetworkCfgPort::ROUTER_PORT_EAST );
            }
        }
        // 3D topology, North, south, northwest, southeast, west, east;
        else if ( t_3d )
        {
            if ( port.portAxis() == 0 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_NORTH : EsyNetworkCfgPort::ROUTER_PORT_SOUTH );
            }
            else if ( port.portAxis() == 1 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_NORTHWEST : EsyNetworkCfgPort::ROUTER_PORT_SOUTHEAST );
            }
            else if ( port.portAxis() == 2 )
            {
                port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC ) ?
                    EsyNetworkCfgPort::ROUTER_PORT_WEST : EsyNetworkCfgPort::ROUTER_PORT_EAST );
            }
        }
        // otherwise topology, North, south
        else
        {
            port.setPortDirection( ( port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC ) ?
                EsyNetworkCfgPort::ROUTER_PORT_NORTH : EsyNetworkCfgPort::ROUTER_PORT_SOUTH );
        }

        // set Size of input and output buffer
        // Set port 0 as NI port.
        port.setNetworkInterface( false );
    }

    // Generate configuration for all router.
    updateRouter();
}

void EsyNetworkCfg::readXml( TiXmlElement * root )
{
    // Loop all item behind root.
    for ( TiXmlNode * p_child = root->FirstChild(); p_child != NULL; p_child = p_child->NextSibling() )
    {
        TiXmlElement * t_element = p_child->ToElement();

        // Topology element
        readItemEnum( t_element, NET_ARGU_TOPOLOGY, m_topology, NoCTopology )

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
        m_router_count = setRouterCount();

        // Data width element.
        readItemLong( t_element, NET_ARGU_DATA_WIDTH, m_data_width )

        // Template router element.
        if ( t_element->Value() == std::string( NET_ARGU_TEMPLATE_ROUTER ) )
        {
            m_template_router.readXml( t_element );
        }

        // Special router elements.
        if ( t_element->Value() == std::string( NET_ARGU_ROUTER ) )
        {
            updateRouter();

            // Loop for each special router.
            for ( TiXmlNode * p_router_child = t_element->FirstChild(); p_router_child != NULL; p_router_child = p_router_child->NextSibling() )
            {
                TiXmlElement * t_router_element = p_router_child->ToElement();
                EsyNetworkCfgRouter router_cfg;
                router_cfg.readXml( t_router_element );

                m_router[ router_cfg.routerId() ] = router_cfg;
            }
        }
    }
}

void EsyNetworkCfg::writeXml( TiXmlElement * root )
{
    // Topology element.
    writeItem( root, NET_ARGU_TOPOLOGY, m_topology )

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

    // Data width element.
    writeItem( root, NET_ARGU_DATA_WIDTH, m_data_width )

    // Template router.
    TiXmlElement * t_template_router_item = new TiXmlElement( NET_ARGU_TEMPLATE_ROUTER );
    m_template_router.writeXml( t_template_router_item );
    root->LinkEndChild( t_template_router_item );

    // Special routers.
    TiXmlElement * t_router_list_item = new TiXmlElement( NET_ARGU_ROUTER );
    t_router_list_item->SetAttribute( NET_ARGU_SIZE, (int)m_router .size() );
    for ( std::size_t i = 0; i < m_router.size(); i ++ )
    {
        // If the configuration of specified router is different from template router, record it.
        if ( !( m_router[ i ] == defaultRouterCfg( (long)i ) ) )
        {
            TiXmlElement * t_router_item = new TiXmlElement( NET_ARGU_DATA );
            t_router_item->SetAttribute( NET_ARGU_INDEX, (int)i );
            m_router[ i ] .writeXml( t_router_item );
            t_router_list_item->LinkEndChild( t_router_item );
        }
    }
    root->LinkEndChild( t_router_list_item );
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
        resizeDim( AX_NUM );
        if ( size( AX_Y ) == 0 )
        {
            setSize( AX_Y, size( AX_X ) );
        }
    }
    // If the topology is multiple diamension mesh/torus, resize diamension to 2D if there is only one diamension before.
    else if ( m_topology == NT_MESH_DIA || m_topology == NT_TORUS_DIA )
    {
        if ( dim() < AX_NUM )
        {
            resizeDim( AX_NUM );
            if ( size( AX_Y ) == 0 )
            {
                setSize( AX_Y, size( AX_X ) );
            }
        }
    }
    // If the topology is ring, set diamension to 1 and set size to the number of routers.
    else if ( m_topology == NT_RING )
    {
        setSize( AX_X, routerCount() );
        resizeDim( 1 );
    }
    // If the topology is switch, set diamension to 1 and set size to 1.
    else
    {
        resizeDim( 1 );
        setSize( AX_X, 1 );
    }
}

int EsyNetworkCfg::setRouterCount() const
{
    int num = 1;
    for ( std::size_t i = 0; i < m_size.size(); i ++ )
    {
        num = num * m_size[ i ];
    }
    return num;
}

int EsyNetworkCfg::setNiCount() const
{
    int num = 0;
    for ( std::size_t i = 0; i < m_router.size(); i ++ )
    {
        for ( std::size_t j = 0; j < m_router[ i ].portNum(); j ++ )
        {
            if ( m_router[ i ].port( j ).networkInterface() )
            {
                num ++;
            }
        }
    }
    return num;
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

EsyNetworkCfgRouter EsyNetworkCfg::defaultRouterCfg( long id )
{
    // Copy router from template router.
    EsyNetworkCfgRouter t_router_cfg;
    t_router_cfg.setRouterId( id );
    t_router_cfg.setPipeCycle( 1.0 );
    t_router_cfg.setPos( 0.0, 0.0 );
    t_router_cfg.setPort( m_template_router.port() );

    // Ger coordinary of the router.
    std::vector< long > t_ax = seq2Coord( t_router_cfg.routerId() );

    // Disable port for mesn topology
    if ( m_topology == NT_MESH_2D || m_topology == NT_MESH_DIA )
    {
        for ( long t_port_index = 0; t_port_index < t_router_cfg.portNum(); t_port_index ++ )
        {
            EsyNetworkCfgPort& t_port = t_router_cfg.port( t_port_index );
            // Except network interface port
            if ( t_port.networkInterface() )
            {
                continue;
            }

            // The decrease direction port at the first router on the axis or the increase direction port at the last router on the axis,
            // reset the vc number to 0 to close the port.
            long t_port_axis = t_port.portAxis();
            if ( ( t_ax[ t_port_axis ] == 0 && t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC )
              || ( t_ax[ t_port_axis ]  == m_size[ t_port_axis ] - 1 && t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_INC )  )
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
            t_port.setNeighborRouter( t_router_cfg.routerId() );
            t_port.setNeighborPort( t_port_index );
        }
        // Decrease port, connect to the previous router on the axis, port + 1.
        else if ( t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_DEC )
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
        else if ( t_port.portAxisDir() == EsyNetworkCfgPort::AXDIR_INC )
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

    return t_router_cfg;
}

void EsyNetworkCfg::updateRouter()
{
    // Clear router.
    m_router.clear();
    // Regenerate router.
    for ( int i = 0; i < routerCount(); i ++ )
    {
        m_router.push_back( defaultRouterCfg( i ) );
    }
    // ni 
    setNiCount();
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

long int EsyNetworkCfg::maxVcNum() const
{
    long vcnum = 1;
    for ( long i = 0; i < routerCount(); i ++ )
    {
        if ( m_router[ i ].maxVcNum() > vcnum )
        {
            vcnum = m_router[ i ].maxVcNum();
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

#undef readItem
#undef writeItem
