#include "soc_view.h"
#include "esy_eventtype.h"
#include "esy_faultcfg.h"

#define DRAW_ROUTER_CONNECT_LENGTH  100

// ---- constructor and destructor ---- //
SoCView::SoCView ( QWidget * parent ) : QGraphicsView( parent )
{
	sx = 250;
	sy = 250;
}

SoCView::~SoCView()
{
}
// ---- constructor and destructor ---- //


// ---- slot functions ---- //
void SoCView::wheelEvent(QWheelEvent * event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		if (event->delta() > 0)
		{
			sx += 6; sy += 6;
		}
		else
		{
			sx -= 6; sy -= 6;
		}
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		QMatrix matrix;
		matrix.scale(qPow(qreal(2), (sx - 250)/qreal(50)),
					 qPow(qreal(2), (sx - 250)/qreal(50)));
		matrix.rotate(0);
		setMatrix(matrix);
		event->accept();
	}
	else if (event->modifiers() & Qt::ShiftModifier)
	{
		horizontalScrollBar()->setSliderPosition(
			horizontalScrollBar()->sliderPosition() + event->delta() );
	}
	else
	{
		QGraphicsView::wheelEvent( event );
	}
}

void SoCView::keyReleaseEvent(QKeyEvent * event)
{
	switch( event ->key() )
	{
		case Qt::Key_Equal:
			wheelEvent( new QWheelEvent( QCursor::pos(), 10,
				Qt::NoButton, Qt::ControlModifier, Qt::Vertical ) );
			break;
		case Qt::Key_Minus:
			wheelEvent( new QWheelEvent( QCursor::pos(), -10,
				Qt::NoButton, Qt::ControlModifier, Qt::Vertical ) );
			break;
		default: emit hotKeyEvent( event ); break;
	}
}

void SoCView::mouseReleaseEvent(QMouseEvent * event)
{
	emit selectRouterEvent( event );
}

void SoCScene::UpdateTooltip()
{
	QGraphicsItem * tmp = itemAt( m_last_scene_pos, QTransform() );

	if ( tmp )
	{
		if ( !tmp->toolTip().isEmpty() )
		{
			QToolTip::showText( m_last_screen_pos, tmp->toolTip() );
		}
	}
}

void SoCScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
	m_last_pos = mouseEvent->lastPos();
	m_last_scene_pos = mouseEvent->lastScenePos();
	m_last_screen_pos = mouseEvent->lastScreenPos();

	QGraphicsScene::mouseMoveEvent( mouseEvent );
}

