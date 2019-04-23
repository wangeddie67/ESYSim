#ifndef ROUTER_CFG_H
#define ROUTER_CFG_H

#include "../define/qt_include.h"

enum NoCTopologyEnum{
    NOC_TOPOLOGY_ROUTER,
    NOC_TOPOLOGY_2DMESH,
    NOC_TOPOLOGY_2DTORUS,
    NOC_TOPOLOGY_IRREGULAR
};
#define NOC_TOPOLOGY_ROUTER_STR     QString( "Router" )
#define NOC_TOPOLOGY_2DMESH_STR     QString( "2D-Mesh" )
#define NOC_TOPOLOGY_2DTORUS_STR    QString( "2D-Torus" )
#define NOC_TOPOLOGY_IRREGULAR_STR  QString( "Irregular" )

enum NoCPortDirEnum{
    NOC_PORT_NORTH,
    NOC_PORT_SOUTH,
    NOC_PORT_WEST,
    NOC_PORT_EAST,
    NOC_PORT_NORTHWEST,
    NOC_PORT_NORTHEAST,
    NOC_PORT_SOUTHWEST,
    NOC_PORT_SOUTHEAST,
    NOC_PORT_NUM
};
#define NOC_PORT_NORTH_STR      QString( "North" )
#define NOC_PORT_SOUTH_STR      QString( "South" )
#define NOC_PORT_WEST_STR       QString( "West" )
#define NOC_PORT_EAST_STR       QString( "East" )
#define NOC_PORT_NORTHWEST_STR  QString( "NorthWest" )
#define NOC_PORT_NORTHEAST_STR  QString( "NorthEast" )
#define NOC_PORT_SOUTHWEST_STR  QString( "SouthWest" )
#define NOC_PORT_SOUTHEAST_STR  QString( "SouthEast" )

#define DEFAULT_TOPOLOGY    NOC_TOPOLOGY_2DMESH
#define DEFAULT_TOPOLOGY_STR    NOC_TOPOLOGY_2DMESH_STR
#define DEFAULT_NETWORK_DIM     2
#define DEFAULT_NETWORK_SIZE    8
#define DEFAULT_PHY_NUM 5
#define DEFAULT_VC_NUM  1
#define DEFAULT_PORT_DIRECTION  NOC_PORT_SOUTHWEST
#define DEFAULT_PORT_DIRECTION_STR  NOC_PORT_SOUTHWEST_STR
#define DEFAULT_PORT_FAULT  false
#define DEFAULT_INPUT_BUFFER    12
#define DEFAULT_OUTPUT_BUFFER   12
#define DEFAULT_ROUTER_FAULT false
#define DEFAULT_POSITION    0.0
#define DEFAULT_NEIGHBOR_ID 0
#define DEFAULT_NEIGHBOR_PORT   0
#define DEFAULT_PIPE_CYCLE  1.0
#define DEFAULT_NETWORK_INTERFACE false

class PortCfgStructure
{
public:
    bool m_full;
    int m_vc_num;
    NoCPortDirEnum m_port_dir;
    int m_neighbor_id;
    int m_neighbor_port;
    int m_input_buffer;
    int m_output_buffer;
    bool m_port_fault;
    bool m_network_interface;

public:
    PortCfgStructure() :
        m_full( true ),
        m_vc_num( DEFAULT_VC_NUM ), m_port_dir( DEFAULT_PORT_DIRECTION ),
        m_neighbor_id( -1 ), m_neighbor_port( -1 ),
        m_input_buffer( DEFAULT_INPUT_BUFFER ), m_output_buffer( DEFAULT_OUTPUT_BUFFER ),
        m_port_fault( DEFAULT_PORT_FAULT ), m_network_interface( DEFAULT_NETWORK_INTERFACE )
    {}

    PortCfgStructure( const PortCfgStructure & t ) :
        m_full( t .m_full ),
        m_vc_num( t .m_vc_num ), m_port_dir( t .m_port_dir ),
        m_neighbor_id( t .m_neighbor_id ), m_neighbor_port( t .m_neighbor_port ),
        m_input_buffer( t .m_input_buffer ), m_output_buffer( t .m_output_buffer ),
        m_port_fault( t .m_port_fault ), m_network_interface( t .m_network_interface )
    {}

    void readXml( QDomElement * root );
    void writeXml( bool s, QDomDocument * doc, QDomElement * root );
};

class RouterCfgStructure{
public:
    int m_id;
    QPointF m_pos;
    QVector< PortCfgStructure > m_port;
    bool m_router_fault;
    double m_pipe_cycle;

public:
    RouterCfgStructure() :
        m_id( 0 ), m_pos( 0.0, 0.0 ), m_port( 1, PortCfgStructure() ),
        m_router_fault( false ), m_pipe_cycle( 1.0 )
    {}

    RouterCfgStructure( const RouterCfgStructure & t ) :
        m_id( t .m_id ), m_pos( t .m_pos ), m_port( t .m_port ),
        m_router_fault( t .m_router_fault ), m_pipe_cycle( t .m_pipe_cycle )
    {}

