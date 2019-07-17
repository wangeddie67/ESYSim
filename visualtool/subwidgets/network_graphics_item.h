#ifndef ROUTER_GRAPHICS_ITEM_H
#define ROUTER_GRAPHICS_ITEM_H

#include "../define/qt_include.h"
#include "../modelsview/graphics_config_tree.h"
#include "esy_netcfg.h"

// draw one router
class PortGraphicsItem : public QGraphicsItem
{
private:
	static QPointF port_boundary_point[ 3 ];

	// constructor and deconstructor
public:
	PortGraphicsItem( qreal size, GraphicsConfigModel * graphics_cfg,
		QGraphicsItem * parent );

	// noc variable
private:
	qreal m_size;
	GraphicsConfigModel * mp_graphics_cfg;
	bool m_light;

	// rewrote draw function;
public:
	QRectF boundingRect() const;
	void setLightEnable( bool enable ) { m_light = enable; }

private:
	void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option, QWidget *widget);
};

// draw one router
class RouterGraphicsItem : public QGraphicsItem
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

	QVector< QVector< PortGraphicsItem * > > m_input_port;
	QVector< QVector< PortGraphicsItem * > > m_output_port;
	QVector< QVector< QPair< int, int > > > m_assign_table;

	// draw parameter
	QRectF m_router_rect;
	qreal m_router_width;
	QVector< qreal > m_port_offset;

	// I/O function
public:
	const EsyNetworkCfgRouter & routerCfg() const { return m_router_cfg; }

	// port position
	QPointF inputPortConnectPos( int phy );
	QPointF outputPortConnectPos( int phy );
	QPointF niConnectPos( int phy );

	// rewrote draw function;
	// draw noc router
public:
	QRectF boundingRect() const;
private:
	void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option, QWidget *widget);

	// draw functions;
private:
	// draw hard part of arrow: ----->
	void drawArrowHead(QPainter *painter, const QPointF & head,
		const QPointF & end, qreal size = 5, qreal angle = 45);
	// draw  end part of arrow: >-----
	void drawArrowEnd(QPainter *painter, const QPointF & head,
		const QPointF & end, qreal size = 5, qreal angle = 45);
};

// draw link
class LinkGraphicsItem : public QGraphicsLineItem
{
private:
	int m_src_id;
	int m_src_port;
	int m_des_id;
	int m_des_port;

	bool m_faulty_flag;
	bool m_path_flag;
	long m_max_traffic;
	long m_traffic;

	GraphicsConfigModel * m_graphics_cfg;

public:
	LinkGraphicsItem( int src_id, int src_port, int des_id, int des_port,
		GraphicsConfigModel * graphics_cfg ) :
		m_src_id( src_id ), m_src_port( src_port ),
		m_des_id( des_id ), m_des_port( des_port ),
		m_faulty_flag( false ), m_path_flag( false ),
		m_graphics_cfg( graphics_cfg )
	{
		setPen( m_graphics_cfg->configItem( "Link", "Normal" )->linePen() );
		setZValue( -10 );
	}

	int srcId() const { return m_src_id; }
	int srcPort() const { return m_src_port; }
	int desId() const { return m_des_id; }
	int desPort() const { return m_des_port; }

	void setDesId( int id ) { m_des_id = id; }
	void setDesPort( int port ) { m_des_port = port; }

	void setPath( bool path ) { m_path_flag = path; m_traffic = 0; }
	void addPath( bool path, long count )
		{ m_path_flag = path; m_traffic += count; }
	void setFaulty( bool faulty ) { m_faulty_flag = faulty; }
	void setMaxTraffic( long traffic ) { m_max_traffic = traffic; }

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
			   QWidget *widget);
};

#endif
