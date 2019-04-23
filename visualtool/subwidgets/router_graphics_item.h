#ifndef ROUTER_GRAPHICS_ITEM_H
#define ROUTER_GRAPHICS_ITEM_H

#include "../define/qt_include.h"

#include "../modelsview/path_trace_table.h"
#include "../modelsview/graphics_config_tree.h"
#include "../modelsview/application_gantt_chart.h"

#include "port_graphics_item.h"
#include "esy_networkcfg.h"


// draw NoC and show the states of each packet and router
class RouterGraphicsItem : public QGraphicsItem, public ProcessorItem
{
private:
    static qreal portdir_angle[ EsyNetworkCfgPort::ROUTER_PORT_NUM ];

    static QPointF portdir_radio_unit[ EsyNetworkCfgPort::ROUTER_PORT_NUM ];

    static QPointF portdir_circle_unit[ EsyNetworkCfgPort::ROUTER_PORT_NUM ];

    static QPointF router_boundary_point[ EsyNetworkCfgPort::ROUTER_PORT_NUM ];

    // constructor and deconstructor
public:
    RouterGraphicsItem( const EsyNetworkCfgRouter & router_cfg,
                        GraphicsConfigModel * graphics_cfg );

	// noc variable
private:
    EsyNetworkCfgRouter m_router_cfg;
    GraphicsConfigModel * mp_graphics_cfg;

    // string for input channel output port<vc<field>
    QVector< QVector< PortGraphicsItem * > > m_input_port;
    // string for output channel output port<vc<field>
    QVector< QVector< PortGraphicsItem * > > m_output_port;

    // draw parameter
    QRectF router_rect;
    qreal router_width;
    QVector< qreal > dir_width;
    qreal max_dir_width;
    QVector< qreal > port_offset;

    QVector< QVector< QVector< QPair< int, int > > > > m_routing_table;
    QVector< QVector< QPair< int, int > > > m_assign_table;

    QVector< EsyDataItemTraffic > m_path_pair_list;
	long m_path_max_count;

    bool m_faulty_flag;

	// I/O function
public:
    const EsyNetworkCfgRouter & routerCfg() const { return m_router_cfg; }
    // flit
    int inputFlitSize( int port, int vc ) const
        { return m_input_port[ port ][ vc ] ->flitSize(); }
    int outputFlitSize( int port, int vc ) const
        { return m_output_port[ port ][ vc ] ->flitSize(); }
    const ShortFlitStruct & getInputTopFlit( int port, int vc ) const
        { return m_input_port[ port ][ vc ]->getFirstFlit(); }
    const ShortFlitStruct & getOutputTopFlit( int port, int vc ) const
        { return m_output_port[ port ][ vc ]->getFirstFlit(); }
    const ShortFlitStruct & getInputLastFlit( int port, int vc ) const
        { return m_input_port[ port ][ vc ]->getLastFlit(); }
    const ShortFlitStruct & getOutputLastFlit( int port, int vc ) const
        { return m_output_port[ port ][ vc ]->getLastFlit(); }
    const QString & inputPortState( int port, int vc ) const
        { return m_input_port[ port ][ vc ]->state(); }
    const QString & outputPortState( int port, int vc ) const
        { return m_output_port[ port ][ vc ]->state(); }

    const QVector< EsyDataItemTraffic > & pathPairList()
        { return m_path_pair_list; }

    // simulation event
    void insertInputPortFirst( int phy, int dst, const ShortFlitStruct & flit );
    void insertInputPortLast ( int phy, int dst, const ShortFlitStruct & flit );
    void removeInputPortFirst( int phy, int dst );
    void removeInputPortLast ( int phy, int dst );
    void insertOutputPortFirst( int phy, int dst, const ShortFlitStruct & flit );
    void insertOutputPortLast ( int phy, int dst, const ShortFlitStruct & flit );
    void removeOutputPortFirst( int phy, int dst );
    void removeOutputPortLast ( int phy, int dst );
    void insertRoutingTableFirst( int phy_i, int dst_i, int phy_o, int dst_o );
    void insertRoutingTableLast ( int phy_i, int dst_i, int phy_o, int dst_o );
    void removeRoutingTableFirst( int phy_i, int dst_i );
    void removeRoutingTableLast ( int phy_i, int dst_i );
    void insertAssignTable( int phy_i, int dst_i, int phy_o, int dst_o );
    void removeAssignTable( int phy_i, int dst_i );
    void setInputVCState( int phy, int dst, int state );
    void setOutputVCState( int phy, int dst, int state );

    void setFaulty( bool faulty ) { m_faulty_flag = faulty; }
    void setInputPortFaulty( int port, int vc, bool faulty );
    void setOutputPortFaulty( int port, int vc, bool faulty );
    void setPathFaulty( int inpc, int invc, int outpc, int outvc, bool faulty);

    void setTaskState( ProcessorStatus i, int appid, int taskid );

    // port position
    QPointF inputPortConnectPos( int phy );
    QPointF outputPortConnectPos( int phy );

	// rewrote draw function;
	// draw noc router
public:
    QRectF boundingRect() const;
private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	// draw functions;
private:
	// draw hard part of arrow: ----->
	void drawArrowHead(QPainter *painter, const QPointF & head, const QPointF & end, qreal size = 5, qreal angle = 45);
	// draw  end part of arrow: >-----
	void drawArrowEnd(QPainter *painter, const QPointF & head, const QPointF & end, qreal size = 5, qreal angle = 45);
	
	// simulate function
public:

	void setPath( long in_pc, long in_vc, long out_pc, long out_vc );
	void setPath( long in_pc, long in_vc, long out_pc, long out_vc, long count );
    void clearPath();
    void setPathMaxCount( long count );

    void incPortTimer();
    long portTimer( long phy, long vc )
        { return m_input_port[ phy ][ vc ]->timer(); }
    QVector< QPair< int, int > > turnVector( int phy, int vc );
};

#endif
