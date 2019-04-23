#include "tile_graphics_item.h"

#define DRAW_COMP_HEIGHT   25.0
#define DRAW_COMP_WIDTH   150.0
#define DRAW_LIGHT_RADIUS   4.0
#define DRAW_VC_WIDTH    10
#define DRAW_SIN45D   0.7071
#define DRAW_COS45D   0.7071

const QStringList TileGraphicsItem::m_comp_text_list = QStringList() <<
	"Tile" << "IAdder" << "FAdder" << "IMulti" << "FMulti" <<
	"Regs" << "IL1" << "DL1" << "IL2" << "DL2" << "Mem" << "NI";
const QList< QPointF > TileGraphicsItem::m_comp_rect_list = QList< QPointF >() <<
	QPointF( DRAW_COMP_WIDTH,         DRAW_COMP_HEIGHT ) << // TileID
	QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT ) << // IADDER
	QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT ) << // FADDER
	QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT ) << // IMULTI
	QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT ) << // FMULTI
	QPointF( DRAW_COMP_WIDTH / 3,     DRAW_COMP_HEIGHT * 4 ) << // REG
	QPointF( DRAW_COMP_WIDTH / 2,     DRAW_COMP_HEIGHT ) << // IL1
	QPointF( DRAW_COMP_WIDTH / 2,     DRAW_COMP_HEIGHT ) << // DL1
	QPointF( DRAW_COMP_WIDTH / 2,     DRAW_COMP_HEIGHT ) << // IL2
	QPointF( DRAW_COMP_WIDTH / 2,     DRAW_COMP_HEIGHT ) << // DL2
	QPointF( DRAW_COMP_WIDTH * 3 / 5, DRAW_COMP_HEIGHT ) << // Mem
	QPointF( DRAW_COMP_WIDTH / 4,     DRAW_COMP_HEIGHT ); // NI

const QList< QPointF > TileGraphicsItem::m_comp_init_pos_list = QList< QPointF >() <<
	QPointF( 0.0,                     0.0 ) << // TileID
	QPointF( 0.0,                     DRAW_COMP_HEIGHT ) << // IADDER
	QPointF( 0.0,                     DRAW_COMP_HEIGHT * 2 ) << // FADDER
	QPointF( 0.0,                     DRAW_COMP_HEIGHT * 3 ) << // IMULTI
	QPointF( 0.0,                     DRAW_COMP_HEIGHT * 4 ) << // FMULTI
	QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT ) << // REG
	QPointF( 0.0,                     DRAW_COMP_HEIGHT * 5 ) << // IL1
	QPointF( DRAW_COMP_WIDTH / 2,     DRAW_COMP_HEIGHT * 5 ) << // DL1
	QPointF( 0.0,                     DRAW_COMP_HEIGHT * 6 ) << // IL2
	QPointF( DRAW_COMP_WIDTH / 2,     DRAW_COMP_HEIGHT * 6 ) << // DL2
	QPointF( 0.0,                     DRAW_COMP_HEIGHT * 7 ) << // Mem
	QPointF( DRAW_COMP_WIDTH * 3 / 5, DRAW_COMP_HEIGHT * 7 ); // NI

