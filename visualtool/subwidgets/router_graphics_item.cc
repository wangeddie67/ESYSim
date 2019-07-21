#include "router_graphics_item.h"

#define DRAW_VC_WIDTH    25.0
#define DRAW_ARROW_SIZE     10.0
#define DRAW_VC_HEIGHT   (3*DRAW_VC_WIDTH)
#define DRAW_VC_SPLIT_WIDTH (DRAW_VC_WIDTH)
#define DRAW_ROUTER_ARROW_SIZE  10
#define DRAW_ROUTER_ARROW_ANGLE 30
#define DRAW_TAN22P5D   0.4142
#define DRAW_SIN45D   0.7071
#define DRAW_COS45D   0.7071
// pi
#define PI 3.141592654

qreal RouterGraphicsItem::portdir_angle[
    EsyNetCfgPort::ROUTER_PORT_NUM ] =
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
    EsyNetCfgPort::ROUTER_PORT_NUM ] =
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
    EsyNetCfgPort::ROUTER_PORT_NUM ] =
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
    EsyNetCfgPort::ROUTER_PORT_NUM ] =
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
	const EsyNetCfgRouter & router_cfg,GraphicsConfigModel * graphics_cfg
):
	ProcessorItem( router_cfg.routerId() ),
	m_router_cfg( router_cfg ), mp_graphics_cfg( graphics_cfg ),
	m_faulty_flag( false )
{
	dir_width.fill( 0.0, EsyNetCfgPort::ROUTER_PORT_NUM );
	port_offset.fill( 0.0, m_router_cfg.portNum() );
	max_dir_width = 0;
	// width for each direction
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		long vc_t = m_router_cfg.port( port ).inputVcNumber() +
			m_router_cfg.port( port ).outputVcNumber();
		port_offset[ port ] =
			dir_width[ m_router_cfg.port( port ).portDirection() ] +
			( vc_t * DRAW_VC_WIDTH + DRAW_VC_SPLIT_WIDTH ) / 2;
		dir_width[ m_router_cfg.port( port ).portDirection() ] +=
			vc_t * DRAW_VC_WIDTH + DRAW_VC_SPLIT_WIDTH;
	}
	// max width
	for ( int dir = 0; dir < EsyNetCfgPort::ROUTER_PORT_NUM; dir ++ )
	{
		if ( dir_width[ dir ] > max_dir_width )
		{
			max_dir_width = dir_width[ dir ];
		}
	}
	// set rect of router
	router_width = 2 *
			( DRAW_VC_HEIGHT + ( max_dir_width * 0.5 ) / DRAW_TAN22P5D );
	router_rect.setX( - router_width / 2 - DRAW_ARROW_SIZE );
	router_rect.setY( - router_width / 2 - DRAW_ARROW_SIZE );
	router_rect.setWidth ( router_width + 2 * DRAW_ARROW_SIZE );
	router_rect.setHeight( router_width + 2 * DRAW_ARROW_SIZE );
	// update offset
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		port_offset[ port ] -=
			dir_width[ m_router_cfg.port( port ).portDirection() ] / 2;

		if ( m_router_cfg.port( port ).portDirection() ==
				EsyNetCfgPort::ROUTER_PORT_SOUTH ||
			 m_router_cfg.port( port ).portDirection() ==
				EsyNetCfgPort::ROUTER_PORT_EAST  ||
			 m_router_cfg.port( port ).portDirection() ==
				EsyNetCfgPort::ROUTER_PORT_SOUTHWEST ||
			 m_router_cfg.port( port ).portDirection() ==
				EsyNetCfgPort::ROUTER_PORT_SOUTHEAST )
		{
			port_offset[ port ] = - port_offset[ port ];
		}
	}

	// intput and output field and routing table, resize port
	m_input_port.resize( m_router_cfg.portNum() );
	m_output_port.resize( m_router_cfg.portNum() );
	m_routing_table.resize( m_router_cfg.portNum() );
	m_assign_table.resize( m_router_cfg.portNum() );

	// input and output field for each port
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		if ( m_router_cfg.port( port ).totalVcNumber() != 0 )
		{
			// resize vc for each port
			m_input_port[ port ].resize(
						m_router_cfg.port( port ).inputVcNumber() );
			m_output_port[ port ].resize(
						m_router_cfg.port( port ).outputVcNumber() );
			m_routing_table[ port ].resize(
						m_router_cfg.port( port ).inputVcNumber() );
			m_assign_table[ port ].resize(
						m_router_cfg.port( port ).inputVcNumber() );

			EsyNetCfgPort::RouterPortDirection dir =
					m_router_cfg.port( port ).portDirection();

			// for each vc
			for ( int vc = 0; vc < m_router_cfg.port( port ).inputVcNumber();
				  vc ++ )
			{
				m_routing_table[ port ][ vc ] .resize(0);
				m_assign_table[ port ][ vc ] = QPair< int, int >( -1, -1 );
				// input field
				m_input_port[ port ][ vc ] = new PortGraphicsItem(
					DRAW_VC_WIDTH, DRAW_VC_HEIGHT,
					m_router_cfg.port( port ).inputBuffer(),
					mp_graphics_cfg, this );
				m_input_port[ port ][ vc ]->setPos(
					portdir_radio_unit[ dir ] *
						( router_width / 2 - DRAW_VC_HEIGHT ) +
					portdir_circle_unit[ dir ] * ( port_offset[ port ] -
						m_router_cfg.port( port ).totalVcNumber() *
							DRAW_VC_WIDTH / 2 +
						( vc + 0.5 ) * DRAW_VC_WIDTH ) );
				m_input_port[ port ][ vc ]->setRotation(
					portdir_angle[ dir ] - 180 );
			}
			// output field
			for ( int vc = 0; vc < m_router_cfg.port( port ).outputVcNumber();
				  vc ++ )
			{
				m_output_port[ port ][ vc ] = new PortGraphicsItem(
					DRAW_VC_WIDTH, DRAW_VC_HEIGHT,
					m_router_cfg.port( port ).outputBuffer(),
					mp_graphics_cfg, this );
				m_output_port[ port ][ vc ]->setPos(
					portdir_radio_unit[ dir ] * router_width / 2 +
					portdir_circle_unit[ dir ] * ( port_offset[ port ] +
						m_router_cfg.port( port ).totalVcNumber() *
							DRAW_VC_WIDTH / 2 -
						( vc + 0.5 ) * DRAW_VC_WIDTH ) );
				m_output_port[ port ][ vc ]->setRotation( portdir_angle[ dir ] );
			}

			// ni
			if ( m_router_cfg.port( port ).networkInterface() )
			{
				NIGraphicsItem * t_ni_item = new NIGraphicsItem(
					m_router_cfg.port( port ).totalVcNumber() * DRAW_VC_WIDTH,
					DRAW_VC_WIDTH, DRAW_VC_WIDTH, mp_graphics_cfg, this );
				t_ni_item ->setPos( portdir_radio_unit[ dir ] * router_width / 2 +
					portdir_circle_unit[ dir ] * port_offset[ port ] );
				t_ni_item ->setRotation( portdir_angle[ dir ] + 90 );
				QVector< QPointF > max_position;
				max_position .append( t_ni_item ->mapToItem( this,
					QPointF( - m_router_cfg.port( port ).totalVcNumber() *
					DRAW_VC_WIDTH / 2, DRAW_VC_WIDTH + DRAW_VC_WIDTH ) ) );
				max_position .append( t_ni_item ->mapToItem( this,
					QPointF( - m_router_cfg.port( port ).totalVcNumber() *
					DRAW_VC_WIDTH / 2, DRAW_VC_WIDTH + DRAW_VC_WIDTH ) ) );
				for ( int i = 0; i < max_position .size(); i ++ )
				{
					if ( max_position[ i ] .rx() < router_rect .x() )
					{
						router_rect .setWidth( router_rect.width() +
							router_rect .x() - max_position[ i ] .rx() );
						router_rect .setX( max_position[ i ] .rx() );
					}
					if ( max_position[ i ] .rx() >
						 router_rect.width() + router_rect .x() )
					{
						router_rect .setWidth(
							max_position[ i ] .rx() - router_rect .x() );
					}
					if ( max_position[ i ] .ry() < router_rect .y() )
					{
						router_rect .setHeight( router_rect.height() +
							router_rect .y() - max_position[ i ] .ry() );
						router_rect .setY( max_position[ i ] .ry() );
					}
					if ( max_position[ i ] .ry() >
						 router_rect.height() + router_rect .y() )
					{
						router_rect .setHeight( max_position[ i ] .ry() -
												router_rect .y() );
					}
				}
			}
		}
	}
	
	setFlags( ItemIsSelectable );
	setAcceptHoverEvents( true );

	m_path_max_count = -1;
}

