#include "port_graphics_item.h"

QStringList ShortFlitStruct::const_flit_type_string = QStringList() <<
    QString( "Head" ) << QString( "Body" ) << QString( "Tail" );

QStringList PortGraphicsItem::const_state_string_list = QStringList() <<
    QString( "INIT" ) << QString( "ROUTING" ) << QString( "VC_AB" ) <<
    QString( "SW_AB" ) << QString( "SW_TR" ) << QString( "HOME" );

// ---- constructor and destructor ---- //
PortGraphicsItem::PortGraphicsItem(
    qreal width, qreal height, int buffer_size,
    GraphicsConfigModel * graphics_cfg, QGraphicsItem * parent
) :
    QGraphicsItem( parent ), m_graphics_cfg( graphics_cfg ),
    m_buffer_rect( 0, - width / 2, height, width ), m_buffer_size( buffer_size ),
    m_timer( 0 ), m_traffic( 0 ), m_max_traffic( 0 ),
    m_faulty_flag( false ), m_path_flag( false )
{
    m_flit_width = m_buffer_rect.width() / m_buffer_size;
}

// ---- constructor and destructor ---- //

// ---- draw function ---- //
QRectF PortGraphicsItem::boundingRect() const
{
    return m_buffer_rect;
}

// paint function
void PortGraphicsItem::paint( QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( widget );
    Q_UNUSED( option );

	// draw buffer
    GraphicsConfigModelItem * port_graphics_item;
    if ( m_flit_list.size() >= m_buffer_size || m_faulty_flag )
    {
        port_graphics_item = m_graphics_cfg->configItem( "Port", "Faulty" );
    }
    else
    {
        port_graphics_item = m_graphics_cfg->configItem( "Port", "Normal" );
    }
    painter->setPen( port_graphics_item->linePen() );
    painter->setBrush( port_graphics_item->fillBrush() );
    painter->drawRect( m_buffer_rect );

	// draw flit
    QPointF flitpoint = m_buffer_rect.topLeft();
    for ( int i = 0; i < m_flit_list.size(); i ++ )
	{
        if ( i == m_buffer_size )
        {
			break;
        }
		
		// draw flit
        QRect flitrect( flitpoint.x(), flitpoint.y(),
                        m_flit_width, m_buffer_rect.height() );
        // if
        GraphicsConfigModelItem * flit_graphics_item;
        if ( m_flit_list[ i ].flitFlag() & ShortFlitStruct::FLIT_DROP )
        {
            flit_graphics_item = m_graphics_cfg->configItem( "Flit", "Drop" );
        }
        else if ( m_flit_list[ i ].flitFlag() & ShortFlitStruct::FLIT_ACK )
        {
            flit_graphics_item = m_graphics_cfg->configItem( "Flit", "Ack" );
        }
        else if ( m_flit_list[ i ].flitType() == ShortFlitStruct::FLIT_HEAD )
        {
            flit_graphics_item = m_graphics_cfg->configItem( "Flit", "Head" );
        }
        else if ( m_flit_list[ i ].flitType() == ShortFlitStruct::FLIT_TAIL )
        {
            flit_graphics_item = m_graphics_cfg->configItem( "Flit", "Tail" );
        }
        else
        {
            flit_graphics_item = m_graphics_cfg->configItem( "Flit", "Body" );
        }
        painter->setPen( flit_graphics_item->linePen() );
        painter->setBrush( flit_graphics_item->fillBrush() );
        painter->drawRect( flitrect );

		// add point
        flitpoint += QPointF( m_flit_width, 0 );
	}

    if ( m_path_flag )
    {
        QPen t_pen = m_graphics_cfg->configItem( "Path", "Normal" )->linePen();
        if ( m_max_traffic > 0 )
        {
            t_pen.setWidthF( t_pen.widthF() / m_max_traffic * m_traffic );
        }
        painter->setPen( t_pen );
        painter->drawLine( 0, 0, m_buffer_rect.width(), 0 );
    }
	
    painter->setPen( port_graphics_item->linePen() );
    painter->setBrush( Qt::NoBrush  );

    QFont coordfont = painter->font();
    coordfont.setPointSize( m_buffer_rect.height() * 0.5 );
    painter->setFont( coordfont );
    if ( m_path_flag )
    {
        if ( m_max_traffic > 0 )
        {
            painter->drawText( boundingRect(), Qt::AlignCenter,
                               QString::number( m_traffic ) );
        }
    }
    else
    {
        painter->drawText( boundingRect(), Qt::AlignCenter, m_state_str );
    }

    QString buffer_tool_tip = QString( "Status : %1\nBuffer : %2/%3" ).
        arg( m_state_str ).arg( m_flit_list.size() ).arg( m_buffer_size );
    for ( int i = 0; i < m_flit_list.size(); i ++ )
    {
        if ( i >= m_buffer_size )
        {
            buffer_tool_tip += QString( "\n%1 more flits" )
                .arg( m_flit_list.size() - m_buffer_size );
            break;
        }
        else
        {
            buffer_tool_tip += QString( "\n%1.Flit %2:%3->%4,%5" ).arg( i )
                .arg( m_flit_list[ i ].flitId() )
                .arg( m_flit_list[ i ].flitSrc() )
                .arg( m_flit_list[ i ].flitDst() )
                .arg( ShortFlitStruct::const_flit_type_string[ m_flit_list[ i ].
                    flitType() ] );
        }
    }
    setToolTip( buffer_tool_tip );
}
// ---- draw function ---- //

