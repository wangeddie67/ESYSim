#include "network_graphics_item.h"

#define DRAW_VC_WIDTH    10
#define DRAW_ARROW_SIZE     2.5
#define DRAW_VC_SPLIT_WIDTH DRAW_VC_WIDTH
#define DRAW_VC_RECT \
	QRectF( 0, -DRAW_VC_HEIGHT / 2, DRAW_VC_WIDTH, DRAW_VC_HEIGHT )
#define DRAW_NI_SPERATE     DRAW_VC_WIDTH
#define DRAW_ROUTER_ARROW_SIZE  2.5
#define DRAW_ROUTER_ARROW_ANGLE 30
#define DRAW_TAN22P5D   0.4142
#define DRAW_SIN45D   0.7071
#define DRAW_COS45D   0.7071

// pi
#define PI 3.141592654

QPointF PortGraphicsItem::port_boundary_point[ 3 ] =
{ QPointF( - 0.5, 0 ), QPointF( 0.5, 0.5 ), QPointF( 0.5, -0.5 ) };

PortGraphicsItem::PortGraphicsItem(
	qreal size, GraphicsConfigModel *graphics_cfg, QGraphicsItem *parent
):
	QGraphicsItem( parent ), m_size( size ), mp_graphics_cfg( graphics_cfg ),
	m_light( false )
{}

QRectF PortGraphicsItem::boundingRect() const
{
	return QRectF( -0.5 * m_size, -0.5 * m_size, m_size, m_size );
}

void PortGraphicsItem::paint(
	QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED( widget );
	Q_UNUSED( option );

	// draw buffer
    GraphicsConfigModelItem * port_graphics_item =
        mp_graphics_cfg->configItem( "Port", "Normal" );
	painter->setPen( port_graphics_item->linePen() );
	QBrush brush = port_graphics_item->fillBrush();
	if ( m_light )
	{
		brush.setColor( Qt::green );
	}
	painter->setBrush( brush );

	QPointF port_boundary_point_t[ 3 ];
	for ( int i = 0; i < 3; i ++ )
    {
		port_boundary_point_t[ i ] = port_boundary_point[ i ] * m_size;
    }
    painter->drawConvexPolygon( port_boundary_point_t, 3 );
}

qreal RouterGraphicsItem::portdir_angle[ EsyNetworkCfgPort::ROUTER_PORT_NUM ] =
{
	 90, // PORTDIR_NORTH,
	270, // PORTDIR_SOUTH,
	  0, // PORTDIR_WEST,
	180, // PORTDIR_EAST,
	 45, // PORTDIR_NORTHWEST,
	135, // PORTDIR_NORTHEAST,
	315, // PORTDIR_SOUTHWEST,
	225  // PORTDIR_SOUTHEAST,
};

QPointF RouterGraphicsItem::portdir_radio_unit[
	EsyNetworkCfgPort::ROUTER_PORT_NUM ] =
{
	QPointF(             0, -           1 ), // PORTDIR_NORTH,
	QPointF(             0,             1 ), // PORTDIR_SOUTH,
	QPointF( -           1,             0 ), // PORTDIR_WEST,
	QPointF(             1,             0 ), // PORTDIR_EAST,
	QPointF( - DRAW_COS45D, - DRAW_SIN45D ), // PORTDIR_NORTHWEST,
	QPointF(   DRAW_COS45D, - DRAW_SIN45D ), // PORTDIR_NORTHEAST,
	QPointF( - DRAW_COS45D,   DRAW_SIN45D ), // PORTDIR_SOUTHWEST,
	QPointF(   DRAW_COS45D,   DRAW_SIN45D )  // PORTDIR_SOUTHEAST,
};

QPointF RouterGraphicsItem::portdir_circle_unit[
	EsyNetworkCfgPort::ROUTER_PORT_NUM ] =
{
	QPointF(             1,             0 ), // PORTDIR_NORTH,
	QPointF( -           1,             0 ), // PORTDIR_SOUTH,
	QPointF(             0, -           1 ), // PORTDIR_WEST,
	QPointF(             0,             1 ), // PORTDIR_EAST,
	QPointF(   DRAW_SIN45D, - DRAW_COS45D ), // PORTDIR_NORTHWEST,
	QPointF(   DRAW_SIN45D,   DRAW_COS45D ), // PORTDIR_NORTHEAST,
	QPointF( - DRAW_SIN45D, - DRAW_COS45D ), // PORTDIR_SOUTHWEST,
	QPointF( - DRAW_SIN45D,   DRAW_COS45D )  // PORTDIR_SOUTHEAST,
};