// ---- constructor and deconstructor ---- //

// ---- draw router ---- //
// boundingRect
QRectF RouterGraphicsItem::boundingRect() const
{
	return router_rect;
}

// paint
void RouterGraphicsItem::paint( QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	Q_UNUSED( widget );

	// set pen style
	if ( m_faulty_flag )
	{
		painter->setPen(
			mp_graphics_cfg->configItem( "Router", "Faulty" )->linePen() );
	}
	else if ( (option->state & QStyle::State_Selected) ||
		 (option->state & QStyle::State_MouseOver) )
	{
		painter->setPen(
			mp_graphics_cfg->configItem( "Router", "Selected" )->linePen() );
	}
	else
	{
		QPen t_pen = mp_graphics_cfg->configItem( "Router", "Normal" )->linePen();
		if ( option->state & QStyle::State_MouseOver )
		{
			QColor t_color = t_pen.color();
			t_color.setAlpha( 70 );
			t_pen.setColor( t_color );
		}
		painter->setPen( t_pen );
	}
	// draw router boundary
	QPointF router_boundary_point_t[ EsyNetCfgPort::ROUTER_PORT_NUM ];
	for ( int i = 0; i < EsyNetCfgPort::ROUTER_PORT_NUM; i ++ )
	{
		router_boundary_point_t[ i ] =
			router_boundary_point[ i ] * router_width / 2;
	}
	painter->drawPolygon( router_boundary_point_t,
		EsyNetCfgPort::ROUTER_PORT_NUM );

	// draw pe
	if ( m_task_status != PE_RELEASE )
	{
		QPainterPath path;
		path.moveTo(router_boundary_point[ 0 ]* router_width / 4);
		for ( int i = 1; i < EsyNetCfgPort::ROUTER_PORT_NUM; i ++ )
		{
			path.lineTo(router_boundary_point[ i ] * router_width / 4);
		}
		path.lineTo(router_boundary_point[ 0 ] * router_width / 4);

		painter->drawPath( path );
		painter->fillPath( path, QBrush( m_color ) );
	}

	for( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		EsyNetCfgPort::RouterPortDirection t_dir =
				m_router_cfg.port( port ).portDirection();
		if( m_router_cfg.port( port ).inputVcNumber() != 0 )
		{
			QPointF t_input_point = inputPortConnectPos( port );
			painter ->drawLine( t_input_point, t_input_point +
				portdir_radio_unit[ t_dir ] * DRAW_ARROW_SIZE +
				portdir_circle_unit[ t_dir ] * DRAW_ARROW_SIZE );
			painter ->drawLine( t_input_point, t_input_point +
				portdir_radio_unit[ t_dir ] * DRAW_ARROW_SIZE -
				portdir_circle_unit[ t_dir ] * DRAW_ARROW_SIZE );
		}
		if( m_router_cfg.port( port ).outputVcNumber() != 0 )
		{
			QPointF t_output_point = outputPortConnectPos( port );
			painter ->drawLine( t_output_point +
				portdir_radio_unit[ t_dir ] * DRAW_ARROW_SIZE,
				t_output_point +
				portdir_circle_unit[ t_dir ] * DRAW_ARROW_SIZE );
			painter ->drawLine( t_output_point +
				portdir_radio_unit[ t_dir ] * DRAW_ARROW_SIZE,
				t_output_point -
				portdir_circle_unit[ t_dir ] * DRAW_ARROW_SIZE );
		}
	}

	// set font
	QFont coordfont =  painter ->font();
	coordfont .setPointSize( 30 );
	painter ->setFont( coordfont );
	// coordinate of router
	QString coordstr = "";
	if ( m_task_status == PE_RELEASE )
	{
		coordstr = QString( "(%1)" ).arg( m_router_cfg.routerId() );
	}
	else
	{
		coordstr = QString( "(%1)\n%2 - %3\n%4" )
			.arg( m_router_cfg.routerId() )
			.arg(m_app_id).arg(m_task_id)
			.arg( const_pe_status_list[ m_task_status ] );
	}
	painter->drawText( boundingRect(), Qt::AlignCenter, coordstr );

	// set font
	QFont statefont =  painter ->font();
    statefont .setPointSize( DRAW_VC_WIDTH );
	painter -> setFont( statefont );

	for( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		// draw arrow
		for( int vc = 0; vc < m_router_cfg.port( port ).inputVcNumber(); vc ++ )
		{
			// draw routing arrow
			if ( m_input_port[ port ][ vc ]->faulty() )
			{
				painter->setPen( mp_graphics_cfg->configItem(
					"Require", "Faulty" )->linePen() );
			}
			else
			{
				painter->setPen( mp_graphics_cfg->configItem(
					"Require", "Normal" )->linePen() );
			}
			for( int item = 0; item < m_routing_table[ port ][ vc ].size();
					item ++ )
			{
				// draw an arrow from source to destination
				QPointF srcpoint = mapFromItem(
					m_input_port[ port ][ vc ], QPointF( 0, 0 ) );
				QPointF dstpoint = mapFromItem( m_output_port
						[ m_routing_table[ port ][ vc ][ item ].first ]
						[ m_routing_table[ port ][ vc ][ item ].second ],
					QPointF( DRAW_VC_HEIGHT, 0 ) );
				drawArrowHead( painter, dstpoint, srcpoint,
					DRAW_ROUTER_ARROW_SIZE, DRAW_ROUTER_ARROW_ANGLE );
			}
			// draw assign routing arrow
			if ( m_input_port[ port ][ vc ]->faulty() )
			{
				painter->setPen( mp_graphics_cfg->configItem(
					"Assign", "Faulty" )->linePen() );
			}
			else
			{
				painter->setPen( mp_graphics_cfg->configItem(
					"Assign", "Normal" )->linePen() );
			}
			if ( m_assign_table[ port ][ vc ].first >= 0 &&
					m_assign_table[ port ][ vc ].second >= 0 )
			{
				// draw an arrow from source to destination
				QPointF srcpoint = mapFromItem(
					m_input_port[ port ][ vc ], QPointF( 0, 0 ) );
				QPointF dstpoint = mapFromItem(
					m_output_port[ m_assign_table[ port ][ vc ].first ]
						[ m_assign_table[ port ][ vc ].second ],
					QPointF( DRAW_VC_HEIGHT, 0 ) );
				drawArrowHead( painter, dstpoint, srcpoint,
					DRAW_ROUTER_ARROW_SIZE, DRAW_ROUTER_ARROW_ANGLE );
			}
		}
	}
	// draw path
	for ( int i = 0; i < m_path_pair_list .size() ; i ++ )
	{
		if ( m_path_pair_list[ i ].inputPc() < 0 ||
			 m_path_pair_list[ i ].inputPc() >= m_input_port.size() )
		{
			continue;
		}
		if ( m_path_pair_list[ i ].inputVc() < 0 ||
			 m_path_pair_list[ i ].inputVc() >=
			 m_input_port[ m_path_pair_list[ i ].inputPc() ].size() )
		{
			continue;
		}
		if ( m_path_pair_list[ i ].outputPc() < 0 ||
			 m_path_pair_list[ i ].outputPc() >= m_output_port.size() )
		{
			continue;
		}
		if ( m_path_pair_list[ i ].outputVc() < 0 ||
			 m_path_pair_list[ i ].outputVc() >=
			 m_output_port[ m_path_pair_list[ i ].outputPc() ].size() )
		{
			continue;
		}
		QPen t_pen;
		if ( m_path_pair_list[ i ].faulty() )
		{
			t_pen = mp_graphics_cfg->configItem( "Path", "Faulty" )->linePen();
		}
		else
		{
			t_pen = mp_graphics_cfg->configItem( "Path", "Normal" )->linePen();
		}
		if ( m_path_max_count > 0 )
		{
			t_pen.setWidthF( m_path_pair_list[ i ].traffic() * 10 /
				m_path_max_count + 1 );
		}
		painter->setPen( t_pen );

		QPointF srcpoint, dstpoint;

		srcpoint = mapFromItem(
			m_input_port[ m_path_pair_list[ i ].inputPc() ]
				[ m_path_pair_list[ i ].inputVc() ], QPointF( 0, 0 ) );
		dstpoint = mapFromItem(
			m_output_port[ m_path_pair_list[ i ].outputPc() ]
				[ m_path_pair_list[ i ].outputVc() ],
			QPointF( DRAW_VC_HEIGHT, 0 ) );
		drawArrowHead( painter, dstpoint, srcpoint,
			DRAW_ROUTER_ARROW_SIZE, DRAW_ROUTER_ARROW_ANGLE );
	}
}

