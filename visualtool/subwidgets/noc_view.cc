#include "noc_view.h"
#include "esy_eventtype.h"
#include "esy_faultcfg.h"

#define DRAW_ROUTER_CONNECT_LENGTH  60

// ---- constructor and destructor ---- //
NoCView::NoCView ( QWidget * parent ) : QGraphicsView( parent )
{
	sx = 250;
	sy = 250;
}

NoCView::~NoCView()
{
}
// ---- constructor and destructor ---- //


// ---- slot functions ---- //
void NoCView::wheelEvent(QWheelEvent * event)
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
		matrix.scale(qPow(qreal(2), (sx - 250)/qreal(50)), qPow(qreal(2), (sx - 250)/qreal(50)));
		matrix.rotate(0);
		setMatrix(matrix);
		event->accept();
	}
	else if (event->modifiers() & Qt::ShiftModifier)
		horizontalScrollBar() ->setSliderPosition( horizontalScrollBar() ->sliderPosition() + event ->delta() );
	else
		QGraphicsView::wheelEvent( event );
}

void NoCView::keyReleaseEvent(QKeyEvent * event)
{
	switch( event ->key() )
	{
		case Qt::Key_Equal:
			wheelEvent( new QWheelEvent( QCursor::pos(), 10, Qt::NoButton, Qt::ControlModifier, Qt::Vertical ) );
			break;
		case Qt::Key_Minus:
			wheelEvent( new QWheelEvent( QCursor::pos(), -10, Qt::NoButton, Qt::ControlModifier, Qt::Vertical ) );
			break;
		default: emit hotKeyEvent( event ); break;
	}
}

void NoCView::mouseReleaseEvent(QMouseEvent * event)
{
	emit selectRouterEvent( event );
}

void NoCScene::UpdateTooltip()
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

void NoCScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_last_pos = mouseEvent->lastPos();
    m_last_scene_pos = mouseEvent->lastScenePos();
    m_last_screen_pos = mouseEvent->lastScreenPos();

    QGraphicsScene::mouseMoveEvent( mouseEvent );
}

// initialize noc scence
void NoCScene::drawNoCScene()
{
    m_router_vector.resize( mp_network_cfg->routerCount() );

    // generate router
    for ( int t_index = 0; t_index < mp_network_cfg->routerCount(); t_index ++ )
    {
        vector< long > t_ax = mp_network_cfg->seq2Coord( t_index );
        int t_ax_x = t_ax[ EsyNetworkCfg::AX_X ];
		int t_ax_y = 0;
		if ( t_ax.size() > 1 )
		{
			t_ax_y = t_ax[ EsyNetworkCfg::AX_Y ];
		}
        // new router
        RouterGraphicsItem *item = new RouterGraphicsItem(
                    mp_network_cfg->router( t_index ), mp_graphics_cfg );
        m_router_vector[ t_index ] = item;
        addItem( item );
        // set parameter
//        if ( mp_network_cfg->router( t_index ).pos().first == 0.0 &&
//             mp_network_cfg->router( t_index ).pos().second == 0.0 )
//        {
            if ( ( mp_network_cfg->topology() ==
                   EsyNetworkCfg::NOC_TOPOLOGY_2DMESH ) ||
                 ( mp_network_cfg->topology() ==
                   EsyNetworkCfg::NOC_TOPOLOGY_2DTORUS ) )
            {
                mp_network_cfg->router( t_index ).setPos(
					DRAW_ROUTER_CONNECT_LENGTH * t_ax_x,
					DRAW_ROUTER_CONNECT_LENGTH * t_ax_y );
            }
            else
            {
                mp_network_cfg->router( t_index ).setPos(
					DRAW_ROUTER_CONNECT_LENGTH * t_index, 0.0 );
            }
//        }
        item->setPos( mp_network_cfg->router( t_index ).pos().first,
                      mp_network_cfg->router( t_index ).pos().second );
    }

	regularScene();

	// connect router
    for ( int t_index = 0; t_index < mp_network_cfg->routerCount(); t_index ++ )
    {
        EsyNetworkCfgRouter t_router_cfg = mp_network_cfg->router( t_index );

        for ( long t_port_index = 0;
            t_port_index < t_router_cfg.portNum(); t_port_index ++ )
        {
            LinkGraphicsItem * t_link = addLink( t_index, t_port_index );

            if ( t_link )
            {
                m_link_vector.append( t_link );
                addItem( t_link );
            }
        }
    }
}

