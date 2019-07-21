#include "path_trace_table.h"

QStringList PathTraceModel::const_viewheader_list = QStringList() <<
    "ID" << "In\nPC" << "In\nVC" << "Out\nPC" << "Out\nVC" << "Traffic";

void PathTraceModel::setPathTrace( const vector< EsyDataItemHop > & pathhop )
{
    clear();

    for ( size_t hop = 0; hop < pathhop.size(); hop ++ )
    {
        QList< QStandardItem * > itemlist;
        itemlist <<
            new StandardSortingItem( QString::number(
                pathhop[ hop ].addr() ) ) <<
            new StandardSortingItem( QString::number(
                pathhop[ hop ].inputPc() ) ) <<
            new StandardSortingItem( QString::number(
                pathhop[ hop ].inputVc() ) ) <<
            new StandardSortingItem( QString::number(
                pathhop[ hop ].outputPc() ) ) <<
            new StandardSortingItem( QString::number(
                pathhop[ hop ].outputVc() ) ) <<
            new StandardSortingItem( "1" );
        appendRow( itemlist );
    }
}

void PathTraceModel::setPathPair(
        const QVector< EsyDataItemTraffic > & pathpair )
{
    clear();

    for ( int hop = 0; hop < pathpair.size(); hop ++ )
    {
        QList< QStandardItem * > itemlist;
        itemlist <<
            new StandardSortingItem( QString::number(
                pathpair[ hop ].addr() ) ) <<
            new StandardSortingItem( QString::number(
                pathpair[ hop ].inputPc() ) ) <<
            new StandardSortingItem( QString::number(
                pathpair[ hop ].inputVc() ) ) <<
            new StandardSortingItem( QString::number(
                pathpair[ hop ].outputPc() ) ) <<
            new StandardSortingItem( QString::number(
                pathpair[ hop ].outputPc() ) ) <<
            new StandardSortingItem( QString::number(
                pathpair[ hop ].traffic() ) );
        appendRow( itemlist );
    }
}

int PathTraceModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent == QModelIndex() )
    {
        return invisibleRootItem()->rowCount();
    }
    return 0;
}

int PathTraceModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );

    return PATHTRACE_COLUMN_COUNT;
}

QVariant PathTraceModel::data( const QModelIndex &index, int role ) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        if ( index.column() == PATHTRACE_IN_PC ||
             index.column() == PATHTRACE_OUT_PC )
        {
            EsyNetCfgRouter routercfg = mp_network_cfg->router(
                        item( index.row(), PATHTRACE_ID )->text().toInt() );
            return QString::fromStdString(
                routercfg.portName( itemFromIndex( index )->text().toInt() ) );
        }
        else
        {
            return itemFromIndex( index )->text();
        }
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return QVariant();
}

QVariant PathTraceModel::headerData( int section,
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

PathTraceTableView::PathTraceTableView( QWidget * parent ) :
    QTableView( parent )
{
    setAlternatingRowColors( true );
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->hide();
    setEditTriggers( QAbstractItemView::NoEditTriggers );
}