// initialize noc scence
void SoCScene::drawSoCScene()
{
	// generate network
	for ( int t_index = 0;
		t_index < mp_soc_cfg->network().routerCount(); t_index ++ )
	{
		// new router
		RouterGraphicsItem *item = new RouterGraphicsItem(
			mp_soc_cfg->network().router( t_index ), mp_graphics_cfg );
		m_router_vector.append( item );
		addItem( item );
		// set parameter
//		if ( mp_soc_cfg->network().router( t_index ).pos().first == 0.0 &&
//			 mp_soc_cfg->network().router( t_index ).pos().second == 0.0 )
//		{
			if ( ( mp_soc_cfg->network().topology() ==
				   EsyNetworkCfg::NOC_TOPOLOGY_2DMESH ) ||
				 ( mp_soc_cfg->network().topology() ==
				   EsyNetworkCfg::NOC_TOPOLOGY_2DTORUS ) )
			{
				vector< long > t_ax =
					mp_soc_cfg->network().seq2Coord( t_index );
				int t_ax_x = t_ax[ EsyNetworkCfg::AX_X ];
				int t_ax_y = t_ax[ EsyNetworkCfg::AX_Y ];
				item->setPos( DRAW_ROUTER_CONNECT_LENGTH * t_ax_x,
					DRAW_ROUTER_CONNECT_LENGTH * t_ax_y );
			}
			else
			{
				item->setPos( DRAW_ROUTER_CONNECT_LENGTH * t_index, 0.0 );
			}
//		}
//		else
//		{
//			item->setPos( mp_soc_cfg->network().router( t_index ).pos().first,
//				mp_soc_cfg->network().router( t_index ).pos().second );
//		}
	}
	// generate tile
    for ( int t_index = 0;
		t_index < (long)mp_soc_cfg->tileCount(); t_index ++ )
	{
		// new tile
		TileGraphicsItem *item = new TileGraphicsItem(
					mp_soc_cfg->tile( t_index ), mp_graphics_cfg );
		m_tile_vector.append( item );
		addItem( item );

        long ni_id = mp_soc_cfg->tile( t_index ).niId();
        long ni_port = mp_soc_cfg->tile( t_index ).niPhy();
        QPointF ni_pos = m_router_vector[ ni_id ]->mapToScene(
            m_router_vector[ ni_id ]->niConnectPos( ni_port ) );
        item->setNiPos( ni_pos );
        mp_soc_cfg->tile( t_index ).setPos(
            item->pos().x(), item->pos().y() );
	}
    regularScene();

    // connect network
	for ( int t_index = 0;
		t_index < mp_soc_cfg->network().routerCount(); t_index ++ )
	{
		EsyNetworkCfgRouter t_router_cfg =
			mp_soc_cfg->network().router( t_index );
		// new link
		for ( long t_port_index = 0;
			t_port_index < t_router_cfg.portNum(); t_port_index ++ )
		{
			LinkGraphicsItem * t_link = addNetworkLink( t_index, t_port_index );
			if ( t_link )
			{
				m_link_vector.append( t_link );
				addItem( t_link );
			}
		}
	}
	// connect ni
	for ( int t_index = 0;
		t_index < (long)mp_soc_cfg->tileCount(); t_index ++ )
	{
		LinkGraphicsItem * t_in_link  = addTileLink( t_index, true );
		addItem( t_in_link );
		m_ni_vector.append( t_in_link );
		LinkGraphicsItem * t_out_link  = addTileLink( t_index, false );
		addItem( t_out_link );
		m_ni_vector.append( t_out_link );
    }
}

LinkGraphicsItem * SoCScene::addNetworkLink( int src_id, int src_port )
{
	if ( src_id < 0 || src_id >= mp_soc_cfg->network().routerCount() )
	{
		return 0;
	}
	if ( src_port < 0 ||
		src_port >= mp_soc_cfg->network().router( src_id ).portNum() )
	{
		return 0;
	}

	EsyNetworkCfgRouter t_router_cfg = mp_soc_cfg->network().router( src_id );

	if ( t_router_cfg.port( src_port ).neighborRouter() < 0 ||
		 t_router_cfg.port( src_port ).neighborRouter() >=
		 mp_soc_cfg->network().routerCount() )
	{
		return 0;
	}
	if ( t_router_cfg.port( src_port ).neighborPort() < 0 ||
		 t_router_cfg.port( src_port ).neighborPort() >=
		 mp_soc_cfg->network().router(
			 t_router_cfg.port( src_port ).neighborRouter() ).portNum() )
	{
		return 0;
	}

	LinkGraphicsItem * t_link = new LinkGraphicsItem( src_id, src_port,
				t_router_cfg.port( src_port ).neighborRouter(),
				t_router_cfg.port( src_port ).neighborPort(), mp_graphics_cfg );

	QPointF t_src_point =
		m_router_vector[ src_id ]->mapToScene(
		m_router_vector[ src_id ]->outputPortConnectPos( src_port ) );
	QPointF t_dst_point =
		m_router_vector[ t_router_cfg.port( src_port ).neighborRouter() ]->
			mapToScene( m_router_vector[
				t_router_cfg.port( src_port ).neighborRouter() ]->
			inputPortConnectPos(
				t_router_cfg.port( src_port ).neighborPort() ) );

	t_link->setLine( t_src_point.x(), t_src_point.y(),
				   t_dst_point.x(), t_dst_point.y() );

	return t_link;
}

