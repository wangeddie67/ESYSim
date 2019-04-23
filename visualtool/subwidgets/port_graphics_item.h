#ifndef PORT_GRAPHICS_ITEM_H
#define PORT_GRAPHICS_ITEM_H

#include "../define/qt_include.h"
#include "../modelsview/graphics_config_tree.h"

class ShortFlitStruct
{
public:
    enum FlitTypeEnum{
        FLIT_HEAD = 0,
        FLIT_BODY,
        FLIT_TAIL
    };

    enum FlitFlagEnum
    {
        FLIT_NULL = 0x00,
        FLIT_ACK = 0x01,
        FLIT_DROP = 0x02
    };

    static QStringList const_flit_type_string;

private:
    long m_flit_id;
    FlitTypeEnum m_flit_type;
    int m_flit_src;
    int m_flit_dst;
    int m_flit_flag;

public:
    ShortFlitStruct( void ) :
        m_flit_id( -1 ), m_flit_type( FLIT_HEAD ),
        m_flit_src( -1 ), m_flit_dst( -1 ), m_flit_flag( 0 )
    {}
    ShortFlitStruct( long id, FlitTypeEnum type, int src, int dst, int flag ) :
        m_flit_id( id ), m_flit_type( type ),
        m_flit_src( src ), m_flit_dst( dst ), m_flit_flag( flag )
    {}
    ShortFlitStruct( const ShortFlitStruct & t ) :
        m_flit_id( t.m_flit_id ), m_flit_type( t.m_flit_type ),
        m_flit_src( t.m_flit_src ), m_flit_dst( t.m_flit_dst ),
        m_flit_flag( t.m_flit_flag )
    {}

    long flitId() const { return m_flit_id; }
    FlitTypeEnum flitType() const { return m_flit_type; }
    int flitSrc() const { return m_flit_src; }
    int flitDst() const { return m_flit_dst; }
    int flitFlag() const { return m_flit_flag; }
};

// draw NOC port
class PortGraphicsItem : public QGraphicsItem
{
	// constructor and destructor
public:
    PortGraphicsItem( qreal width, qreal height, int buffer_size,
        GraphicsConfigModel * graphics_cfg, QGraphicsItem * parent = 0 );

	// variables
private:
    static QStringList const_state_string_list;

    GraphicsConfigModel * m_graphics_cfg;

    QRectF m_buffer_rect;
    qreal m_flit_width;

    int m_buffer_size;	// buffer size
    QVector< ShortFlitStruct > m_flit_list;
    long m_timer;

    long m_traffic;
    long m_max_traffic;

    bool m_faulty_flag;
    bool m_path_flag;

    QString m_state_str;

	// I/O functions
public:
    void setBufferSize( long size ) { m_buffer_size = size; }

    void append( const ShortFlitStruct & flit )
        { m_flit_list.append( flit ); }
    void removeFirst() { m_flit_list.remove( 0 ); m_timer = 0; }
    void prepend( const ShortFlitStruct & flit )
        { m_flit_list.prepend( flit ); m_timer = 0; }
    void removeLast() { m_flit_list.remove( m_flit_list.size() - 1 ); }
    int flitSize() const { return m_flit_list.size(); }
    const ShortFlitStruct & getFlit( int index ) const
        { return m_flit_list[ index ]; }
    const ShortFlitStruct & getFirstFlit() const
        { return m_flit_list.first(); }
    const ShortFlitStruct & getLastFlit() const
        { return m_flit_list.last(); }

    void incTimer() { m_timer ++; }
    long timer() { return m_timer; }

    void setState( int state )
        { m_state_str = const_state_string_list[ state ]; }
    const QString & state() { return m_state_str; }

    void setFaulty( bool faulty ) { m_faulty_flag = faulty; }
    bool faulty() { return m_faulty_flag; }

    void setPath( bool path ) { m_path_flag = path; }
    void clearTraffic() { m_traffic = 0; }
    void incTraffic( long traffic ) { m_traffic += traffic; }
    void setMaxTraffic( long traffic ) { m_max_traffic = traffic; }

	// rewrote draw function;
private:
	// draw noc router
	QRectF boundingRect() const;
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget );
};

// draw NI port
class NIGraphicsItem : public QGraphicsItem
{
    // constructor and destructor
public:
    NIGraphicsItem( qreal width, qreal height, qreal length,
        GraphicsConfigModel * graphics_cfg, QGraphicsItem * parent = 0 );

    // variables
private:
    QRectF m_ni_rect;
    qreal m_width;
    qreal m_height;
    qreal m_length;

    GraphicsConfigModel * m_graphics_cfg;
    // rewrote draw function;
private:
    // draw noc router
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
};

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