#define DRAW_TILE_LINES QVector< QPointF >() << \
	QPointF( 0, DRAW_COMP_HEIGHT ) << \
		QPointF( DRAW_COMP_WIDTH, DRAW_COMP_HEIGHT ) << \
	QPointF( 0, DRAW_COMP_HEIGHT * 2 ) << \
		QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT * 2 ) << \
	QPointF( 0, DRAW_COMP_HEIGHT * 3 ) << \
		QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT * 3 ) << \
	QPointF( 0, DRAW_COMP_HEIGHT * 4 ) << \
		QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT * 4 ) << \
	QPointF( 0, DRAW_COMP_HEIGHT * 5 ) << \
		QPointF( DRAW_COMP_WIDTH, DRAW_COMP_HEIGHT * 5 ) << \
	QPointF( 0, DRAW_COMP_HEIGHT * 6 ) << \
		QPointF( DRAW_COMP_WIDTH, DRAW_COMP_HEIGHT * 6 ) << \
	QPointF( 0, DRAW_COMP_HEIGHT * 7 ) << \
		QPointF( DRAW_COMP_WIDTH, DRAW_COMP_HEIGHT * 7 ) << \
	QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT ) << \
		QPointF( DRAW_COMP_WIDTH / 3 * 2, DRAW_COMP_HEIGHT * 5 ) << \
	QPointF( DRAW_COMP_WIDTH / 2, DRAW_COMP_HEIGHT * 5 ) << \
		QPointF( DRAW_COMP_WIDTH / 2, DRAW_COMP_HEIGHT * 7 ) << \
	QPointF( DRAW_COMP_WIDTH / 5 * 3, DRAW_COMP_HEIGHT * 7 ) << \
		QPointF( DRAW_COMP_WIDTH / 5 * 3, DRAW_COMP_HEIGHT * 8 )

// ---- constructor and destructor ---- //
TileCompGraphicsItem::TileCompGraphicsItem(
	qreal width, qreal height, qreal fontsize,
	const QString & text, qreal radius, int lightnum,
	GraphicsConfigModel * graphics_cfg, QGraphicsItem * parent
) :
	QGraphicsItem( parent ), mp_graphics_cfg( graphics_cfg ),
	m_rect( -width / 2, -height / 2, width, height ),
	m_radius( radius ), m_fontsize( fontsize ), m_text( text ),
	m_light_num( lightnum ),
	m_hit_num( 0 ), m_miss_num( 0 ), m_offset( 0 )
{
}

// ---- constructor and destructor ---- //

void TileCompGraphicsItem::setLightNumber(int hit, int miss, int offset)
{
	m_hit_num = hit;
	m_miss_num = miss;
	m_offset = offset;

	if ( m_hit_num + m_miss_num + m_offset > m_light_num )
	{
		m_light_num = m_hit_num + m_miss_num + m_offset;
	}
}

// ---- draw function ---- //
QRectF TileCompGraphicsItem::boundingRect() const
{
	return m_rect;
}

// paint function
void TileCompGraphicsItem::paint( QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	Q_UNUSED( widget );
	Q_UNUSED( option );

	QFont textfont = painter->font();
	textfont.setPointSize( m_fontsize );
	painter->setFont( textfont );
	painter->drawText( m_rect, Qt::AlignHCenter | Qt::AlignBottom, m_text );

	int light = 0;
	qreal light_x = - m_rect.width() / 2;
	qreal light_y = - m_rect.height() / 2;
	for ( light = 0 ; light < m_light_num; light ++ )
	{
		if ( light == 0 )
		{
			painter->setBrush(
				mp_graphics_cfg->configItem( "Light", "Empty" )->fillBrush() );
		}
		if ( light == m_offset )
		{
			painter->setBrush(
				mp_graphics_cfg->configItem( "Light", "Hit" )->fillBrush() );
		}
		if ( light == m_offset + m_hit_num )
		{
			painter->setBrush(
				mp_graphics_cfg->configItem( "Light", "Miss" )->fillBrush() );
		}
		if ( light == m_offset + m_hit_num + m_miss_num )
		{
			painter->setBrush(
				mp_graphics_cfg->configItem( "Light", "Empty" )->fillBrush() );
		}
		painter->drawRect( light_x, light_y, 2*m_radius, 2*m_radius );
		light_x += 2 * m_radius;
	}
}
// ---- draw function ---- //

qreal TileGraphicsItem::ni_angle[ EsySoCCfgTile::NIPOS_COUNT ] =
{
	 45, // PORTDIR_NORTHWEST,
	135, // PORTDIR_NORTHEAST,
	315, // PORTDIR_SOUTHWEST,
	225  // PORTDIR_SOUTHEAST,
};

