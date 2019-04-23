#ifndef TILE_STATE_TABLE_H
#define TILE_STATE_TABLE_H

#include "../define/qt_include.h"

#include "../subwidgets/tile_graphics_item.h"

/*!
 * \brief model of router status
 *
 * Show the status of input/output ports, including the first flit in the buffer.
 */
class TileStateModel : public QStandardItemModel
{
	Q_OBJECT

public:
	enum TileStateColumn
	{
		TILESTATE_NAME = 0,
		TILESTATE_VALUE,
		TILESTATE_COLUMN_COUNT
	};

	/*!
	 * \brief column of router state view
	 */
	enum TileStateRow
	{
		TILESTATE_ID = 0,
		TILESTATE_CYCLE,
		TILESTATE_IL1_ACCESS,
		TILESTATE_IL1_HIT,
		TILESTATE_IL1_MISS,
		TILESTATE_DL1_ACCESS,
		TILESTATE_DL1_HIT,
		TILESTATE_DL1_MISS,
		TILESTATE_IL2_ACCESS,
		TILESTATE_IL2_HIT,
		TILESTATE_IL2_MISS,
		TILESTATE_DL2_ACCESS,
		TILESTATE_DL2_HIT,
		TILESTATE_DL2_MISS,
		TILESTATE_MEM_ACCESS,
		TILESTATE_MSG_SEND,
		TILESTATE_MSG_RECV,
		TILESTATE_MSG_PROBE,
		TILESTATE_ADDR_INTEGER,
		TILESTATE_MULTI_INTEGER,
		TILESTATE_ADDR_FLOAT,
		TILESTATE_MULTI_FLOAT,
		TILESTATE_ROW_COUNT
	};

protected:
	static QStringList const_viewheader_list;

	TileGraphicsItem * mp_tile_item;  /**< pointer to router item in graphic */

public:
	/** @name construct functions
	 * Construct an new model.
	 */
	///@{
	TileStateModel( QObject *parent ) :
		QStandardItemModel( parent ), mp_tile_item( 0 )
	{}
	///@}

	/*!
	 * \brief set pointer to router item in graphic
	 * \param item  pointer to router item in graphic
	 */
	void setTileGraphicsItem( TileGraphicsItem * item );

	/** @name functions for MVC
	 * reimplementation functions for MVC.
	 */
	///@{
	/*!
	 * \brief Returns the number of rows under the given parent.
	 *
	 * If the parent is invalid, return the children of root item. Otherwise,
	 * return 0.
	 *
	 * \param parent  the parent index of model
	 * \return  the total number of items or 0.
	 */
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	/*!
	 * \brief returns the number of columns for the children of the given parent.
	 * \param parent  the parent index of model
	 * \return  ROUTERSTATE_COLUMN_COUNT
	 */
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	/*!
	 * \brief returns the data stored under the given role for the item
	 * referred to by the index.
	 *
	 * For item in input/output physical channel column, show the name of port;
	 * Otherwise, show the text in item directly.
	 *
	 * \param index  model index of item
	 * \param role   DisplayRole, EditRole or CheckStateRole
	 * \return  data stored under the given role for this item
	 */
	QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
	/*!
	 * \brief returns the data for the given role and section in the header
	 * with the specified orientation.
	 *
	 * Horizontal header returns const_viewheader_list.
	 *
	 * \param section      row number or column number
	 * \param orientation  orientation, vertical or horizational
	 * \param role         role, DisplayRole or TextAlignmentRole
	 * \return  data for the given role and section in the specified header
	 */
//	QVariant headerData( int section, Qt::Orientation orientation,
//						 int role) const;
	///@}

	/*!
	 * \brief update the router state table.
	 */
	void update();

protected:
	/*!
	 * \brief check the pointer to router item
	 * \return  if the pointer is 0, return FALSE; otherwise, return TRUE.
	 */
	bool isValid() const { return (mp_tile_item != 0); }
};

/*!
 * \brief table view for router status.
 */
class TileStateTableView : public QTableView
{
	Q_OBJECT

public:
	/*!
	 * \brief constructs a new table for router status
	 * \param parent  pointer to the parent widget
	 */
	TileStateTableView( QWidget * parent );
};

#endif // NETWORK_CFG_TREE_H