QPointF RouterGraphicsItem::router_boundary_point[
	EsyNetworkCfgPort::ROUTER_PORT_NUM ] =
{
	QPointF(   DRAW_TAN22P5D, -             1 ),
	QPointF(               1, - DRAW_TAN22P5D ),
	QPointF(               1,   DRAW_TAN22P5D ),
	QPointF(   DRAW_TAN22P5D,               1 ),
	QPointF( - DRAW_TAN22P5D,               1 ),
	QPointF( -             1,   DRAW_TAN22P5D ),
	QPointF( -             1, - DRAW_TAN22P5D ),
	QPointF( - DRAW_TAN22P5D, -             1 )
};

// ---- constructor and deconstructor ---- //
// constructor
RouterGraphicsItem::RouterGraphicsItem(
	const EsyNetworkCfgRouter & router_cfg,GraphicsConfigModel * graphics_cfg
):
	m_router_cfg( router_cfg ), mp_graphics_cfg( graphics_cfg )
{
	QVector< qreal > dir_width( EsyNetworkCfgPort::ROUTER_PORT_NUM, 0.0 );
	m_port_offset.fill( 0.0, m_router_cfg.portNum() );
	qreal max_dir_width = 0;
	// width for each direction
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		qreal port_width =
			m_router_cfg.port( port ).totalVcNumber() * DRAW_VC_WIDTH +
			DRAW_VC_SPLIT_WIDTH;
		m_port_offset[ port ] =
			dir_width[ m_router_cfg.port( port ).portDirection() ] +
			port_width / 2;
		dir_width[ m_router_cfg.port( port ).portDirection() ] += port_width;
	}
	// max width
	for ( int dir = 0; dir < EsyNetworkCfgPort::ROUTER_PORT_NUM; dir ++ )
	{
		if ( dir_width[ dir ] > max_dir_width )
		{
			max_dir_width = dir_width[ dir ];
		}
	}
	if ( max_dir_width < 2 * DRAW_VC_WIDTH )
	{
		max_dir_width = 2 * DRAW_VC_WIDTH;
	}
	// set rect of router
	m_router_width = 2 * ( max_dir_width * 0.5 ) / DRAW_TAN22P5D;
	m_router_rect.setX( - m_router_width / 2 - DRAW_VC_WIDTH / 2 );
	m_router_rect.setY( - m_router_width / 2 - DRAW_VC_WIDTH / 2 );
	m_router_rect.setWidth ( m_router_width + DRAW_VC_WIDTH );
	m_router_rect.setHeight( m_router_width + DRAW_VC_WIDTH );
	// update offset
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		m_port_offset[ port ] -=
			dir_width[ m_router_cfg.port( port ).portDirection() ] / 2;

		if ( m_router_cfg.port( port ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_SOUTH ||
			 m_router_cfg.port( port ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_EAST  ||
			 m_router_cfg.port( port ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_SOUTHWEST ||
			 m_router_cfg.port( port ).portDirection() ==
				EsyNetworkCfgPort::ROUTER_PORT_SOUTHEAST )
		{
			m_port_offset[ port ] = - m_port_offset[ port ];
		}
	}

	// intput and output field and routing table, resize port
	m_input_port.resize( m_router_cfg.portNum() );
	m_output_port.resize( m_router_cfg.portNum() );
	m_assign_table.resize( m_router_cfg.portNum() );

	// input and output field for each port
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		if ( m_router_cfg.port( port ).totalVcNumber() != 0 )
		{
			m_input_port[ port ].resize(
						m_router_cfg.port( port ).inputVcNumber() );
			m_output_port[ port ].resize(
						m_router_cfg.port( port ).outputVcNumber() );
			m_assign_table[ port ].resize(
						m_router_cfg.port( port ).inputVcNumber() );

			EsyNetworkCfgPort::RouterPortDirection dir =
					m_router_cfg.port( port ).portDirection();

			// for each vc
			for ( int vc = 0; vc < m_router_cfg.port( port ).inputVcNumber();
				vc ++ )
			{
				m_assign_table[ port ][ vc ] = QPair< int, int >( -1, -1 );
				// input field
				m_input_port[ port ][ vc ] =
					new PortGraphicsItem( DRAW_VC_WIDTH, mp_graphics_cfg, this );
				m_input_port[ port ][ vc ]->setPos(
					portdir_radio_unit[ dir ] * m_router_width / 2 +
					portdir_circle_unit[ dir ] * ( m_port_offset[ port ] -
						m_router_cfg.port( port ).totalVcNumber() *
							DRAW_VC_WIDTH / 2 +
						( vc + 0.5 ) * DRAW_VC_WIDTH ) );
				m_input_port[ port ][ vc ]->setRotation(
					portdir_angle[ dir ] - 180 );
			}
			// for each vc
			for ( int vc = 0; vc < m_router_cfg.port( port ).outputVcNumber();
				vc ++ )
			{
				// output field
				m_output_port[ port ][ vc ] =
					new PortGraphicsItem( DRAW_VC_WIDTH, mp_graphics_cfg, this );
				m_output_port[ port ][ vc ]->setPos(
					portdir_radio_unit[ dir ] * m_router_width / 2 +
					portdir_circle_unit[ dir ] * ( m_port_offset[ port ] +
						m_router_cfg.port( port ).totalVcNumber() *
							DRAW_VC_WIDTH / 2 -
						( vc + 0.5 ) * DRAW_VC_WIDTH ) );
				m_output_port[ port ][ vc ]->setRotation( portdir_angle[ dir ] );
			}
		}
	}

	setFlags( ItemIsSelectable );
	setAcceptHoverEvents( true );
}