TileGraphicsItem::TileGraphicsItem( const EsySoCCfgTile & tile_cfg,
	GraphicsConfigModel * graphics_cfg
) :
	m_tile_cfg( tile_cfg ), mp_graphics_cfg( graphics_cfg )
{
	m_tile_rect.setWidth( DRAW_COMP_WIDTH );
	m_tile_rect.setHeight( 8 * DRAW_COMP_HEIGHT );

	for ( int i = 0; i < TILECOMP_COUNT; i ++ )
	{
		QString t_text;
		if ( i == TILECOMP_TILEID )
		{
			t_text = m_comp_text_list[ i ] +
				QString( " %1" ).arg( tile_cfg.tileId() );
		}
		else
		{
			t_text = m_comp_text_list[ i ];
		}

		int t_light_num = 0;
		switch ( i )
		{
		case TILECOMP_IADDER: t_light_num = tile_cfg.integarAdderUnit(); break;
		case TILECOMP_FADDER: t_light_num = tile_cfg.floatAddrUnit(); break;
		case TILECOMP_IMULTI: t_light_num = tile_cfg.integarMultiUnit(); break;
		case TILECOMP_FMULTI: t_light_num = tile_cfg.floatMultiUnit(); break;
		case TILECOMP_IL1:
		case TILECOMP_DL1:
		case TILECOMP_IL2:
		case TILECOMP_DL2: t_light_num = tile_cfg.ruuIfqSize(); break;
		case TILECOMP_MEMORY:
		case TILECOMP_NI: t_light_num = 1; break;
		case TILECOMP_TILEID:
		case TILECOMP_REGS:
		default: t_light_num = 0; break;
		}

		TileCompGraphicsItem * t_item = new TileCompGraphicsItem(
			m_comp_rect_list[ i ].x(), m_comp_rect_list[ i ].y(),
			DRAW_COMP_HEIGHT / 2, t_text, DRAW_LIGHT_RADIUS,
			t_light_num, mp_graphics_cfg, this );
		m_comp_list.append( t_item );
	}

	m_input_port = new PortGraphicsItem(
		DRAW_VC_WIDTH, mp_graphics_cfg, this );
	m_output_port = new PortGraphicsItem(
		DRAW_VC_WIDTH, mp_graphics_cfg, this );

	setFlags( ItemIsSelectable );
	setAcceptHoverEvents( true );
}

QRectF TileGraphicsItem::boundingRect() const
{
	return QRectF( - m_tile_rect.width() / 2, - m_tile_rect.height() / 2,
				  m_tile_rect.width(), m_tile_rect.height() );
}

void TileGraphicsItem::setNiPos( const QPointF & pos )
{
	switch( m_tile_cfg.niPos() )
	{
	case EsySoCCfgTile::NIPOS_NORTHEAST: setPos( pos +
		QPointF( -m_tile_rect.width() / 2, m_tile_rect.height() / 2 ) ); break;
	case EsySoCCfgTile::NIPOS_NORTHWEST: setPos( pos +
		QPointF( m_tile_rect.width() / 2, m_tile_rect.height() / 2 ) ); break;
	case EsySoCCfgTile::NIPOS_SOUTHEAST: setPos( pos +
		QPointF( -m_tile_rect.width() / 2, -m_tile_rect.height() / 2 ) ); break;
	case EsySoCCfgTile::NIPOS_SOUTHWEST:
	default: setPos( pos +
		QPointF( m_tile_rect.width() / 2, -m_tile_rect.height() / 2 ) ); break;
	}
}

QRectF TileGraphicsItem::rectTranspose( const QRectF & rect )
{
	QRectF t( rect.topLeft() -
		QPointF( m_tile_rect.width() / 2, m_tile_rect.height() / 2 ), rect.size() );
	switch ( m_tile_cfg.niPos() )
	{
	case EsySoCCfgTile::NIPOS_NORTHWEST:
		return QRectF( - t.right(), -t.bottom(), t.width(), t.height() );
	case EsySoCCfgTile::NIPOS_NORTHEAST:
		return QRectF( t.left(), -t.bottom(), t.width(), t.height() );
	case EsySoCCfgTile::NIPOS_SOUTHWEST:
		return QRectF( - t.right(), t.top(), t.width(), t.height() );
	case EsySoCCfgTile::NIPOS_SOUTHEAST:
		return QRectF( t.left(), t.top(), t.width(), t.height() );
	default:
		return t;
	}
}

