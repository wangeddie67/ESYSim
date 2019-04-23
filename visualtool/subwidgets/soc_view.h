#ifndef NOC_VIEW_H
#define NOC_VIEW_H

#include "../define/qt_include.h"
#include "../subwidgets/network_graphics_item.h"
#include "../subwidgets/tile_graphics_item.h"

// draw NoC and show the states of each packet and router
class SoCView : public QGraphicsView
{
	Q_OBJECT
	
	// constructorno and deconstructor
public:
	SoCView ( QWidget * parent = 0 );
	~SoCView ( void );

	// display variable
private:
	qreal sx, sy;	// scalar factor
	
signals:
	void selectRouterEvent( QMouseEvent * event );
	void hotKeyEvent( QKeyEvent * event );
	
protected:
	void wheelEvent(QWheelEvent * event);
	void keyReleaseEvent(QKeyEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
};

class SoCScene : public QGraphicsScene
{
    Q_OBJECT

    // constructorno and deconstructor
private:
    QPointF m_last_pos;
    QPointF m_last_scene_pos;
    QPoint m_last_screen_pos;

    QVector< RouterGraphicsItem * > m_router_vector;
	QVector< TileGraphicsItem * > m_tile_vector;
	QVector< LinkGraphicsItem * > m_link_vector;
	QVector< LinkGraphicsItem * > m_ni_vector;

	EsySoCCfg * mp_soc_cfg;
	GraphicsConfigModel * mp_graphics_cfg;

public:
	SoCScene ( EsySoCCfg* soc_cfg,
		GraphicsConfigModel * graphics_cfg, QWidget * parent = 0 ) :
		QGraphicsScene( parent ),
		mp_soc_cfg( soc_cfg ), mp_graphics_cfg( graphics_cfg )
	{
		setBackgroundBrush( QBrush( Qt::white ) );
        drawSoCScene();
	}

	RouterGraphicsItem * routerItem( long index )
		{ return m_router_vector[ index ]; }
	TileGraphicsItem * tileItem( long index )
		{ return m_tile_vector[ index ]; }

	void drawSoCScene();
	void clearSoCScene();
	LinkGraphicsItem * addNetworkLink( int src_id, int src_port );
	LinkGraphicsItem * addTileLink( int tile, bool in );
	void regularScene();

	void disableTileSelectable( int tile );
	long long simCycle( int tile );

	int selectedRouter();
	int selectedTile();
	void clearSelected();
	void setSelectRouter( int index );
	void setSelectTile( int index );

	void recordNextHandler( const EsyDataItemSoCRecord & recorditem );
	void recordPrevHandler( const EsyDataItemSoCRecord & recorditem );

	void UpdateTooltip();
protected:
	void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent );
};

#endif