// draw head part of arrow
void RouterGraphicsItem::drawArrowHead( QPainter *painter,
	const QPointF & head, const QPointF & end, qreal size, qreal angle)
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
void RouterGraphicsItem::drawArrowEnd( QPainter *painter,
	const QPointF & head, const QPointF & end, qreal size, qreal angle)
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
		newend + QPointF(size * qSin(radioleft), size * qCos(radioleft)));
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
		EsyNetCfgPort::RouterPortDirection dir =
			m_router_cfg.port( phy ).portDirection();
		return portdir_radio_unit[ dir ] * router_width / 2 +
			portdir_circle_unit[ dir ] * ( port_offset[ phy ] -
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
		EsyNetCfgPort::RouterPortDirection dir =
			m_router_cfg.port( phy ).portDirection();
		return portdir_radio_unit[ dir ] * router_width / 2 +
			portdir_circle_unit[ dir ] * ( port_offset[ phy ] +
				m_router_cfg.port( phy ).totalVcNumber() * DRAW_VC_WIDTH / 2 -
				m_router_cfg.port( phy ).outputVcNumber() * DRAW_VC_WIDTH / 2 );
	}
}

// ---- simulation functions ---- //
void RouterGraphicsItem::insertInputPortFirst(
	int phy, int dst, const ShortFlitStruct & flit )
{
	m_input_port[ phy ][ dst ] ->prepend( flit );
}