    void readXml( QDomElement * root );
    void writeXml( QDomDocument * doc, QDomElement * root );
};

class NetworkCfgStructure{
public:
    NoCTopologyEnum m_topology;
    QVector< int > m_size;
    QVector< PortCfgStructure > m_port;

    QVector< RouterCfgStructure > m_router;

public:
    NetworkCfgStructure() :
        m_topology( NOC_TOPOLOGY_ROUTER ), m_size( 0.0, 0.0 ), m_port( 1, PortCfgStructure() ),
        m_router()
    {}

    NetworkCfgStructure( const NetworkCfgStructure & t ) :
        m_topology( t .m_topology ), m_size( t .m_size ), m_port( t .m_port ),
        m_router( t .m_router )
    {}

    void readXml( QDomElement * root );
    void writeXml( QDomDocument * doc, QDomElement * root );

    ErrorHandler readXml( QString fname );
    ErrorHandler writeXml( QString fname );

    int routerCount();

    int coord2Seq( std::vector< char > coord );
    std::vector< char > seq2Coord( int seq );
    int coordDistance( char p1, char p2 );
};


#define NET_ARGU_TOPOLOGY   QString( "Topology" )
#define NET_ARGU_NETWORK_SIZE   QString( "Network Size" )
#define NET_ARGU_X_AXIS QString( "X Axis" )
#define NET_ARGU_Y_AXIS QString( "Y Axis" )
#define NET_ARGU_PORT_NUMBER    QString( "Port Number" )
#define NET_ARGU_PORT_CONFIG_SET    QString( "Port Config Set" )
#define NET_ARGU_PORT_VC_NUMBER QString( "VC Number" )
#define NET_ARGU_PORT_DIRECTION QString( "Port Direction" )
#define NET_ARGU_PORT_FAULT QString( "Port Fault" )
#define NET_ARGU_PORT_NEIGHBOR_ID   QString( "Neighbor ID" )
#define NET_ARGU_PORT_NEIGHBOR_PORT QString( "Neighbor Port" )
#define NET_ARGU_PORT_INPUT_BUFFER  QString( "Input Buffer Size" )
#define NET_ARGU_PORT_OUTPUT_BUFFER QString( "Output Buffer Size")
#define NET_ARGU_ROUTER_FAULT   QString( "Router Fault" )
#define NET_ARGU_POSITION   QString( "Position" )
#define NET_ARGU_PIPELINE   QString( "Pipeline Cycle")
#define NET_ARGU_INPUT_NEIGHBOR_SET   QString( "Input Port Neighbor" )
#define NET_ARGU_OUTPUT_NEIGHBOR_SET   QString( "Output Port Neighbor" )
#define NET_ARGU_NEITHBOR_ID    QString( "Neighbor Id" )
#define NET_ARGU_NEITHBOR_PORT  QString( "Neighbor Port" )
#define NET_ARGU_PORT_NETWORK_INTERFACE QString( "Network Interface" )

#define NET_ARGU_XML_FILE_HEAD  QString( "Network_Configuration_File" )
#define NET_ARGU_XML    QString( "networkcfg" )
#define NET_ARGU_XML_TOPOLOGY   QString( "topology" )
#define NET_ARGU_XML_NETWORK_SIZE   QString( "size" )
#define NET_ARGU_XML_ROUTER_CFG     QString( "router_cfg" )
#define NET_ARGU_XML_ROUTER_ID      QString( "id" )
#define NET_ARGU_XML_ROUTER_POS     QString( "position" )
#define NET_ARGU_XML_ROUTER_POS_X   QString( "x" )
#define NET_ARGU_XML_ROUTER_POS_Y   QString( "y" )
#define NET_ARGU_XML_ROUTER_FALSE   QString( "router_false" )
#define NET_ARGU_XML_ROUTER_PIPE    QString( "pipe_cycle" )
#define NET_ARGU_XML_PORT_NUMBER    QString( "port_num" )
#define NET_ARGU_XML_PORT_CFG   QString( "port_cfg" )
#define NET_ARGU_XML_PORT_VC_NUMBER QString( "vc_num" )
#define NET_ARGU_XML_PORT_DIRECTION QString( "port_dir" )
#define NET_ARGU_XML_PORT_NEIGHBOR_ID   QString( "neighbor_id" )
#define NET_ARGU_XML_PORT_NEIGHBOR_PORT QString( "neighbor_port" )
#define NET_ARGU_XML_PORT_INPUT_BUFFER  QString( "input_buffer" )
#define NET_ARGU_XML_PORT_OUTPUT_BUFFER QString( "output_buffer" )
#define NET_ARGU_XML_PORT_FALSE QString( "port_false" )
#define NET_ARGU_XML_PORT_NETWORK_INTERFACE QString( "ni" )

#define NET_ARGU_XML_DATA   QString( "data" )
#define NET_ARGU_XML_SIZE   QString( "size" )
#define NET_ARGU_XML_INDEX  QString( "index" )

#endif // ROUTER_CFG_H