// ---- constructor and deconstructor ---- //

// ---- draw router ---- //
// boundingRect
QRectF RouterGraphicsItem::boundingRect() const
{
	return m_router_rect;
}

// paint
void RouterGraphicsItem::paint( QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	Q_UNUSED( widget );

	if ( (option->state & QStyle::State_Selected) ||
		 (option->state & QStyle::State_MouseOver) )
	{
		painter->setPen(
			mp_graphics_cfg->configItem( "Component", "Selected" )->linePen() );
	}
	else
	{
		QPen t_pen = mp_graphics_cfg->configItem( "Component", "Normal" )->linePen();
		if ( option->state & QStyle::State_MouseOver )
		{
			QColor t_color = t_pen.color();
			t_color.setAlpha( 70 );
			t_pen.setColor( t_color );
		}
		painter->setPen( t_pen );
	}
	// draw router boundary
	QPointF router_boundary_point_t[ EsyNetworkCfgPort::ROUTER_PORT_NUM ];
	for ( int i = 0; i < EsyNetworkCfgPort::ROUTER_PORT_NUM; i ++ )
	{
		router_boundary_point_t[ i ] =
			router_boundary_point[ i ] * m_router_width / 2;
	}
	painter->drawPolygon( router_boundary_point_t,
		EsyNetworkCfgPort::ROUTER_PORT_NUM );

	// set font
	QFont coordfont = painter->font();
	coordfont.setPointSize( 15 );
	painter ->setFont( coordfont );
	// coordinate of router
	QString coordstr = QString( "%1" ).arg( m_router_cfg.routerId() );
	painter->drawText( boundingRect(), Qt::AlignCenter, coordstr );

	for( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		// draw arrow
		for( int vc = 0; vc < m_router_cfg.port( port ).inputVcNumber(); vc ++ )
		{
			painter->setPen( mp_graphics_cfg->
				configItem( "Link", "Normal" )->linePen() );
			if ( m_assign_table[ port ][ vc ].first >= 0 &&
				 m_assign_table[ port ][ vc ].second >= 0 )
			{
				// draw an arrow from source to destination
				QPointF srcpoint = mapFromItem(
					m_input_port[ port ][ vc ], QPointF( 0, 0 ) );
				QPointF dstpoint = mapFromItem(	m_output_port
					[ m_assign_table[ port ][ vc ].first ]
					[ m_assign_table[ port ][ vc ].second ],
					QPointF( 0, 0 ) );
				painter->drawLine( dstpoint, srcpoint );
			}
		}
	}
}