QPointF TileGraphicsItem::pointTranspose( const QPointF & point )
{
	QPointF t = point -
		QPointF( m_tile_rect.width() / 2, m_tile_rect.height() / 2 );
	switch ( m_tile_cfg.niPos() )
	{
	case EsySoCCfgTile::NIPOS_NORTHWEST:
		return QPointF( - t.x(), -t.y() );
	case EsySoCCfgTile::NIPOS_NORTHEAST:
		return QPointF( t.x(), -t.y() );
	case EsySoCCfgTile::NIPOS_SOUTHWEST:
		return QPointF( - t.x(), t.y() );
	case EsySoCCfgTile::NIPOS_SOUTHEAST:
		return QPointF( t.x(), t.y() );
	default:
		return t;
	}
}

QVector< QPointF > TileGraphicsItem::lineTranspose(
	const QVector<QPointF> & points)
{
	QVector< QPointF > t_lines;
	for ( int i = 0; i < points.size(); i ++ )
	{
		t_lines.append( pointTranspose( points[ i ] ) );
	}
	return t_lines;
}

QPointF TileGraphicsItem::inputPortConnectPos()
{
	switch ( m_tile_cfg.niPos() )
	{
	case EsySoCCfgTile::NIPOS_SOUTHWEST:
	case EsySoCCfgTile::NIPOS_NORTHEAST:
		return pointTranspose(
			QPointF( m_tile_rect.width(), m_tile_rect.height() ) -
			QPointF( 2 * DRAW_VC_WIDTH * DRAW_SIN45D,
					 1 * DRAW_VC_WIDTH * DRAW_SIN45D ) );
	case EsySoCCfgTile::NIPOS_SOUTHEAST:
	case EsySoCCfgTile::NIPOS_NORTHWEST:
	default:
		return pointTranspose(
			QPointF( m_tile_rect.width(), m_tile_rect.height() ) -
			QPointF( 1 * DRAW_VC_WIDTH * DRAW_SIN45D,
					 2 * DRAW_VC_WIDTH * DRAW_SIN45D ) );
	}
}

QPointF TileGraphicsItem::outputPortConnectPos()
{
	switch ( m_tile_cfg.niPos() )
	{
	case EsySoCCfgTile::NIPOS_SOUTHWEST:
	case EsySoCCfgTile::NIPOS_NORTHEAST:
		return pointTranspose(
			QPointF( m_tile_rect.width(), m_tile_rect.height() ) -
			QPointF( 1 * DRAW_VC_WIDTH * DRAW_SIN45D,
					 2 * DRAW_VC_WIDTH * DRAW_SIN45D ) );
	case EsySoCCfgTile::NIPOS_SOUTHEAST:
	case EsySoCCfgTile::NIPOS_NORTHWEST:
	default:
		return pointTranspose(
			QPointF( m_tile_rect.width(), m_tile_rect.height() ) -
			QPointF( 2 * DRAW_VC_WIDTH * DRAW_SIN45D,
					 1 * DRAW_VC_WIDTH * DRAW_SIN45D ) );
	}
}

