#include "router_state_table.h"

QStringList RouterStateModel::const_viewheader_list = QStringList() <<
    "Port" << "VC" << "State" << "Size" << "Packet" << "Src" << "Dst";

void RouterStateModel::setRouterGraphicsItem( RouterGraphicsItem *item )
{
    clear();

    mp_router_item = item;

    for ( int port = 0; port < item->routerCfg().portNum(); port ++ )
    {
		int vc_size = 0;
		if ( m_type == ROUTERSTATE_INPUT )
		{
			vc_size = item->routerCfg().port( port ).inputVcNumber();
		}
		else
		{
			vc_size = item->routerCfg().port( port ).outputVcNumber();
		}
		for ( int vc = 0; vc < vc_size; vc ++ )
        {
            QList< QStandardItem * > itemlist;
            itemlist << new QStandardItem( QString::number( port ) ) <<
                    new QStandardItem( QString::number( vc ) );
            for ( int col = 2; col < ROUTERSTATE_COLUMN_COUNT; col ++ )
            {
                itemlist << new QStandardItem();
            }
            appendRow( itemlist );
        }
    }

    update();
}

int RouterStateModel::rowCount( const QModelIndex &parent ) const
{
    if ( !isValid() )
    {
        return 0;
    }

    if ( parent == QModelIndex() )
    {
        return invisibleRootItem()->rowCount();
    }
    return 0;
}

int RouterStateModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    if ( !isValid() )
    {
        return 0;
    }
    return ROUTERSTATE_COLUMN_COUNT;
}

QVariant RouterStateModel::data( const QModelIndex &index, int role ) const
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
            if ( col == ROUTERSTATE_PORT )
            {
                int port = item( row, ROUTERSTATE_PORT )->text().toInt();
                t_str = QString::fromStdString(
                        mp_router_item->routerCfg().portName( port ) );
            }
            else if ( col >= ROUTERSTATE_STATE && col <= ROUTERSTATE_SIZE )
            {
                int port = item( row, ROUTERSTATE_PORT )->text().toInt();
                int vc = item( row, ROUTERSTATE_VC )->text().toInt();

                if ( m_type == ROUTERSTATE_INPUT )
                {
                    switch ( col )
                    {
                    case ROUTERSTATE_STATE:
                        t_str = mp_router_item->inputPortState( port, vc );
                        break;
                    case ROUTERSTATE_SIZE:
                        t_str = QString( "%1/%2" ).arg(
                            mp_router_item->inputFlitSize( port, vc ) ).arg(
                            mp_router_item->routerCfg().port( port ).
                                inputBuffer() );
                        break;
                    }
                }
                else
                {
                    switch ( col )
                    {
                    case ROUTERSTATE_STATE:
                        t_str = mp_router_item->outputPortState( port, vc );
                        break;
                    case ROUTERSTATE_SIZE:
                        t_str = QString( "%1/%2" ).arg(
                            mp_router_item->outputFlitSize( port, vc ) ).arg(
                            mp_router_item->routerCfg().port( port ).
                                outputBuffer() );
                        break;
                    }
                }
            }
            else if ( col >= ROUTERSTATE_FLIT )
            {
                int port = item( row, ROUTERSTATE_PORT )->text().toInt();
                int vc = item( row, ROUTERSTATE_VC )->text().toInt();

                bool valid = false;
                ShortFlitStruct flit;
                if ( m_type == ROUTERSTATE_INPUT )
                {
                    if ( mp_router_item->inputFlitSize( port, vc ) > 0 )
                    {
                        valid = true;
                        flit = mp_router_item->getInputTopFlit( port, vc );
                    }
                }
                else
                {
                    if ( mp_router_item->outputFlitSize( port, vc ) > 0 )
                    {
                        valid = true;
                        flit = mp_router_item->getOutputTopFlit( port, vc );
                    }
                }
                if ( valid )
                {
                    switch ( col )
                    {
                    case ROUTERSTATE_FLIT:
                        t_str = QString::number( flit.flitId() ); break;
                    case ROUTERSTATE_SRC:
                        t_str = QString::number( flit.flitSrc() ); break;
                    case ROUTERSTATE_DST:
                        t_str = QString::number( flit.flitDst() ); break;
                    }
                }
            }
            else
            {
                t_str = itemFromIndex( index )->text();
            }
            return t_str;
        }
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant RouterStateModel::headerData( int section,
                          Qt::Orientation orientation, int role) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        {
            if ( orientation == Qt::Horizontal )
            {
                return const_viewheader_list[ section ];
                break;
            }
        }
    }
    return QVariant();
}

void RouterStateModel::update()
{
    emit dataChanged( index(0, 0), index( rowCount() -1, columnCount() -1 ) );
}

RouterStateTableView::RouterStateTableView( QWidget * parent ) :
    QTableView( parent )
{
    setAlternatingRowColors( true );
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->hide();
    setEditTriggers( QAbstractItemView::NoEditTriggers );
}

void RouterStateTableView::updateColumnWidth()
{
    resizeColumnsToContents();

    setColumnWidth( RouterStateModel::ROUTERSTATE_STATE, 100 );
    setColumnWidth( RouterStateModel::ROUTERSTATE_SIZE, 50 );
}


void RouterStateTableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel( model );

    connect( this->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT( updateColumnWidth() ) );

    updateColumnWidth();
}