void RouterGraphicsItem::insertInputPortLast(
	int phy, int dst, const ShortFlitStruct & flit )
{
	m_input_port[ phy ][ dst ] ->append( flit );
}

void RouterGraphicsItem::removeInputPortFirst( int phy, int dst )
{
	m_input_port[ phy ][ dst ] ->removeFirst();
}

void RouterGraphicsItem::removeInputPortLast( int phy, int dst )
{
	m_input_port[ phy ][ dst ] ->removeLast();
}

void RouterGraphicsItem::insertOutputPortFirst(
	int phy, int dst, const ShortFlitStruct & flit )
{
	m_output_port[ phy ][ dst ] ->prepend( flit );
}

void RouterGraphicsItem::insertOutputPortLast(
	int phy, int dst, const ShortFlitStruct & flit )
{
	m_output_port[ phy ][ dst ] ->append( flit );
}

void RouterGraphicsItem::removeOutputPortFirst( int phy, int dst )
{
	m_output_port[ phy ][ dst ] ->removeFirst();
}

void RouterGraphicsItem::removeOutputPortLast( int phy, int dst )
{
	m_output_port[ phy ][ dst ] ->removeLast();
}

void RouterGraphicsItem::insertRoutingTableFirst(
	int phy_i, int dst_i, int phy_o, int dst_o )
{
	m_routing_table[ phy_i ][ dst_i ] .prepend(
		QPair< int, int >( phy_o, dst_o) );
}

