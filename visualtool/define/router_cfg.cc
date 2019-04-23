#include "router_cfg.h"

void PortCfgStructure::readXml( QDomElement * root )
{
    QDomNodeList t_node_list = root ->childNodes();
    for ( int i = 0; i < t_node_list .size(); i ++ )
    {
        QDomElement t_element = t_node_list .at( i ) .toElement();
        if ( t_element .tagName() == NET_ARGU_XML_PORT_VC_NUMBER )
        {
            m_vc_num = t_element .text() .toInt();
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_DIRECTION )
        {
            QString t_str = t_element .text();
            if      ( t_str == NOC_PORT_NORTH_STR )     m_port_dir = NOC_PORT_NORTH;
            else if ( t_str == NOC_PORT_SOUTH_STR )     m_port_dir = NOC_PORT_SOUTH;
            else if ( t_str == NOC_PORT_WEST_STR )      m_port_dir = NOC_PORT_WEST;
            else if ( t_str == NOC_PORT_EAST_STR )      m_port_dir = NOC_PORT_EAST;
            else if ( t_str == NOC_PORT_NORTHWEST_STR ) m_port_dir = NOC_PORT_NORTHWEST;
            else if ( t_str == NOC_PORT_NORTHEAST_STR ) m_port_dir = NOC_PORT_NORTHEAST;
            else if ( t_str == NOC_PORT_SOUTHWEST_STR ) m_port_dir = NOC_PORT_SOUTHWEST;
            else if ( t_str == NOC_PORT_SOUTHEAST_STR ) m_port_dir = NOC_PORT_SOUTHEAST;
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_NEIGHBOR_ID )
        {
            m_neighbor_id = t_element .text() .toInt();
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_NEIGHBOR_PORT )
        {
            m_neighbor_port = t_element .text() .toInt();
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_INPUT_BUFFER )
        {
            m_input_buffer = t_element .text() .toInt();
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_OUTPUT_BUFFER )
        {
            m_output_buffer = t_element .text() .toInt();
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_FALSE )
        {
            QString t_str = t_element .text();
            if      ( t_str == "true" ) m_port_fault = true;
            else if ( t_str == "TRUE" ) m_port_fault = true;
            else if ( t_str == "True" ) m_port_fault = true;
            else                        m_port_fault = false;
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_NETWORK_INTERFACE )
        {
            QString t_str = t_element .text();
            if      ( t_str == "true" ) m_network_interface = true;
            else if ( t_str == "TRUE" ) m_network_interface = true;
            else if ( t_str == "True" ) m_network_interface = true;
            else                        m_network_interface = false;
        }
    }
}

