#include "faults_list_table.h"

QStringList FaultListModel::const_viewheader_list = QStringList() <<
    "Type" << "Router" << "Port" << "VC" << "Bit" << "State" << "Trasfer Matrix";

int FaultListModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent == QModelIndex() )
    {
        return invisibleRootItem()->rowCount();
    }
    else
    {
        return 0;
    }
}

int FaultListModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return FAULTCFG_COLUMN;
}

QVariant FaultListModel::data( const QModelIndex &index, int role ) const
{
    int row = index.row();
    int column = index.column();
    switch( role )
    {
    case Qt::DisplayRole:
        {
            FaultListModelItem * t_item = (FaultListModelItem *)item( row );
            QString t_str;
            switch ( column )
            {
            case FAULTCFG_TYPE:
                if ( t_item->faultType() == -1 )
                {
                    t_str = "All";
                }
                else
                {
                    t_str = QString::fromStdString(
                        EsyFaultConfigItem::faultTypeStr( t_item->faultType() ) );
                }
                break;
            case FAULTCFG_ROUTER:
                if ( t_item->router() == -1 )
                {
                    t_str = "All";
                }
                else
                {
                    t_str = QString::number( t_item->router() );
                }
                break;
            case FAULTCFG_PORT:
                if ( t_item->port() == -1 )
                {
                    t_str = "All";
                }
                else
                {
                    t_str = QString::number( t_item->port() );
                }
                break;
            case FAULTCFG_VC:
                if ( t_item->vc() == -1 )
                {
                    t_str = "All";
                }
                else
                {
                    t_str = QString::number( t_item->vc() );
                }
                break;
            case FAULTCFG_BIT:
                if ( t_item->bit() == -1 )
                {
                    t_str = "All";
                }
                else
                {
                    t_str = QString::number( t_item->bit() );
                }
                break;
            case FAULTCFG_STATE:
                {
                    vector< long > t_state = t_item->state();
                    t_str += "";
                    for ( size_t i = 0; i < t_state.size(); i ++ )
                    {
                        if ( i != 0 )
                        {
                            t_str += "\n";
                        }
                        if ( t_state[ i ] == 0 )
                        {
                            t_str += "Living";
                        }
                        else
                        {
                            t_str += "Faulty";
                        }
                    }
                    break;
                }
            case FAULTCFG_TRASFER:
                {
                    vector< vector< double > > t_matrix =
                            t_item->transferMatrix();
                    t_str += "";
                    for ( size_t i = 0; i < t_matrix.size(); i ++ )
                    {
                        if ( i != 0 )
                        {
                            t_str += "\n";
                        }
                        for ( size_t j = 0; j < t_matrix[ i ].size(); j ++ )
                        {
                            if ( j != 0 )
                            {
                                t_str += ",";
                            }
                            t_str += QString::number( t_matrix[ i ][ j ] );
                        }
                    }
                    break;
                }
            }
            return t_str;
        }
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant FaultListModel::headerData( int section,
                          Qt::Orientation orientation, int role) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        {
            if ( orientation == Qt::Horizontal )
            {
                if ( section < FAULTCFG_COLUMN  )
                {
                    return const_viewheader_list[ section ];
                }
                break;
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags FaultListModel::flags( const QModelIndex &index ) const
{
    Q_UNUSED( index )

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void FaultListModel::setFaultCfgList(const EsyFaultConfigList &fault_cfg_list)
{
    for ( size_t i = 0; i < fault_cfg_list.size(); i ++ )
    {
        appendItem( fault_cfg_list[ i ] );
    }
}

EsyFaultConfigList FaultListModel::getFaultCfgList()
{
    EsyFaultConfigList t_list;
    for ( int i = 0; i < invisibleRootItem()->rowCount(); i ++ )
    {
        t_list.push_back(
            ((FaultListModelItem *)item( i ))->getEsyFaultConfigItem() );
    }

    return t_list;
}

void FaultListModel::appendItem( const EsyFaultConfigItem &item )
{
    QList< QStandardItem * > t_item_list;
    t_item_list << new FaultListModelItem( item );
    for ( int j = 1; j < columnCount(); j ++ )
    {
        t_item_list << new QStandardItem();
    }
    appendRow( t_item_list );
}

void FaultListModel::updateItem( int row, const EsyFaultConfigItem & new_item )
{
    *( (FaultListModelItem *)item( row ) ) = new_item;
    emit dataChanged( index( row, 0 ), index( row, FAULTCFG_COLUMN ) );
}

FaultListTableView::FaultListTableView(QWidget *parent) :
    QTableView( parent )
{
    horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    verticalHeader()->setHidden( true );
    setAlternatingRowColors( true );
    setSelectionBehavior( QAbstractItemView::SelectRows );
}