void RouterGraphicsItem::insertRoutingTableLast(
	int phy_i, int dst_i, int phy_o, int dst_o )
{
	m_routing_table[ phy_i ][ dst_i ].append(
		QPair< int, int >( phy_o, dst_o) );
}

void RouterGraphicsItem::removeRoutingTableFirst( int phy_i, int dst_i )
{
	m_routing_table[ phy_i ][ dst_i ] .remove( 0 );
}

void RouterGraphicsItem::removeRoutingTableLast( int phy_i, int dst_i )
{
	m_routing_table[ phy_i ][ dst_i ].remove(
		m_routing_table[ phy_i ][ dst_i ] .size() - 1 );
}

void RouterGraphicsItem::insertAssignTable(
	int phy_i, int dst_i, int phy_o, int dst_o )
{
	m_assign_table[ phy_i ][ dst_i ] = QPair< int, int >( phy_o, dst_o);
}

void RouterGraphicsItem::removeAssignTable( int phy_i, int dst_i )
{
	m_assign_table[ phy_i ][ dst_i ] = QPair< int, int >( -1, -1 );
}

void RouterGraphicsItem::setInputVCState( int phy, int dst, int state )
{
	m_input_port[ phy ][ dst ] ->setState( state );
}

void RouterGraphicsItem::setOutputVCState( int phy, int dst, int state )
{
	m_output_port[ phy ][ dst ] ->setState( state );
}