void PortCfgStructure::writeXml( bool s, QDomDocument * doc, QDomElement * root )
{
    m_full = s;
    // m_vc_num
    QDomElement t_vc_num_item = doc ->createElement( NET_ARGU_XML_PORT_VC_NUMBER );
    t_vc_num_item .appendChild( doc ->createTextNode( QString::number( m_vc_num ) ) );
    root ->appendChild( t_vc_num_item );
    // port direction
    QDomElement t_dir_item = doc ->createElement( NET_ARGU_XML_PORT_DIRECTION );
    QString t_dir_str;
    switch ( m_port_dir )
    {
    case NOC_PORT_NORTH:     t_dir_str = NOC_PORT_NORTH_STR;     break;
    case NOC_PORT_SOUTH:     t_dir_str = NOC_PORT_SOUTH_STR;     break;
    case NOC_PORT_WEST:      t_dir_str = NOC_PORT_WEST_STR;      break;
    case NOC_PORT_EAST:      t_dir_str = NOC_PORT_EAST_STR;      break;
    case NOC_PORT_NORTHWEST: t_dir_str = NOC_PORT_NORTHWEST_STR; break;
    case NOC_PORT_NORTHEAST: t_dir_str = NOC_PORT_NORTHEAST_STR; break;
    case NOC_PORT_SOUTHWEST: t_dir_str = NOC_PORT_SOUTHWEST_STR; break;
    case NOC_PORT_SOUTHEAST: t_dir_str = NOC_PORT_SOUTHEAST_STR; break;
    default:                 t_dir_str = NOC_PORT_NORTH_STR;     break;
    }
    t_dir_item .appendChild( doc ->createTextNode( t_dir_str ) );
    root ->appendChild( t_dir_item );
    if ( m_full )
    {
        // neighbor id
        QDomElement t_neighbor_id_item = doc ->createElement( NET_ARGU_XML_PORT_NEIGHBOR_ID );
        t_neighbor_id_item .appendChild( doc ->createTextNode( QString::number( m_neighbor_id ) ) );
        root ->appendChild( t_neighbor_id_item );
        // neighbor port
        QDomElement t_neighbor_port_item = doc ->createElement( NET_ARGU_XML_PORT_NEIGHBOR_PORT );
        t_neighbor_port_item .appendChild( doc ->createTextNode( QString::number( m_neighbor_port ) ) );
        root ->appendChild( t_neighbor_port_item );
    }
    // input buffer
    QDomElement t_input_buffer_item = doc ->createElement( NET_ARGU_XML_PORT_INPUT_BUFFER );
    t_input_buffer_item .appendChild( doc ->createTextNode( QString::number( m_input_buffer ) ) );
    root ->appendChild( t_input_buffer_item );
    // output buffer
    QDomElement t_output_buffer_item = doc ->createElement( NET_ARGU_XML_PORT_OUTPUT_BUFFER );
    t_output_buffer_item .appendChild( doc ->createTextNode( QString::number( m_output_buffer ) ) );
    root ->appendChild( t_output_buffer_item );
    QDomElement t_network_interface_item = doc ->createElement( NET_ARGU_XML_PORT_NETWORK_INTERFACE );
    QString t_ni_str( "false" );
    if ( m_network_interface == true )
        t_ni_str = "true";
    t_network_interface_item .appendChild( doc ->createTextNode( t_ni_str ) );
    root ->appendChild( t_network_interface_item );
    // port false
    if ( m_full )
    {
        QDomElement t_port_false_item = doc ->createElement( NET_ARGU_XML_PORT_FALSE );
        QString t_false_str( "false" );
        if ( m_port_fault == true )
            t_false_str = "true";
        t_port_false_item .appendChild( doc ->createTextNode( t_false_str ) );
        root ->appendChild( t_port_false_item );
    }
}

void RouterCfgStructure::readXml( QDomElement * root )
{
    QDomNodeList t_node_list = root ->childNodes();
    for ( int i = 0; i < t_node_list .size(); i ++ )
    {
        QDomElement t_element = t_node_list .at( i ) .toElement();
        if ( t_element .tagName() == NET_ARGU_XML_ROUTER_ID )
        {
            m_id = t_element .text() .toInt();
        }
        else if ( t_element .tagName() == NET_ARGU_XML_ROUTER_POS )
        {
            QDomNodeList t_axis_node = t_element .childNodes();
            for ( int j = 0; j < t_axis_node .size(); j ++ )
            {
                QDomElement t_axis_element = t_axis_node .at( j ) .toElement();
                if ( t_axis_element .tagName() == NET_ARGU_XML_ROUTER_POS_X )
                {
                    m_pos .setX( t_axis_element .text() .toDouble() );
                }
                else if ( t_axis_element .tagName() == NET_ARGU_XML_ROUTER_POS_Y )
                {
                    m_pos .setY( t_axis_element .text() .toDouble() );
                }
            }
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_CFG )
        {
            int t_size = t_element .attribute( NET_ARGU_XML_SIZE ) .toInt();
            m_port .resize( t_size );
            QDomNodeList t_port_list = t_element .childNodes();

            for ( int j = 0; j < t_port_list .size(); j ++ )
            {
                QDomElement t_port_item = t_port_list .at( j ) .toElement();
                m_port[ t_port_item .attribute( NET_ARGU_XML_INDEX ) .toInt() ] .readXml( &t_port_item );
            }
        }
        else if ( t_element .tagName() == NET_ARGU_XML_ROUTER_FALSE )
        {
            QString t_str = t_element .text();
            if      ( t_str == "true" ) m_router_fault = true;
            else if ( t_str == "TRUE" ) m_router_fault = true;
            else if ( t_str == "True" ) m_router_fault = true;
            else                        m_router_fault = false;
        }
        else if ( t_element .tagName() == NET_ARGU_XML_ROUTER_PIPE )
        {
            m_pipe_cycle = t_element .text() .toDouble();
        }
    }
}

