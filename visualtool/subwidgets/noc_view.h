#ifndef NOC_VIEW_H
#define NOC_VIEW_H

#include "../define/qt_include.h"
#include "../subwidgets/router_graphics_item.h"

// draw NoC and show the states of each packet and router
class NoCView : public QGraphicsView
{
	Q_OBJECT
	
	// constructorno and deconstructor
public:
	NoCView ( QWidget * parent = 0 );
	~NoCView ( void );

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

class NoCScene : public QGraphicsScene
{
    Q_OBJECT

    // constructorno and deconstructor
private:
    QPointF m_last_pos;
    QPointF m_last_scene_pos;
    QPoint m_last_screen_pos;

    QVector< RouterGraphicsItem * > m_router_vector;
    QVector< LinkGraphicsItem * > m_link_vector;

    EsyNetworkCfg * mp_network_cfg;
    GraphicsConfigModel * mp_graphics_cfg;

public:
    NoCScene ( EsyNetworkCfg* network_cfg,
        GraphicsConfigModel * graphics_cfg, QWidget * parent = 0 ) :
        QGraphicsScene( parent ),
        mp_network_cfg( network_cfg ), mp_graphics_cfg( graphics_cfg )
    {
        setBackgroundBrush( QBrush( Qt::white ) );
        drawNoCScene();
    }

    void drawNoCScene();
    void clearNoCScene();
    LinkGraphicsItem * addLink( int src_id, int src_port );
	void regularScene();

    RouterGraphicsItem * routerItem( int index )
        { return m_router_vector[ index ]; }

    int selectedRouter();
    void setSelectRouterNoCScene( int index );

    void setPathTraceModelItemVector( const QVector< EsyDataItemTraffic > & list );

    bool eventCheck( const EsyDataItemEventtrace & eventitem, QString & errmsg, bool forward );
    void eventNextHandler( const EsyDataItemEventtrace & eventitem );
    void eventPrevHandler( const EsyDataItemEventtrace & eventitem );
    void pathHandler( const EsyDataItemPathtrace & pathitem );

    void eventTraceDeadlockCheck();
    void pathTraceDeadlockCheck();
    QVector< EsyDataItemHop > graphicsDeadlockCheck(
        QVector< QVector< QVector< QVector< QPair< int, int > > > > > graphics );

    void UpdateTooltip();

protected:
    void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent );
};

#endif