// ---- NIGraphicsItem ---- //
// ---- constructor and destuctor -- //
NIGraphicsItem::NIGraphicsItem(
    qreal width, qreal height, qreal length,
    GraphicsConfigModel * graphics_cfg, QGraphicsItem * parent
):
    QGraphicsItem( parent ), m_ni_rect( 0, - width / 2, height + length, width ),
    m_width( width ), m_height( height ), m_length( length ),
    m_graphics_cfg( graphics_cfg )
{
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

// draw noc router
QRectF NIGraphicsItem::boundingRect() const
{
    return m_ni_rect;
}

void NIGraphicsItem::paint( QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED( widget );
    Q_UNUSED( option );

    painter->setPen( m_graphics_cfg->configItem( "NI", "Normal" )->linePen() );
    painter->setBrush( m_graphics_cfg->configItem( "NI", "Normal" )->fillBrush() );

    painter->drawLine( QPointF( -m_width / 4, 0 ),
                       QPointF( -m_width / 4, m_length ) );
    painter->drawLine( QPointF(  m_width / 4, 0 ),
                       QPointF(  m_width / 4, m_length ) );

    painter->drawLine( QPointF( -m_width / 2, m_length ),
        QPointF( -m_width / 2, m_length + m_height ) );
    painter->drawLine( QPointF(  m_width / 2, m_length ),
        QPointF(  m_width / 2, m_length + m_height ) );
    painter->drawLine( QPointF(  m_width / 2, m_length ),
        QPointF( -m_width / 2, m_length ) );

    QFont coordfont = painter->font();
    coordfont.setPointSize( m_height * 0.5 );
    painter->setFont( coordfont );
    painter->drawText( QRectF( -m_width / 2, m_length, m_width, m_height ),
        Qt::AlignCenter, QString( "NI" ) );
}

void LinkGraphicsItem::paint(QPainter *painter,
    const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if ( m_path_flag )
    {
        setPen(
            m_graphics_cfg->configItem( "Path", "Normal" )->linePen() );
    }
    else
    {
        setPen( m_graphics_cfg->configItem( "Link",
            m_faulty_flag?("Faulty"):("Normal") )->linePen() );
    }

    QGraphicsLineItem::paint( painter, option, widget );
}