void RouterCfgStructure::writeXml( QDomDocument * doc, QDomElement * root )
{
    // m_vc_num
    QDomElement t_id_item = doc ->createElement( NET_ARGU_XML_ROUTER_ID );
    t_id_item .appendChild( doc ->createTextNode( QString::number( m_id ) ) );
    root ->appendChild( t_id_item );
    // position
    QDomElement t_pos_item = doc ->createElement( NET_ARGU_XML_ROUTER_POS );
    QDomElement t_pos_x_item = doc ->createElement( NET_ARGU_XML_ROUTER_POS_X );
    t_pos_x_item .appendChild( doc ->createTextNode( QString::number( m_pos .rx() ) ) );
    t_pos_item .appendChild( t_pos_x_item );
    QDomElement t_pos_y_item = doc ->createElement( NET_ARGU_XML_ROUTER_POS_Y );
    t_pos_y_item .appendChild( doc ->createTextNode( QString::number( m_pos .ry() ) ) );
    t_pos_item .appendChild( t_pos_y_item );
    root ->appendChild( t_pos_item );
    // router
    QDomElement t_port_list_item = doc ->createElement( NET_ARGU_XML_PORT_CFG );
    t_port_list_item .setAttribute( NET_ARGU_XML_SIZE, m_port .size() );
    for ( int i = 0; i < m_port .size(); i ++ )
    {
        QDomElement t_port_item = doc ->createElement( NET_ARGU_XML_DATA );
        t_port_item .setAttribute( NET_ARGU_XML_INDEX, i );
        m_port[ i ] .writeXml( true, doc, &t_port_item );
        t_port_list_item .appendChild( t_port_item );
    }
    root ->appendChild( t_port_list_item );
    // router false
    QDomElement t_router_false_item = doc ->createElement( NET_ARGU_XML_ROUTER_FALSE );
    QString t_false_str( "false" );
    if ( m_router_fault == true )
        t_false_str = "true";
    t_router_false_item .appendChild( doc ->createTextNode( t_false_str ) );
    root ->appendChild( t_router_false_item );
    // pipe cycle
    QDomElement t_pipe_item = doc ->createElement( NET_ARGU_XML_ROUTER_PIPE );
    t_pipe_item .appendChild( doc ->createTextNode( QString::number( m_pipe_cycle ) ) );
    root ->appendChild( t_pipe_item );
}