void RouterGraphicsItem::setPath(
	long in_pc, long in_vc, long out_pc, long out_vc )
{  
	m_path_pair_list.append( EsyDataItemTraffic(
		 routerCfg().routerId(), in_pc, in_vc, out_pc, out_vc, 1 ) );
	m_input_port[ in_pc ][ in_vc ]->setPath( true );
	m_output_port[ out_pc ][ out_vc ]->setPath( true );
}

void RouterGraphicsItem::setPath(
	long in_pc, long in_vc, long out_pc, long out_vc, long count )
{  
	m_path_pair_list.append( EsyDataItemTraffic(
		 routerCfg().routerId(), in_pc, in_vc, out_pc, out_vc, count ) );
	m_input_port[ in_pc ][ in_vc ]->setPath( true );
	m_input_port[ in_pc ][ in_vc ]->incTraffic( count );
	m_output_port[ out_pc ][ out_vc ]->setPath( true );
	m_output_port[ out_pc ][ out_vc ]->incTraffic( count );
}

void RouterGraphicsItem::setInputPortFaulty( int port, int vc, bool faulty )
{
	for ( int i = 0; i < m_input_port[ port ].size(); i ++ )
	{
		if ( (vc == -1) || (i == vc) )
		{
			m_input_port[ port ][ i ]->setFaulty( faulty );
		}
	}
}