void TileGraphicsItem::setRecordItem( const EsyDataItemSoCRecord & item )
{
//	if ( (int)item.valid() == 1 )
//	{
		m_record_item = item;

		m_input_port->setLightEnable( item.msgRecv() > 0 );
		m_output_port->setLightEnable( item.msgSend() > 0 );

		for ( int i = 0; i < TILECOMP_COUNT; i ++ )
		{
			int t_hit = 0;
			int t_miss = 0;
			int t_offset = 0;
			switch ( i )
			{
			case TILECOMP_IADDER: t_hit = m_record_item.addInteger(); break;
			case TILECOMP_FADDER: t_hit = m_record_item.addFloat(); break;
			case TILECOMP_IMULTI: t_hit = m_record_item.multiIntegar(); break;
			case TILECOMP_FMULTI: t_hit = m_record_item.multiFloat(); break;
			case TILECOMP_IL1:
				t_hit = m_record_item.il1Hit();
				t_miss = m_record_item.il1Miss(); break;
			case TILECOMP_DL1:
				t_hit = m_record_item.dl1Hit();
				t_miss = m_record_item.dl1Miss(); break;
			case TILECOMP_IL2:
				t_offset = m_record_item.il1Hit();
				t_hit = m_record_item.il2Hit();
				t_miss = m_record_item.il2Miss(); break;
			case TILECOMP_DL2:
				t_offset = m_record_item.dl1Hit();
				t_hit = m_record_item.dl2Hit();
				t_miss = m_record_item.dl2Miss(); break;
			case TILECOMP_MEMORY:
				t_hit = m_record_item.memAccess(); break;
			case TILECOMP_NI:
				t_hit = m_record_item.msgProbe(); break;
			case TILECOMP_TILEID:
			case TILECOMP_REGS:
			default: break;
			}
			m_comp_list[ i ]->setLightNumber( t_hit, t_miss, t_offset );
		}
//	}
//	else
//	{
//		m_record_item.setSimCycle( item.simCycle() );
//	}
}

void TileGraphicsItem::paint(QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED( widget )

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
    QPointF tile_boundary_point_t[ 5 ];
	tile_boundary_point_t[ 0 ] = QPointF( m_tile_rect.width(), 0 );
	tile_boundary_point_t[ 1 ] = QPointF( 0, 0 );
	tile_boundary_point_t[ 2 ] = QPointF( 0, m_tile_rect.height() );
	tile_boundary_point_t[ 3 ] =
		QPointF( m_tile_rect.width(), m_tile_rect.height() ) -
		QPointF( 3 * DRAW_VC_WIDTH * DRAW_SIN45D, 0 );
	tile_boundary_point_t[ 4 ] =
		QPointF( m_tile_rect.width(), m_tile_rect.height() ) -
		QPointF( 0, 3 * DRAW_VC_WIDTH * DRAW_SIN45D );
	for ( int i = 0; i < 5; i ++ )
	{
		tile_boundary_point_t[ i ] =
			pointTranspose( tile_boundary_point_t[ i ] );
	}
	painter->drawPolygon( tile_boundary_point_t, 5 );

    m_input_port->setPos( inputPortConnectPos() );
//    m_input_port->setRotation( ni_angle[ m_tile_cfg.niPos() ] - 180 );
    m_output_port->setPos( outputPortConnectPos() );
//    m_output_port->setRotation( ni_angle[ m_tile_cfg.niPos() ] );

    painter->setPen(
        mp_graphics_cfg->configItem( "Component", "Normal" )->linePen() );
    if ( m_tile_cfg.tileType() != EsySoCCfgTile::TILE_ASIC )
    {
        QVector< QPointF > point_list = lineTranspose( DRAW_TILE_LINES );
        painter->drawLines( point_list );
    }
    else
    {
        QVector< QPointF > point_list = lineTranspose( DRAW_TILE_LINES );
        QVector< QPointF > point_list_1;
        point_list_1 << point_list[ 0 ] << point_list[ 1 ];
        painter->drawLines( point_list_1 );
    }

	for ( int i = 0; i < TILECOMP_COUNT; i ++ )
	{
        if ( m_tile_cfg.tileType() == EsySoCCfgTile::TILE_ASIC && i > 0 )
        {
            m_comp_list[ i ]->setVisible( false );
        }
        else
        {
            m_comp_list[ i ]->setVisible( true );
        }
        QPointF tile_pos = m_comp_init_pos_list[ i ] +
			( m_comp_rect_list[ i ] / 2 );
		m_comp_list[ i ]->setPos( pointTranspose( tile_pos ) );
    }

    if ( m_tile_cfg.tileType() == EsySoCCfgTile::TILE_ASIC )
    {
        painter->drawText( boundingRect(), Qt::AlignCenter,
            QString::fromStdString( m_tile_cfg.asicName() ) );
    }
}

