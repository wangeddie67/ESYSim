#include "tile_state_table.h"

QStringList TileStateModel::const_viewheader_list = QStringList() <<
	"Tile ID" << "Tile Cycle" <<
	"IL1 Cache Access" << "IL1 Cache Hit" << "IL1 Cache Miss" <<
	"DL1 Cache Access" << "DL1 Cache Hit" << "DL1 Cache Miss" <<
	"IL2 Cache Access" << "IL2 Cache Hit" << "IL2 Cache Miss" <<
	"DL2 Cache Access" << "DL2 Cache Hit" << "DL2 Cache Miss" <<
	"Memory Access" << "Message Send" << "Message Receive" << "Message Probe" <<
	"Integer Adder" << "Integer Multiplexer" << "Float Adder" << "Float Multiplexer";

void TileStateModel::setTileGraphicsItem( TileGraphicsItem *item )
{
	if ( mp_tile_item )
	{
		invisibleRootItem()->removeRows( 0, TILESTATE_ROW_COUNT );
	}

	mp_tile_item = item;

	for ( int row = 0; row < TILESTATE_ROW_COUNT; row ++ )
	{
		QList< QStandardItem * > itemlist;
		itemlist << new QStandardItem( const_viewheader_list[ row ] ) <<
			new QStandardItem( "" );
		appendRow( itemlist );
	}

	update();
}

int TileStateModel::rowCount( const QModelIndex &parent ) const
{
	Q_UNUSED( parent );
	if ( !isValid() )
	{
		return 0;
	}

	if ( parent == QModelIndex() )
	{
		return TILESTATE_ROW_COUNT;
	}
	return 0;
}

int TileStateModel::columnCount( const QModelIndex &parent ) const
{
	Q_UNUSED( parent );
	if ( !isValid() )
	{
		return 0;
	}
	return TILESTATE_COLUMN_COUNT;
}

QVariant TileStateModel::data( const QModelIndex &index, int role ) const
{
	if ( !isValid() )
	{
		return 0;
	}

	int row = index.row();
	int col = index.column();

	switch( role )
	{
	case Qt::DisplayRole:
		{
			QString t_str;
			if ( col == TILESTATE_NAME )
			{
				t_str = const_viewheader_list[ row ];
			}
			else if ( col == TILESTATE_VALUE )
			{
				EsyDataItemSoCRecord t_item = mp_tile_item->recordItem();
				switch ( row )
				{
				case TILESTATE_ID:
					t_str = QString::number( t_item.actualId() ); break;
				case TILESTATE_CYCLE:
					t_str = QString::number( t_item.simCycle() ); break;
				case TILESTATE_IL1_ACCESS:
					t_str = QString::number( t_item.il1Access() ); break;
				case TILESTATE_IL1_HIT:
					t_str = QString::number( t_item.il1Hit() ); break;
				case TILESTATE_IL1_MISS:
					t_str = QString::number( t_item.il1Miss() ); break;
				case TILESTATE_DL1_ACCESS:
					t_str = QString::number( t_item.dl1Access() ); break;
				case TILESTATE_DL1_HIT:
					t_str = QString::number( t_item.dl1Hit() ); break;
				case TILESTATE_DL1_MISS:
					t_str = QString::number( t_item.dl1Miss() ); break;
				case TILESTATE_IL2_ACCESS:
					t_str = QString::number( t_item.il2Access() ); break;
				case TILESTATE_IL2_HIT:
					t_str = QString::number( t_item.il2Hit() ); break;
				case TILESTATE_IL2_MISS:
					t_str = QString::number( t_item.il2Miss() ); break;
				case TILESTATE_DL2_ACCESS:
					t_str = QString::number( t_item.dl2Access() ); break;
				case TILESTATE_DL2_HIT:
					t_str = QString::number( t_item.dl2Hit() ); break;
				case TILESTATE_DL2_MISS:
					t_str = QString::number( t_item.dl2Miss() ); break;
				case TILESTATE_MEM_ACCESS:
					t_str = QString::number( t_item.memAccess() ); break;
				case TILESTATE_MSG_SEND:
					t_str = QString::number( t_item.msgSend() ); break;
				case TILESTATE_MSG_RECV:
					t_str = QString::number( t_item.msgRecv() ); break;
				case TILESTATE_MSG_PROBE:
					t_str = QString::number( t_item.msgProbe() ); break;
				case TILESTATE_ADDR_INTEGER:
					t_str = QString::number( t_item.addInteger() ); break;
				case TILESTATE_MULTI_INTEGER:
					t_str = QString::number( t_item.multiIntegar() ); break;
				case TILESTATE_ADDR_FLOAT:
					t_str = QString::number( t_item.addFloat() ); break;
				case TILESTATE_MULTI_FLOAT:
					t_str = QString::number( t_item.multiFloat() ); break;
				default: break;
				}
			}
			return t_str;
		}
	case Qt::TextAlignmentRole:
		return Qt::AlignCenter;
	}
	return QVariant();
}

void TileStateModel::update()
{
	emit dataChanged( index(0, 0), index( rowCount() -1, columnCount() -1 ) );
}

TileStateTableView::TileStateTableView( QWidget * parent ) :
	QTableView( parent )
{
	setAlternatingRowColors( true );
	horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	horizontalHeader()->hide();
	verticalHeader()->hide();
	setEditTriggers( QAbstractItemView::NoEditTriggers );
}