LinkGraphicsItem * SoCScene::addTileLink( int tile, bool in )
{
	if ( tile < 0 || tile >= (long)mp_soc_cfg->tileCount() )
	{
		return 0;
	}

	EsySoCCfgTile t_tile_cfg = mp_soc_cfg->tile( tile );

	if ( t_tile_cfg.niId() < 0 ||
		 t_tile_cfg.niId() >= mp_soc_cfg->network().routerCount() )
	{
		return 0;
	}
	if ( t_tile_cfg.niPhy() < 0 ||
		 t_tile_cfg.niPhy() >= mp_soc_cfg->network().router(
			 t_tile_cfg.niId() ).portNum() )
	{
		return 0;
	}

	LinkGraphicsItem * t_link = new LinkGraphicsItem( tile, -1,
		t_tile_cfg.niId(), t_tile_cfg.niPhy(), mp_graphics_cfg );

	QPointF t_src_point, t_dst_point;
	if ( in )
	{
		t_src_point = m_tile_vector[ tile ]->mapToScene(
			m_tile_vector[ tile ]->outputPortConnectPos() );
		t_dst_point = m_router_vector[ t_tile_cfg.niId() ]->
			mapToScene( m_router_vector[ t_tile_cfg.niId() ]->
				inputPortConnectPos( t_tile_cfg.niPhy() ) );
	}
	else
	{
		t_src_point = m_tile_vector[ tile ]->mapToScene(
			m_tile_vector[ tile ]->inputPortConnectPos() );
		t_dst_point = m_router_vector[ t_tile_cfg.niId() ]->
			mapToScene( m_router_vector[ t_tile_cfg.niId() ]->
				outputPortConnectPos( t_tile_cfg.niPhy() ) );
	}

	t_link->setLine( t_src_point.x(), t_src_point.y(),
				   t_dst_point.x(), t_dst_point.y() );

	return t_link;
}

void SoCScene::regularScene()
{
	// get size of each tile
	QVector< QRectF > rect_r;
	for ( int router = 0; router < mp_soc_cfg->network().routerCount();
		router ++ )
	{
		rect_r.append( m_router_vector[ router ]->sceneBoundingRect() );
	}
	for ( size_t tile = 0; tile < mp_soc_cfg->tileCount(); tile ++ )
	{
        size_t ni_id = mp_soc_cfg->tile( tile ).niId();
		rect_r[ ni_id ] = rect_r[ ni_id ].united(
			m_tile_vector[ tile ]->sceneBoundingRect() );
	}

	// found the tiles of same row or column
	QMap< qreal, QRectF > x_rect, y_rect;
	for ( int router = 0; router < mp_soc_cfg->network().routerCount();
		router ++ )
	{
		QPointF pos_r = m_router_vector[ router ]->pos();
		if ( x_rect.contains( pos_r.x() ) )
		{
			x_rect[ pos_r.x() ] = x_rect[ pos_r.x() ].united( rect_r[ router ] );
		}
		else
		{
			x_rect.insert( pos_r.x(), rect_r[ router ] );
		}
		if ( y_rect.contains( pos_r.y() ) )
		{
			y_rect[ pos_r.y() ] = y_rect[ pos_r.y() ].united( rect_r[ router ] );
		}
		else
		{
			y_rect.insert( pos_r.y(), rect_r[ router ] );
		}
	}

	// get location of each row and column
	QMap< qreal, qreal > x_to_x, y_to_y;
	qreal x_offset = - x_rect.firstKey() + x_rect.first().left();
	qreal y_offset = - y_rect.firstKey() + y_rect.first().top();
	for ( QMap< qreal, QRectF >::iterator i = x_rect.begin();
		i != x_rect.end(); i ++ )
	{
		x_to_x.insert( i.key(),
			x_offset + i.key() - i.value().left() );
		x_offset += i.value().width();
	}
	for ( QMap< qreal, QRectF >::iterator i = y_rect.begin();
		i != y_rect.end(); i ++ )
	{
		y_to_y.insert( i.key(),
			y_offset + i.key() - i.value().top() );
		y_offset += i.value().height();
	}

	// update location of components
	for ( size_t tile = 0; tile < mp_soc_cfg->tileCount(); tile ++ )
	{
        size_t ni_id = mp_soc_cfg->tile( tile ).niId();
		QPointF pos = m_router_vector[ ni_id ]->pos();
		m_tile_vector[ tile ]->moveBy( x_to_x[ pos.x() ] - pos.x(),
			y_to_y[ pos.y() ] - pos.y() );
		mp_soc_cfg->tile( tile ).setPos( m_tile_vector[ tile ]->pos().x(),
			m_tile_vector[ tile ]->pos().y() );
	}
	for ( int router = 0; router < mp_soc_cfg->network().routerCount();
		router ++ )
	{
		QPointF pos = m_router_vector[ router ]->pos();
		m_router_vector[ router ]->setPos(
			x_to_x[ pos.x() ], y_to_y[ pos.y() ] );
		mp_soc_cfg->network().router( router ).setPos(
			m_router_vector[ router ]->pos().x(),
			m_router_vector[ router ]->pos().y() );
	}
}