LinkGraphicsItem * NoCScene::addLink( int src_id, int src_port )
{
    if ( src_id < 0 || src_id >= mp_network_cfg->routerCount() )
    {
        return 0;
    }
    if ( src_port < 0 || src_port >= mp_network_cfg->router( src_id ).portNum() )
    {
        return 0;
    }

    EsyNetworkCfgRouter t_router_cfg = mp_network_cfg->router( src_id );

    if ( t_router_cfg.port( src_port ).neighborRouter() < 0 ||
         t_router_cfg.port( src_port ).neighborRouter() >=
         mp_network_cfg->routerCount() )
    {
        return 0;
    }
    if ( t_router_cfg.port( src_port ).neighborPort() < 0 ||
         t_router_cfg.port( src_port ).neighborPort() >=
         mp_network_cfg->router(
             t_router_cfg.port( src_port ).neighborRouter() ).portNum() )
    {
        return 0;
    }

    vector< long > t_ax = mp_network_cfg->seq2Coord( src_id );
    int t_ax_x = t_ax[ EsyNetworkCfg::AX_X ];
    int t_ax_y = t_ax[ EsyNetworkCfg::AX_Y ];

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

    if ( mp_network_cfg->topology() == EsyNetworkCfg::NOC_TOPOLOGY_2DTORUS )
    {
        if ( t_ax_x == 0 && t_router_cfg.port( src_port ).portDirection() ==
             EsyNetworkCfgPort::ROUTER_PORT_WEST )
        {
            t_dst_point = t_src_point + QPointF( -DRAW_ROUTER_CONNECT_LENGTH, 0 );
        }
        else if ( ( t_ax_x ==
            mp_network_cfg->size( EsyNetworkCfg::AX_X ) - 1 ) &&
            t_router_cfg.port( src_port ).portDirection() ==
                EsyNetworkCfgPort::ROUTER_PORT_EAST )
        {
            t_dst_point = t_src_point +
                    QPointF( DRAW_ROUTER_CONNECT_LENGTH, 0 );
        }
        else if ( ( t_ax_y == 0 ) &&
                  t_router_cfg.port( src_port ).portDirection() ==
                  EsyNetworkCfgPort::ROUTER_PORT_NORTH )
        {
            t_dst_point = t_src_point +
                    QPointF( 0, -DRAW_ROUTER_CONNECT_LENGTH );
        }
        else if ( ( t_ax_y ==
                    mp_network_cfg->size( EsyNetworkCfg::AX_Y ) - 1 ) &&
            t_router_cfg.port( src_port ).portDirection() ==
                  EsyNetworkCfgPort::ROUTER_PORT_SOUTH )
        {
            t_dst_point = t_src_point +
                    QPointF( 0, DRAW_ROUTER_CONNECT_LENGTH );
        }
    }

    t_link->setLine( t_src_point.x(), t_src_point.y(),
                   t_dst_point.x(), t_dst_point.y() );

    return t_link;
}