void NetworkCfgStructure::readXml( QDomElement * root )
{
    QDomNodeList t_node_list = root ->childNodes();
    for ( int i = 0; i < t_node_list .size(); i ++ )
    {
        QDomElement t_element = t_node_list .at( i ) .toElement();
        if ( t_element .tagName() == NET_ARGU_XML_TOPOLOGY )
        {
            QString t_text = t_element .text();
            if      ( t_text == NOC_TOPOLOGY_ROUTER_STR )  m_topology = NOC_TOPOLOGY_ROUTER;
            else if ( t_text == NOC_TOPOLOGY_2DMESH_STR )  m_topology = NOC_TOPOLOGY_2DMESH;
            else if ( t_text == NOC_TOPOLOGY_2DTORUS_STR ) m_topology = NOC_TOPOLOGY_2DTORUS;
            else                                           m_topology = NOC_TOPOLOGY_IRREGULAR;
        }
        else if ( t_element .tagName() == NET_ARGU_XML_NETWORK_SIZE )
        {
            int t_size = t_element .attribute( NET_ARGU_XML_SIZE ) .toInt();
            m_size .resize( t_size );
            QDomNodeList t_port_list = t_element .childNodes();

            for ( int j = 0; j < t_port_list .size(); j ++ )
            {
                QDomElement t_port_item = t_port_list .at( j ) .toElement();
                m_size[ t_port_item .attribute( NET_ARGU_XML_INDEX ) .toInt() ] =
                    t_port_item .text() .toInt();
            }
        }
        else if ( t_element .tagName() == NET_ARGU_XML_PORT_CFG )
        {
            int t_size = t_element .attribute( NET_ARGU_XML_SIZE ) .toInt();
            m_port .resize( t_size );
            QDomNodeList t_port_list = t_element .childNodes();

            for ( int j = 0; j < t_port_list .size(); j ++ )
            {
                QDomElement t_port_item = t_port_list .at( j ) .toElement();
                m_port[ t_port_item .attribute( NET_ARGU_XML_INDEX ) .toInt() ] .readXml( &t_port_item );
            }
        }
        else if ( t_element .tagName() == NET_ARGU_XML_ROUTER_CFG )
        {
            int t_size = t_element .attribute( NET_ARGU_XML_SIZE ) .toInt();
            m_router .resize( t_size );
            QDomNodeList t_router_list = t_element .childNodes();

            for ( int j = 0; j < t_router_list .size(); j ++ )
            {
                QDomElement t_router_item = t_router_list .at( j ) .toElement();
                m_router[ t_router_item .attribute( NET_ARGU_XML_INDEX ) .toInt() ] .readXml( &t_router_item );
            }
        }
    }
}

void NetworkCfgStructure::writeXml( QDomDocument * doc, QDomElement * root )
{
    // topology
    QString t_topology_str;
    switch ( m_topology )
    {
    case NOC_TOPOLOGY_ROUTER:    t_topology_str = NOC_TOPOLOGY_ROUTER_STR;    break;
    case NOC_TOPOLOGY_2DMESH:    t_topology_str = NOC_TOPOLOGY_2DMESH_STR;    break;
    case NOC_TOPOLOGY_2DTORUS:   t_topology_str = NOC_TOPOLOGY_2DTORUS_STR;   break;
    case NOC_TOPOLOGY_IRREGULAR: t_topology_str = NOC_TOPOLOGY_IRREGULAR_STR; break;
    }
    QDomElement t_topology_item = doc ->createElement( NET_ARGU_XML_TOPOLOGY );
    t_topology_item .appendChild( doc ->createTextNode( t_topology_str ) );
    root ->appendChild( t_topology_item );
    // network size
    QDomElement t_size_item = doc ->createElement( NET_ARGU_XML_NETWORK_SIZE );
    t_size_item .setAttribute( NET_ARGU_XML_SIZE, m_size .size() );
    for ( int i = 0; i < m_size .size(); i ++ )
    {
        QDomElement t_data_item = doc ->createElement( NET_ARGU_XML_DATA );
        t_data_item .setAttribute( NET_ARGU_XML_INDEX, i );
        t_data_item .appendChild( doc ->createTextNode( QString::number( m_size[ i ] ) ) ) ;
        t_size_item .appendChild( t_data_item );
    }
    root ->appendChild( t_size_item );
    // port
    QDomElement t_port_list_item = doc ->createElement( NET_ARGU_XML_PORT_CFG );
    t_port_list_item .setAttribute( NET_ARGU_XML_SIZE, m_port .size() );
    for ( int i = 0; i < m_port .size(); i ++ )
    {
        QDomElement t_port_item = doc ->createElement( NET_ARGU_XML_DATA );
        t_port_item .setAttribute( NET_ARGU_XML_INDEX, i );
        m_port[ i ] .writeXml( false, doc, &t_port_item );
        t_port_list_item .appendChild( t_port_item );
    }
    root ->appendChild( t_port_list_item );
    // router
    QDomElement t_router_list_item = doc ->createElement( NET_ARGU_XML_ROUTER_CFG );
    t_router_list_item .setAttribute( NET_ARGU_XML_SIZE, m_router .size() );
    for ( int i = 0; i < m_router .size(); i ++ )
    {
        QDomElement t_router_item = doc ->createElement( NET_ARGU_XML_DATA );
        t_router_item .setAttribute( NET_ARGU_XML_INDEX, i );
        m_router[ i ] .writeXml( doc, &t_router_item );
        t_router_list_item .appendChild( t_router_item );
    }
    root ->appendChild( t_router_list_item );
}