#define clearSoCSceneVector( vec ) \
for ( int i = 0; i < vec.size(); i ++ ) \
{ \
	if ( vec[ i ] != 0 ) \
	{ \
		removeItem( vec[ i ] ); \
		delete vec[ i ]; \
	} \
} \
vec.clear();

void SoCScene::clearSoCScene()
{
	clearSoCSceneVector( m_router_vector )
	clearSoCSceneVector( m_link_vector )
	clearSoCSceneVector( m_tile_vector )
	clearSoCSceneVector( m_ni_vector )
}
// ---- add components ---- //

void SoCScene::disableTileSelectable(int tile)
{
	m_tile_vector[ tile ]->setFlag( QGraphicsItem::ItemIsSelectable, false );
	m_tile_vector[ tile ]->setAcceptHoverEvents( false );
}

long long SoCScene::simCycle(int tile)
{
	return m_tile_vector[ tile ]->recordItem().simCycle();
}

void SoCScene::clearSelected()
{
	for ( int i = 0; i < m_router_vector.size(); i ++ )
	{
		m_router_vector[ i ]->setSelected( false );
		m_router_vector[ i ]->update();
	}
	for ( int i = 0; i < m_tile_vector.size(); i ++ )
	{
		m_tile_vector[ i ]->setSelected( false );
		m_tile_vector[ i ]->update();
	}
}

void SoCScene::setSelectRouter( int index )
{
	clearSelected();
	m_router_vector[ index ]->setSelected( true );
	m_router_vector[ index ]->update();

	EsyNetworkCfgRouter router_cfg = m_router_vector[ index ]->routerCfg();
	for( int port = 0; port < router_cfg.portNum(); port ++ )
	{
		if ( router_cfg.port( port ).networkInterface() )
		{
			for ( int tile = 0; tile < m_tile_vector.size(); tile ++ )
			{
				EsySoCCfgTile tile_cfg = m_tile_vector[ tile ]->tileCfg();
				if ( tile_cfg.niId() == index && tile_cfg.niPhy() == port )
				{
					m_tile_vector[ tile ]->setSelected( true );
					m_tile_vector[ tile ]->update();
				}
			}
		}
	}
}

void SoCScene::setSelectTile( int index )
{
	clearSelected();
	m_tile_vector[ index ]->setSelected( true );
	m_tile_vector[ index ]->update();

	EsySoCCfgTile tile_cfg = m_tile_vector[ index ]->tileCfg();
	if ( m_router_vector.size() > tile_cfg.niId() )
	{
		m_router_vector[ tile_cfg.niId() ]->setSelected( true );
		m_router_vector[ tile_cfg.niId() ]->update();
	}
}

int SoCScene::selectedRouter()
{
	for ( int i = 0; i < m_router_vector.size(); i ++ )
	{
		if ( m_router_vector[ i ]->isSelected() )
		{
			return i;
		}
	}
	return -1;
}

int SoCScene::selectedTile()
{
	for ( int i = 0; i < m_tile_vector.size(); i ++ )
	{
		if ( m_tile_vector[ i ]->isSelected() )
		{
			return i;
		}
	}
	return -1;
}

void SoCScene::recordNextHandler(const EsyDataItemSoCRecord &recorditem)
{
#if 0
	qDebug() << recorditem.hop() << recorditem.actualId() << recorditem.simCycle();
#endif
	m_tile_vector[ recorditem.actualId() ]->setRecordItem( recorditem );
}

void SoCScene::recordPrevHandler(const EsyDataItemSoCRecord &recorditem)
{
#if 0
	qDebug() << recorditem.hop() << recorditem.actualId() << recorditem.simCycle();
#endif
	m_tile_vector[ recorditem.actualId() ]->setRecordItem( recorditem );
}