void NoCScene::regularScene()
{
	// found the tiles of same row or column
	QMap< qreal, QRectF > x_rect, y_rect;
	for ( int router = 0; router < mp_network_cfg->routerCount(); router ++ )
	{
		QPointF pos_r = m_router_vector[ router ]->pos();
		if ( x_rect.contains( pos_r.x() ) )
		{
			x_rect[ pos_r.x() ] = x_rect[ pos_r.x() ].united(
				m_router_vector[ router ]->sceneBoundingRect() );
		}
		else
		{
			x_rect.insert( pos_r.x(), m_router_vector[ router ]->sceneBoundingRect() );
		}
		if ( y_rect.contains( pos_r.y() ) )
		{
			y_rect[ pos_r.y() ] = y_rect[ pos_r.y() ].united(
				m_router_vector[ router ]->sceneBoundingRect() );
		}
		else
		{
			y_rect.insert( pos_r.y(), m_router_vector[ router ]->sceneBoundingRect() );
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
		x_offset += i.value().width() + DRAW_ROUTER_CONNECT_LENGTH;
	}
	for ( QMap< qreal, QRectF >::iterator i = y_rect.begin();
		i != y_rect.end(); i ++ )
	{
		y_to_y.insert( i.key(),
			y_offset + i.key() - i.value().top() );
		y_offset += i.value().height() + DRAW_ROUTER_CONNECT_LENGTH;
	}

	// update location of components
	for ( int router = 0; router < mp_network_cfg->routerCount(); router ++ )
	{
		QPointF pos = m_router_vector[ router ]->pos();
		m_router_vector[ router ]->setPos(
			x_to_x[ pos.x() ], y_to_y[ pos.y() ] );
		mp_network_cfg->router( router ).setPos(
			m_router_vector[ router ]->pos().x(),
			m_router_vector[ router ]->pos().y() );
	}
}

void NoCScene::clearNoCScene()
{
    for ( int i = 0; i < m_router_vector.size(); i ++ )
    {
        if ( m_router_vector[ i ] != 0 )
        {
            removeItem( m_router_vector[ i ] );
            delete m_router_vector[ i ];
        }
    }
    m_router_vector.clear();
    for ( int i = 0; i < m_link_vector.size(); i ++ )
    {
        if ( m_link_vector[ i ] != 0 )
        {
            removeItem( m_link_vector[ i ] );
            delete m_link_vector[ i ];
        }
    }
    m_link_vector.clear();
}
// ---- add components ---- //

void NoCScene::setSelectRouterNoCScene( int index )
{
    for ( int i = 0; i < m_router_vector.size(); i ++ )
    {
        m_router_vector[ i ]->setSelected( false );
        m_router_vector[ i ]->update();
    }
    m_router_vector[ index ]->setSelected( true );
    m_router_vector[ index ]->update();
}

int NoCScene::selectedRouter()
{
    for ( int i = 0; i < m_router_vector.size(); i ++ )
    {
        if ( m_router_vector[ i ]->isSelected() )
        {
            return i;
        }
    }
    return 0;
}

void NoCScene::setPathTraceModelItemVector( const QVector< EsyDataItemTraffic > & list )
{
    int max_count = 0;
    for ( int hop = 0; hop < list.size(); hop ++ )
    {
        if ( list[ hop ].traffic() > max_count )
        {
            max_count = list[ hop ].traffic();
        }
    }
    for ( int i = 0; i < m_router_vector.size() ; i ++ )
    {
        m_router_vector[ i ]->clearPath();
        m_router_vector[ i ]->setPathMaxCount( max_count );
    }
    for ( int hop = 0; hop < list.size(); hop ++ )
    {
#if 1
        qDebug() << (int)list[ hop ].addr() <<
            (int)list[ hop ].inputPc() << (int)list[ hop ].inputVc() <<
            (int)list[ hop ].outputPc() << (int)list[ hop ].outputVc() <<
            (int)list[ hop ].traffic();
#endif
        if ( list[ hop ].addr() < 0 ||
             list[ hop ].inputPc() < 0 || list[ hop ].inputVc() < 0 ||
             list[ hop ].outputPc() < 0 || list[ hop ].outputVc() < 0 ||
             list[ hop ].traffic() < 0 )
        {
            continue;
        }
        m_router_vector[ list[ hop ].addr() ]->setPath(
            list[ hop ].inputPc(), list[ hop ].inputVc(),
            list[ hop ].outputPc(), list[ hop ].outputVc(),
            list[ hop ].traffic() );
    }
    pathTraceDeadlockCheck();
    update( sceneRect() );
}

bool NoCScene::eventCheck( const EsyDataItemEventtrace & eventitem, QString & errmsg, bool forward )
{
    if (forward)
    {
        if ( eventitem.type() == ET_FLIT_SWITCH )
        {
            if ( eventitem.src() >= 0 && eventitem.srcPc() >= 0 && eventitem.srcVc() >= 0 )
            {
                if (m_router_vector[eventitem.src()]->
                        inputFlitSize(eventitem.srcPc(), eventitem.srcVc()) <= 0 )
                {
                    errmsg = QString("Router %1 Input Port %2 VC %3 does not have any flit.")
                            .arg((int)eventitem.src()).arg((int)eventitem.srcPc()).arg((int)eventitem.srcVc());
                    return false;
                }
                ShortFlitStruct top_flit = m_router_vector[eventitem.src()]->
                        getInputTopFlit(eventitem.srcPc(), eventitem.srcVc());
                if ( top_flit.flitId() != eventitem.flitId() || top_flit.flitSrc() != eventitem.flitSrc() ||
                     top_flit.flitDst() != eventitem.flitDst() || top_flit.flitType() != eventitem.flitType() )
                {
                    errmsg = QString("Router %9 Input Port %10 VC %11 flit doesn't match.\n"
                                     "The event trace flit: id=%1, src=%2, dst=%3, type=%4\n"
                                     "The stored flit: id=%5, src=%6, dst=%7, type=%8")
                            .arg((int)eventitem.flitId()).arg((int)eventitem.flitSrc())
                            .arg((int)eventitem.flitDst()).arg((int)eventitem.flitType())
                            .arg((int)top_flit.flitId()).arg((int)top_flit.flitSrc())
                            .arg((int)top_flit.flitDst()).arg((int)top_flit.flitType())
                            .arg((int)eventitem.src()).arg((int)eventitem.srcPc()).arg((int)eventitem.srcVc());
                    return false;
                }
            }
        }
        if ( eventitem.type() == ET_FLIT_RECEIVE )
        {
            if ( eventitem.dst() >= 0 && eventitem.dstPc() >= 0 && eventitem.dstVc() >= 0 )
            {
                if (m_router_vector[eventitem.dst()]->
                        inputFlitSize(eventitem.dstPc(), eventitem.dstVc()) >
                    mp_network_cfg->router(eventitem.dst()).port(eventitem.dstPc()).inputBuffer() )
                {
                    errmsg = QString("Router %1 Input Port %2 VC %3 already full.")
                            .arg((int)eventitem.dst()).arg((int)eventitem.dstPc()).arg((int)eventitem.dstVc());
                    return false;
                }
            }
        }
        if ( eventitem.type() == ET_ASSIGN || eventitem.type() == ET_CROUTING || eventitem.type() == ET_ROUTING )
        {
            if ( eventitem.src() >= 0 && eventitem.srcPc() >= 0 && eventitem.srcVc() >= 0 )
            {
                if (m_router_vector[eventitem.src()]->
                        inputFlitSize(eventitem.srcPc(), eventitem.srcVc()) <= 0 )
                {
                    errmsg = QString("Router %1 Input Port %2 VC %3 does not have any flit.")
                            .arg((int)eventitem.src()).arg((int)eventitem.srcPc()).arg((int)eventitem.srcVc());
                    return false;
                }
            }
        }
        return true;
    }
    else
    {
        if ( eventitem.type() == ET_FLIT_SWITCH )
        {
            if ( eventitem.dst() >= 0 && eventitem.dstPc() >= 0 && eventitem.dstVc() >= 0 )
            {
                if (m_router_vector[eventitem.dst()]->
                        outputFlitSize(eventitem.dstPc(), eventitem.dstVc()) <= 0 )
                {
                    errmsg = QString("Router %1 Output Port %2 VC %3 does not have any flit.")
                            .arg((int)eventitem.dst()).arg((int)eventitem.dstPc()).arg((int)eventitem.dstVc());
                    return false;
                }
                ShortFlitStruct top_flit = m_router_vector[eventitem.dst()]->
                        getOutputLastFlit(eventitem.dstPc(), eventitem.dstVc());
                if ( top_flit.flitId() != eventitem.flitId() || top_flit.flitSrc() != eventitem.flitSrc() ||
                     top_flit.flitDst() != eventitem.flitDst() || top_flit.flitType() != eventitem.flitType() )
                {
                    errmsg = QString("Router %9 Input Port %10 VC %11 flit doesn't match.\n"
                                     "The event trace flit: id=%1, src=%2, dst=%3, type=%4\n"
                                     "The stored flit: id=%5, src=%6, dst=%7, type=%8")
                            .arg((int)eventitem.flitId()).arg((int)eventitem.flitSrc())
                            .arg((int)eventitem.flitDst()).arg((int)eventitem.flitType())
                            .arg((int)top_flit.flitId()).arg((int)top_flit.flitSrc())
                            .arg((int)top_flit.flitDst()).arg((int)top_flit.flitType())
                            .arg((int)eventitem.dst()).arg((int)eventitem.dstPc()).arg((int)eventitem.dstVc());
                    return false;
                }
            }
        }
        if ( eventitem.type() == ET_FLIT_SEND )
        {
            if ( eventitem.src() >= 0 && eventitem.srcPc() >= 0 && eventitem.srcVc() >= 0 )
            {
                if (m_router_vector[eventitem.src()]->
                        outputFlitSize(eventitem.srcPc(), eventitem.srcVc()) >
                    mp_network_cfg->router(eventitem.src()).port(eventitem.srcPc()).inputBuffer() )
                {
                    errmsg = QString("Router %1 Output Port %2 VC %3 already full.")
                            .arg((int)eventitem.src()).arg((int)eventitem.srcPc()).arg((int)eventitem.srcVc());
                    return false;
                }
            }
        }
        if ( eventitem.type() == ET_ASSIGN || eventitem.type() == ET_CROUTING || eventitem.type() == ET_ROUTING )
        {
            if ( eventitem.src() >= 0 && eventitem.srcPc() >= 0 && eventitem.srcVc() >= 0 )
            {
                if (m_router_vector[eventitem.src()]->
                        inputFlitSize(eventitem.srcPc(), eventitem.srcVc()) <= 0 )
                {
                    errmsg = QString("Router %1 Input Port %2 VC %3 does not have any flit.")
                            .arg((int)eventitem.src()).arg((int)eventitem.srcPc()).arg((int)eventitem.srcVc());
                    return false;
                }
            }
        }
        return true;
    }
}

void NoCScene::eventNextHandler( const EsyDataItemEventtrace & eventitem )
{
#if 1
    qDebug() << eventitem.time() << (int)eventitem.type() <<
        (int)eventitem.src() <<
        (int)eventitem.srcPc() << (int)eventitem.srcVc() <<
        (int)eventitem.dst() <<
        (int)eventitem.dstPc() << (int)eventitem.dstVc() <<
        eventitem.flitId() <<
        (int)eventitem.flitSize() << (int)eventitem.flitType() <<
        (int)eventitem.flitSrc() << (int)eventitem.flitDst() <<
        eventitem.flitStartTime();
#endif

    int srcrouterseq = eventitem.src();
    int dstrouterseq = eventitem.dst();

    if ( eventitem.type() == ET_FLIT_RECEIVE )
    {
        m_router_vector[ dstrouterseq ]->insertInputPortLast(
            eventitem.dstPc(), eventitem.dstVc(), ShortFlitStruct(
                eventitem.flitId(),
                (ShortFlitStruct::FlitTypeEnum)( eventitem.flitType() ),
                eventitem.flitSrc(), eventitem.flitDst(), eventitem.flitFlag() ) );
    }
    else if ( eventitem.type() == ET_FLIT_SWITCH )
    {
        m_router_vector[ srcrouterseq ]->removeInputPortFirst(
            eventitem.srcPc(), eventitem.srcVc() );
        m_router_vector[ dstrouterseq ]->insertOutputPortLast(
            eventitem.dstPc(), eventitem.dstVc(), ShortFlitStruct(
                eventitem.flitId(),
                (ShortFlitStruct::FlitTypeEnum)( eventitem.flitType() ),
                eventitem.flitSrc(), eventitem.flitDst(), eventitem.flitFlag() ) );
    }
    else if ( eventitem.type() == ET_FLIT_SEND )
    {
        m_router_vector[ srcrouterseq ]->removeOutputPortFirst(
            eventitem.srcPc(), eventitem.srcVc() );
    }
    else if ( eventitem.type() == ET_ROUTING )
    {
        m_router_vector[ dstrouterseq ]->insertRoutingTableLast(
            eventitem.srcPc(), eventitem.srcVc(),
            eventitem.dstPc(), eventitem.dstVc() );
    }
    else if ( eventitem.type() == ET_ROUTING_END )
    {
        m_router_vector[ dstrouterseq ]->removeRoutingTableFirst(
            eventitem.srcPc(), eventitem.srcVc() );
    }
    else if ( eventitem.type() == ET_ASSIGN )
    {
        m_router_vector[ dstrouterseq ]->insertAssignTable(
            eventitem.srcPc(), eventitem.srcVc(),
            eventitem.dstPc(), eventitem.dstVc() );
    }
    else if ( eventitem.type() == ET_ASSIGN_END )
    {
        m_router_vector[ dstrouterseq ]->removeAssignTable(
            eventitem.srcPc(), eventitem.srcVc() );
    }
    else if ( eventitem.type() == ET_VC_STATE )
    {
        m_router_vector[ dstrouterseq ]->setInputVCState(
            eventitem.srcPc(), eventitem.srcVc(), eventitem.flitDst() );
    }
    else if ( eventitem.type() == ET_FT_INJECTION ||
              eventitem.type() == ET_FT_RECOVERY )
    {
        bool nextFaulty = (eventitem.type() == ET_FT_INJECTION);
        switch ( eventitem.faultyType() )
        {
        case EsyFaultConfigItem::FAULT_ROUTER:
            m_router_vector[ eventitem.faultyRouter() ]->setFaulty( nextFaulty );
            break;
        case EsyFaultConfigItem::FAULT_PORT:
            m_router_vector[ eventitem.faultyRouter() ]->setInputPortFaulty(
                eventitem.faultyPort(), eventitem.faultyVC(), nextFaulty );
            for (int l = 0; l < m_link_vector.size(); l ++ )
            {
                if ( m_link_vector[ l ]->desId() == eventitem.faultyRouter() &&
                     m_link_vector[ l ]->desPort() == eventitem.faultyPort() )
                {
                    m_link_vector[ l ]->setFaulty( nextFaulty );
                    m_router_vector[ m_link_vector[ l ]->srcId() ]->
                        setOutputPortFaulty( m_link_vector[ l ]->srcPort(),
                        -1, nextFaulty );
                }
            }
            break;
        case EsyFaultConfigItem::FAULT_LINE:
            {
                for (int l = 0; l < m_link_vector.size(); l ++ )
                {
                    if ( m_link_vector[ l ]->desId() == eventitem.faultyRouter() &&
                         m_link_vector[ l ]->desPort() == eventitem.faultyPort() )
                    {
                        m_link_vector[ l ]->setFaulty( nextFaulty );
                    }
                }
            }
            break;
        case EsyFaultConfigItem::FAULT_SWITCH:
            m_router_vector[ eventitem.faultyRouter() ]->setOutputPortFaulty(
                eventitem.faultyPort(), eventitem.faultyVC(), nextFaulty );
            break;
        }
    }
    else if(eventitem.type() == ET_PE_CHANGE)
    {
        int pe_id = eventitem.src();
        m_router_vector[pe_id]->setTaskState(
                    (ProcessorItem::ProcessorStatus)eventitem.srcVc(),
                    eventitem.dst(), eventitem.dstPc() );
    }
}

void NoCScene::eventPrevHandler( const EsyDataItemEventtrace & eventitem )
{
#if 1
    qDebug() << eventitem.time() << (int)eventitem.type() <<
        (int)eventitem.src() <<
        (int)eventitem.srcPc() << (int)eventitem.srcVc() <<
        (int)eventitem.dst() <<
        (int)eventitem.dstPc() << (int)eventitem.dstVc() <<
        eventitem.flitId() <<
        (int)eventitem.flitSize() << (int)eventitem.flitType() <<
        (int)eventitem.flitSrc() << (int)eventitem.flitDst() <<
        eventitem.flitStartTime();
#endif

    int srcrouterseq = eventitem.src();
    int dstrouterseq = eventitem.dst();

    if ( eventitem.type() == ET_FLIT_RECEIVE )
    {
        m_router_vector[ dstrouterseq ]->removeInputPortLast(
            eventitem.dstPc(), eventitem.dstVc() );
    }
    else if ( eventitem.type() == ET_FLIT_SWITCH )
    {
        m_router_vector[ srcrouterseq ]->insertInputPortFirst(
            eventitem.srcPc(), eventitem.srcVc(), ShortFlitStruct(
                eventitem.flitId(),
                (ShortFlitStruct::FlitTypeEnum)( eventitem.flitType() ),
                eventitem.flitSrc(), eventitem.flitDst(), eventitem.flitFlag() ) );
        m_router_vector[ dstrouterseq ]->removeOutputPortLast(
            eventitem.dstPc(), eventitem.dstVc() );
    }
    else if ( eventitem.type() == ET_FLIT_SEND )
    {
        m_router_vector[ srcrouterseq ]->insertOutputPortFirst(
            eventitem.srcPc(), eventitem.srcVc(), ShortFlitStruct(
                eventitem.flitId(),
                (ShortFlitStruct::FlitTypeEnum)( eventitem.flitType() ),
                eventitem.flitSrc(), eventitem.flitDst(), eventitem.flitFlag() ) );
    }
    else if ( eventitem.type() == ET_ROUTING )
    {
        m_router_vector[ dstrouterseq ]->removeRoutingTableLast(
            eventitem.srcPc(), eventitem.srcVc() );
    }
    else if ( eventitem.type() == ET_ROUTING_END )
    {
        m_router_vector[ dstrouterseq ]->insertRoutingTableFirst(
            eventitem.srcPc(), eventitem.srcVc(),
            eventitem.dstPc(), eventitem.dstVc() );
    }
    else if ( eventitem.type() == ET_ASSIGN )
    {
        m_router_vector[ dstrouterseq ]->removeAssignTable(
            eventitem.srcPc(), eventitem.srcVc() );
    }
    else if ( eventitem.type() == ET_ASSIGN_END )
    {
        m_router_vector[ dstrouterseq ]->insertAssignTable(
            eventitem.srcPc(), eventitem.srcVc(),
            eventitem.dstPc(), eventitem.dstVc() );
    }
    else if ( eventitem.type() == ET_VC_STATE )
    {
        m_router_vector[ dstrouterseq ]->setInputVCState(
            eventitem.srcPc(), eventitem.srcVc(), eventitem.flitSrc() );
    }
    else if ( eventitem.type() == ET_FT_INJECTION ||
              eventitem.type() == ET_FT_RECOVERY )
    {
        bool prevFaulty = (eventitem.type() == ET_FT_RECOVERY);
        switch ( eventitem.faultyType() )
        {
        case EsyFaultConfigItem::FAULT_ROUTER:
            m_router_vector[ eventitem.faultyRouter() ]->setFaulty( prevFaulty );
            break;
        case EsyFaultConfigItem::FAULT_PORT:
            m_router_vector[ eventitem.faultyRouter() ]->setInputPortFaulty(
                eventitem.faultyPort(), eventitem.faultyVC(), prevFaulty );
        case EsyFaultConfigItem::FAULT_LINE:
            {
                for (int l = 0; l < m_link_vector.size(); l ++ )
                {
                    if ( m_link_vector[ l ]->desId() == eventitem.faultyRouter() &&
                         m_link_vector[ l ]->desPort() == eventitem.faultyPort() )
                    {
                        m_link_vector[ l ]->setFaulty( prevFaulty );
                    }
                }
            }
        }
    }
    else if ( eventitem.type() == ET_PE_CHANGE )
    {
        int pe_id = eventitem.src();
        m_router_vector[ pe_id ] ->setTaskState(
                    (ProcessorItem::ProcessorStatus)eventitem.srcPc(),
                    eventitem.dst(), eventitem.dstPc() );
    }
}

void NoCScene::pathHandler( const EsyDataItemPathtrace & eventitem )
{
    for ( int i = 0; i < m_router_vector.size(); i ++ )
    {
        m_router_vector[ i ]->clearPath();
        m_router_vector[ i ]->setPathMaxCount( -1 );
    }
    for ( int i = 0; i < m_link_vector.size(); i ++ )
    {
        m_link_vector[ i ]->setPath( false );
    }
    vector< EsyDataItemHop > pathhop = eventitem.pathhoplist();
    for ( size_t hop = 0; hop < pathhop.size(); hop ++ )
    {
#if 1
        qDebug() << (int)pathhop[ hop ].addr() <<
            (int)pathhop[ hop ].inputPc() << (int)pathhop[ hop ].inputVc() <<
            (int)pathhop[ hop ].outputPc() << (int)pathhop[ hop ].outputVc();
#endif
        m_router_vector[ pathhop[ hop ].addr() ]->setPath(
            pathhop[ hop ].inputPc(), pathhop[ hop ].inputVc(),
            pathhop[ hop ].outputPc(), pathhop[ hop ].outputVc() );
        for ( int j = 0; j < m_link_vector.size(); j ++ )
        {
            if ( ( m_link_vector[ j ]->srcId() == (int)pathhop[ hop ].addr() ) &&
                 ( m_link_vector[ j ]->srcPort() ==
                     (int)pathhop[ hop ].outputPc() ) )
            {
                 m_link_vector[ j ]->setPath( true );
            }
        }
    }
}

void NoCScene::eventTraceDeadlockCheck()
{
    QVector< QVector< QVector< QVector< QPair< int, int > > > > > m_turn_table;

    m_turn_table.resize( mp_network_cfg->routerCount() );
    for ( int id = 0; id < mp_network_cfg->routerCount(); id ++ )
    {
        m_router_vector[ id ]->incPortTimer();
        m_turn_table[ id ].resize( mp_network_cfg->router( id ).portNum() );
        for ( int port = 0; port < mp_network_cfg->router( id ).portNum(); port ++ )
        {
			m_turn_table[ id ][ port ].resize( mp_network_cfg->router( id ).port( port ).inputVcNumber() );
			for ( int vc = 0; vc < mp_network_cfg->router( id ).port( port ).inputVcNumber(); vc ++ )
            {
                if ( m_router_vector[ id ]->inputFlitSize( port, vc ) >=
                     mp_network_cfg->router( id ).port( port ).inputBuffer() - 2 )
                {
                    m_turn_table[ id ][ port ][ vc ] = m_router_vector[ id ]->turnVector( port, vc );
                }
            }
        }
    }

    QVector< EsyDataItemHop > deadlock_line = graphicsDeadlockCheck( m_turn_table );
    for ( int i = 0; i < deadlock_line.size(); i ++ )
    {
        m_router_vector[ deadlock_line[ i ].addr() ]->setInputPortFaulty(
            deadlock_line[ i ].inputPc(), deadlock_line[ i ].inputVc(), true );
    }
}

void NoCScene::pathTraceDeadlockCheck()
{
    QVector< QVector< QVector< QVector< QPair< int, int > > > > > m_turn_table;

    m_turn_table.resize( mp_network_cfg->routerCount() );
    for ( int id = 0; id < mp_network_cfg->routerCount(); id ++ )
    {
        m_turn_table[ id ].resize( mp_network_cfg->router( id ).portNum() );
        for ( int port = 0; port < mp_network_cfg->router( id ).portNum(); port ++ )
        {
			m_turn_table[ id ][ port ].resize( mp_network_cfg->router( id ).port( port ).inputVcNumber() );
        }

        QVector< EsyDataItemTraffic > path_list = m_router_vector[ id ]->pathPairList();
        for ( int path = 0; path < path_list.size(); path ++ )
        {
            m_turn_table[ id ][ path_list[path].inputPc() ][ path_list[path].inputVc() ].append(
                 QPair< int, int >( path_list[path].outputPc(), path_list[path].outputVc() ) );
        }
    }

    QVector< EsyDataItemHop > deadlock_line = graphicsDeadlockCheck( m_turn_table );
    for ( int i = 0; i < deadlock_line.size(); i ++ )
    {
        m_router_vector[ deadlock_line[ i ].addr() ]->setPathFaulty(
            deadlock_line[i].inputPc(), deadlock_line[i].outputVc(),
            deadlock_line[i].outputPc(), deadlock_line[i].outputVc(), true);
    }
}

QVector< EsyDataItemHop > NoCScene::graphicsDeadlockCheck(
    QVector< QVector< QVector< QVector< QPair< int, int > > > > > graphics )
{
    QVector< EsyDataItemHop > deadlock_list;
    for ( long l_id = 0; l_id < mp_network_cfg->routerCount(); l_id ++ )
    {
        for ( long l_port = 0; l_port < mp_network_cfg->router( l_id ).portNum(); l_port ++ )
        {
			for ( long l_vc = 0; l_vc < mp_network_cfg->router( l_id ).port( l_port ).inputVcNumber(); l_vc ++ )
            {
                while ( graphics[ l_id ][ l_port ][ l_vc ].size() > 0 )
                {
                    /* start point */
                    QVector< EsyDataItemHop > path_line;
                    QPair< int, int > t_turn = graphics[ l_id ][ l_port ][ l_vc ].first();
                    graphics[ l_id ][ l_port ][ l_vc ].removeFirst();
                    path_line.append( EsyDataItemHop(
                        l_id, l_port, l_vc, t_turn.first, t_turn.second ) );

                    /* next port */
                    long t_id = mp_network_cfg->router( l_id ).port( t_turn.first ).neighborRouter();
                    long t_port = mp_network_cfg->router( l_id ).port( t_turn.first ).neighborPort();
                    long t_vc = t_turn.second;

                    /* local return check */
                    if ( mp_network_cfg->router( t_id ).port( t_port ).networkInterface() )
                    {
                        continue;
                    }

                    while ( path_line.size() > 0 )
                    {
                        /* next step exist */
                        if ( graphics[ t_id ][ t_port ][ t_vc ].size() > 0 )
                        {
                            QPair< int, int > t_turn = graphics[ t_id ][ t_port ][ t_vc ].first();
                            graphics[ t_id ][ t_port ][ t_vc ].removeFirst();

                            /* ni check */
                            if ( mp_network_cfg->router( t_id ).port( t_turn.first ).networkInterface() )
                            {
                                continue;
                            }

                            /* deadlock check */
                            bool deadlock_flag = false;
                            int c_id = mp_network_cfg->router( t_id ).port( t_turn.first ).neighborRouter();
                            int c_port = mp_network_cfg->router( t_id ).port( t_turn.first ).neighborPort();
                            int c_vc = t_turn.second;

                            for ( int i = 0; i < path_line.size(); i ++ )
                            {
                                if ( path_line[ i ].addr() == c_id &&
                                     path_line[ i ].inputPc() == c_port &&
                                     path_line[ i ].inputVc() == c_vc )
                                {
                                    deadlock_flag = true;
                                    for ( ; i < path_line.size(); i ++ )
                                    {
                                        deadlock_list.append( path_line[ i ] );
                                    }
                                    deadlock_list.append( EsyDataItemHop( t_id, t_port, t_vc, t_turn.first, t_turn.second ) );
                                    break;
                                }
                            }
                            if ( deadlock_flag )
                            {
                                continue;
                            }

                            /* add new hop */
                            EsyDataItemHop t_hop( t_id, t_port, t_vc, t_turn.first, t_turn.second );
                            path_line.append( t_hop );
                            /* next port */
                            t_port = mp_network_cfg->router( t_id ).port( t_turn.first ).neighborPort();
                            t_id = mp_network_cfg->router( t_id ).port( t_turn.first ).neighborRouter();
                            t_vc = t_turn.second;
                        }
                        /* next step is not exist, go back one step */
                        else
                        {
                            path_line.removeLast();
                            if (path_line.size() > 0)
                            {
                                EsyDataItemHop t_hop = path_line.last();
                                /* next port */
                                t_id = mp_network_cfg->router( t_hop.addr() ).port( t_hop.outputPc() ).neighborRouter();
                                t_port = mp_network_cfg->router( t_hop.addr() ).port( t_hop.outputPc() ).neighborPort();
                                t_vc = t_hop.outputVc();
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return deadlock_list;
}