// draw head part of arrow
void RouterGraphicsItem::drawArrowHead(
	QPainter *painter, const QPointF & head, const QPointF & end,
	qreal size, qreal angle)
{
	painter->drawLine(head, end);
	// arrow line
	qreal arrowangle = qAtan2(end.x() - head.x(), end.y() - head.y());
	// left arrow line
	qreal radioleft = arrowangle + qreal(angle)/180*PI;
	painter->drawLine(head,
		head + QPointF(size * qSin(radioleft),  size * qCos(radioleft)));
	// right arrow line
	qreal radioright = arrowangle - qreal(angle)/180*PI;
	painter->drawLine(head,
		head + QPointF(size * qSin(radioright), size * qCos(radioright)));
}

// draw end part pf arrow
void RouterGraphicsItem::drawArrowEnd(
	QPainter *painter, const QPointF & head, const QPointF & end,
	qreal size, qreal angle)
{
	// new end pointer
	qreal arrowangle = qAtan2(end.x() - head.x(), end.y() - head.y());
	qreal sizeprojonarrow = size * qSin(qreal(angle)/180*PI);
	QPointF newend = end - QPointF(sizeprojonarrow * qSin(arrowangle),
		sizeprojonarrow * qCos(arrowangle));

	painter->drawLine(head, newend);
	// left arrow line
	qreal radioleft = arrowangle + qreal(angle)/180*PI;
	painter->drawLine(newend,
		newend + QPointF(size * qSin(radioleft),  size * qCos(radioleft)));
	// right arrow line
	qreal radioright = arrowangle - qreal(angle)/180*PI;
	painter->drawLine(newend,
		newend + QPointF(size * qSin(radioright), size * qCos(radioright)));
}

QPointF RouterGraphicsItem::inputPortConnectPos( int phy )
{
	if ( phy >= m_router_cfg.portNum() || phy < 0 )
	{
		return QPointF( 0.0, 0.0 );
	}
	else
	{
		EsyNetworkCfgPort::RouterPortDirection dir =
			m_router_cfg.port( phy ).portDirection();
		return portdir_radio_unit[ dir ] * m_router_width / 2 +
			portdir_circle_unit[ dir ] * ( m_port_offset[ phy ] -
				m_router_cfg.port( phy ).totalVcNumber() * DRAW_VC_WIDTH / 2 +
				m_router_cfg.port( phy ).inputVcNumber() * DRAW_VC_WIDTH / 2 );
	}
}

QPointF RouterGraphicsItem::outputPortConnectPos( int phy )
{
	if ( phy >= m_router_cfg.portNum() || phy < 0 )
	{
		return QPointF( 0.0, 0.0 );
	}
	else
	{
		EsyNetworkCfgPort::RouterPortDirection dir =
			m_router_cfg.port( phy ).portDirection();
		return portdir_radio_unit[ dir ] * m_router_width / 2 +
			portdir_circle_unit[ dir ] * ( m_port_offset[ phy ] +
				m_router_cfg.port( phy ).totalVcNumber() * DRAW_VC_WIDTH / 2 -
				m_router_cfg.port( phy ).outputVcNumber() * DRAW_VC_WIDTH / 2 );
	}
}

QPointF RouterGraphicsItem::niConnectPos( int phy )
{
	if ( phy >= m_router_cfg.portNum() || phy < 0 )
	{
		return QPointF( 0.0, 0.0 );
	}
	else
	{
		EsyNetworkCfgPort::RouterPortDirection dir =
			m_router_cfg.port( phy ).portDirection();
		return portdir_radio_unit[ dir ] *
				( m_router_width / 2 + DRAW_NI_SPERATE ) +
			portdir_circle_unit[ dir ] * m_port_offset[ phy ];
	}
}

void LinkGraphicsItem::paint(QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( m_path_flag )
	{
		setPen(
			m_graphics_cfg->configItem( "Link", "Normal" )->linePen() );
	}
	else
	{
		setPen( m_graphics_cfg->configItem( "Link",
			m_faulty_flag?("Faulty"):("Normal") )->linePen() );
	}

	QGraphicsLineItem::paint( painter, option, widget );
}