ErrorHandler NetworkCfgStructure::readXml( QString fname )
{
    // open file and check
    QFile argufile( fname );
    if ( !argufile .open( QFile::ReadOnly | QFile::Text ) )
    {
        return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() << fname,
            QString( "NetworkCfgStructure" ), QString( "readXml" ) );
    }
    // open xml stream
    QString errorMsg;
    int errorLine;
    int errorColumn;
    QDomDocument t_doc( NET_ARGU_XML_FILE_HEAD );
    if ( !t_doc .setContent( &argufile, &errorMsg, &errorLine, &errorColumn ) )
    {
        argufile .close();
        return ErrorHandler( ErrorHandler::DOMERROR,
            QStringList() << fname << errorMsg <<
                QString::number( errorLine ) << QString::number( errorColumn ),
            QString( "NetworkCfgStructure" ), QString( "readXml" ) );
    }
    argufile .close();

    // head
    QDomElement t_head = t_doc .documentElement();
    if ( t_head .tagName() != NET_ARGU_XML )
        return ErrorHandler( ErrorHandler::XMLTAGERROR, QStringList() << fname << t_head .tagName(),
            QString( "NetworkCfgStructure" ), QString( "readXml" ) );

    readXml( &t_head );

    return ErrorHandler();
}

ErrorHandler NetworkCfgStructure::writeXml( QString fname )
{
    // open file and check
    QFile argufile( fname );
    if ( !argufile .open( QFile::WriteOnly | QFile::Text ) )
    {
        return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() << "linktools.xml",
            QString( "NetworkCfgStructure" ), QString( "writeXml" ) );
    }

    QDomDocument t_doc( NET_ARGU_XML_FILE_HEAD );

    QDomElement t_root = t_doc .createElement( NET_ARGU_XML );
    t_doc .appendChild( t_root );

    writeXml( &t_doc, &t_root );

    QTextStream t_text( &argufile );
    t_doc .save( t_text, 4 );

    argufile .close();
    return ErrorHandler();
}

int NetworkCfgStructure::routerCount()
{
    if ( m_topology == NOC_TOPOLOGY_2DMESH || m_topology == NOC_TOPOLOGY_2DTORUS )
        return m_size[ 0 ] * m_size[ 1 ];
    else
        return m_size[ 0 ];
}

int NetworkCfgStructure::coord2Seq( std::vector< char > coord )
{
    if ( m_topology == NOC_TOPOLOGY_2DMESH || m_topology == NOC_TOPOLOGY_2DTORUS )
        return coord[ 1 ] * m_size[ 0 ] + coord[ 0 ];
    else
        return coord[ 0 ];
}

std::vector< char > NetworkCfgStructure::seq2Coord( int seq )
{
	std::vector< char > coord;
    if ( m_topology == NOC_TOPOLOGY_2DMESH || m_topology == NOC_TOPOLOGY_2DTORUS )
	{
        coord .push_back( seq % m_size[ 0 ] );
		coord .push_back( seq / m_size[ 1 ] );
	}
    else
        coord .push_back( seq );
	return coord;
}

int NetworkCfgStructure::coordDistance( char p1, char p2 )
{
    if ( m_topology == NOC_TOPOLOGY_2DMESH || m_topology == NOC_TOPOLOGY_2DTORUS )
	{
		std::vector< char > t_p1 = seq2Coord( p1 );
		std::vector< char > t_p2 = seq2Coord( p2 );
        return qAbs( t_p1[ 0 ] - t_p2[ 0 ] ) + qAbs( t_p1[ 1 ] - t_p2[ 1 ] );
	}
    else
        return qAbs( p1 - p2 );
}