void RouterGraphicsItem::setOutputPortFaulty( int port, int vc, bool faulty )
{
	for ( int i = 0; i < m_input_port[ port ].size(); i ++ )
	{
		if ( (vc == -1) || (i == vc) )
		{
			m_output_port[ port ][ i ]->setFaulty( faulty );
		}
	}
}

void RouterGraphicsItem::setPathFaulty(
	int inpc, int invc, int outpc, int outvc, bool faulty)
{
	for ( int i = 0; i < m_path_pair_list.size(); i ++ )
	{
		if ( m_path_pair_list[i].inputPc() == inpc &&
			 m_path_pair_list[i].inputVc() == invc &&
			 m_path_pair_list[i].outputPc() == outpc &&
			 m_path_pair_list[i].outputVc() == outvc )
		{
			m_input_port[ inpc ][ invc ]->setFaulty( true );
			m_path_pair_list[i].setFaulty( faulty );
			m_output_port[ outpc ][ outvc ]->setFaulty( true );
		}
	}
}

void RouterGraphicsItem::clearPath()
{
	for ( int i = 0; i < m_path_pair_list.size(); i ++ )
	{
		m_input_port[ m_path_pair_list[ i ].inputPc() ]
				[ m_path_pair_list[ i ].inputVc() ]->setPath( false );
		m_input_port[ m_path_pair_list[ i ].inputPc() ]
				[ m_path_pair_list[ i ].inputVc() ]->clearTraffic();
		m_output_port[ m_path_pair_list[ i ].outputPc() ]
				[ m_path_pair_list[ i ].outputVc() ]->setPath( false );
		m_output_port[ m_path_pair_list[ i ].outputPc() ]
				[ m_path_pair_list[ i ].outputVc() ]->clearTraffic();
	}
	m_path_pair_list.clear();
}

void RouterGraphicsItem::setPathMaxCount( long count )
{
	m_path_max_count = count;
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		for ( long vc = 0; vc < m_router_cfg.port( port ).inputVcNumber();
			  vc ++ )
		{
			m_input_port[ port ][ vc ]->setMaxTraffic( count );
		}
		for ( long vc = 0; vc < m_router_cfg.port( port ).outputVcNumber();
			  vc ++ )
		{
			m_output_port[ port ][ vc ]->setMaxTraffic( count );
		}
	}
}

void RouterGraphicsItem::incPortTimer()
{
	for ( long port = 0; port < m_router_cfg.portNum(); port ++ )
	{
		for ( long vc = 0; vc < m_router_cfg.port( port ).inputVcNumber();
			  vc ++ )
		{
			if ( m_input_port[ port ][ vc ]->flitSize() > 0 )
			{
				// resize vc for each port
				m_input_port[ port ][ vc ]->incTimer();
			}
		}
	}
}

QVector< QPair< int, int > > RouterGraphicsItem::turnVector( int phy, int vc )
{
	// input and output field for each port
	QVector< QPair< int, int > > t_turn;
	if ( m_assign_table[ phy ][ vc ].first >= 0 ||
		 m_assign_table[ phy ][ vc ].second >= 0 )
	{
		t_turn.append( m_assign_table[ phy ][ vc ] );
		return t_turn;
	}
	for ( int i = 0; i < m_routing_table[ phy ][ vc ].size(); i ++ )
	{
		t_turn.append( m_routing_table[ phy ][ vc ][ i ] );
	}
	return t_turn;
}

void RouterGraphicsItem::setTaskState( ProcessorStatus status, int appid, int taskid )
{
	m_task_status = status;
	if ( status == ProcessorItem::PE_RELEASE )
	{
		m_app_id = -1;
		m_task_id = -1;
	}
	else
	{
		m_app_id = appid;
		m_task_id = taskid;

		qsrand(m_app_id);
		int r=qrand() % 150 + 100;
		qsrand(m_app_id + 555);
		int g=qrand() % 150 + 100;
		qsrand(m_app_id + 2512);
		int b=qrand() % 150 + 100;
		m_color = QColor( r, g, b );
	}
}
