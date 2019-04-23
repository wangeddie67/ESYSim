#ifndef PORT_GRAPHICS_ITEM_H
#define PORT_GRAPHICS_ITEM_H

#include "../define/qt_include.h"
#include "../modelsview/graphics_config_tree.h"
#include "network_graphics_item.h"
#include "esy_soccfg.h"
#include "esy_interdata.h"

// draw light
class TileCompGraphicsItem : public QGraphicsItem
{
	// constructor and destructor
public:
	TileCompGraphicsItem( qreal width, qreal height, qreal fontsize,
		const QString & text, qreal radius, int lightnum,
		GraphicsConfigModel * graphics_cfg, QGraphicsItem * parent = 0 );

	// variables
private:
	GraphicsConfigModel * mp_graphics_cfg;
	QRectF m_rect;
	qreal m_radius;
	qreal m_fontsize;
	QString m_text;

	int m_light_num;
	int m_hit_num;
	int m_miss_num;
	int m_offset;

	// I/O functions
public:
	void setLightNumber( int hit, int miss, int offset );

	// rewrote draw function;
private:
	// draw noc router
	QRectF boundingRect() const;
	void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget );
};

// draw one tile
class TileGraphicsItem : public QGraphicsItem
{
private:
	enum TileCompEnum
	{
		TILECOMP_TILEID,
		TILECOMP_IADDER,
		TILECOMP_FADDER,
		TILECOMP_IMULTI,
		TILECOMP_FMULTI,
		TILECOMP_REGS,
		TILECOMP_IL1,
		TILECOMP_DL1,
		TILECOMP_IL2,
		TILECOMP_DL2,
		TILECOMP_MEMORY,
		TILECOMP_NI,
		TILECOMP_COUNT
	};

	// constructor and deconstructor
public:
	TileGraphicsItem( const EsySoCCfgTile & tile_cfg,
		GraphicsConfigModel * graphics_cfg );

	// noc variable
private:
	static qreal ni_angle[ EsySoCCfgTile::NIPOS_COUNT ];

	EsySoCCfgTile m_tile_cfg;
	GraphicsConfigModel * mp_graphics_cfg;
	EsyDataItemSoCRecord m_record_item;

	PortGraphicsItem * m_input_port;
	PortGraphicsItem * m_output_port;

	static const QStringList m_comp_text_list;
	static const QList< QPointF > m_comp_rect_list;
	static const QList< QPointF > m_comp_init_pos_list;

	QList< TileCompGraphicsItem * > m_comp_list;

	// draw parameter
	QSizeF m_tile_rect;

	// I/O function
public:
	// port position
	QPointF inputPortConnectPos( int phy );
	QPointF outputPortConnectPos( int phy );

	const EsySoCCfgTile & tileCfg() const { return m_tile_cfg; }

	const EsyDataItemSoCRecord & recordItem() { return m_record_item; }
	void setRecordItem( const EsyDataItemSoCRecord & item );

	// rewrote draw function;
	// draw noc router
public:
	QRectF boundingRect() const;
	void setNiPos( const QPointF & pos );
	QPointF inputPortConnectPos();
	QPointF outputPortConnectPos();

private:
	void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option, QWidget *widget);

	QRectF rectTranspose( const QRectF & rect );
	QPointF pointTranspose( const QPointF & point );
	QVector< QPointF > lineTranspose( const QVector<QPointF> & pointPairs );
};

#endif
