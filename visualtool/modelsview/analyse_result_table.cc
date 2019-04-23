#include "analyse_result_table.h"

int ResultTableModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent == QModelIndex() )
    {
        return m_row_count;
    }
    return 0;
}

int ResultTableModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return m_column_count;
}

QVariant ResultTableModel::data( const QModelIndex &index, int role ) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        return itemFromIndex( index )->text();
    case Qt::TextAlignmentRole:
        return Qt::AlignRight + Qt::AlignVCenter;
    }
    return QVariant();
}

QVariant ResultTableModel::headerData( int section,
                          Qt::Orientation orientation, int role) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        if ( orientation == Qt::Horizontal )
        {
            return m_header_list[ section ];
            break;
        }
        else
        {
            return QString::number( section );
            break;
        }
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return QVariant();
}

Qt::ItemFlags ResultTableModel::flags( const QModelIndex &index ) const
{
    Q_UNUSED( index )

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
}

void ResultTableModel::clearData()
{
    clear();
    m_header_list.clear();
    m_row_count = 0;
    m_column_count = 0;
}

void ResultTableModel::appendDataVector( const QString & header,
    const QVector< double > & value )
{
    m_column_count ++;
    m_header_list.append( header );
    if ( m_row_count <= value.size() )
    {
        m_row_count = value.size();
    }

    for ( int i = 0; i < value.size(); i ++ )
    {
        this->setItem( i, m_column_count - 1,
            new StandardSortingItem( QString::number( value[ i ] ) ) );
    }
}

void ResultTableModel::appendDataItem( const QStringList & data )
{
    m_row_count ++;
    if ( m_column_count <= data.size() )
    {
        m_column_count = data.size();
    }

    QList< QStandardItem * > item_list;
    for ( int i = 0; i < data.size(); i ++ )
    {
        item_list << new StandardSortingItem( data[ i ] );
    }
    appendRow( item_list );
}

void ResultTableModel::appendDataItem(
    const QString & argument, double value, const QString & tooltip
)
{
    m_row_count ++;
    appendRow( QList< QStandardItem * >() <<
        new StandardSortingItem( argument ) <<
        new StandardSortingItem( QString::number( value ) ) <<
        new StandardSortingItem( tooltip ));
}

void ResultTableModel::setHeaderList( const QStringList & header )
{
    m_header_list = header;
    m_column_count = m_header_list.size();
}

ErrorHandler ResultTableModel::writeDataFile( const QString & file )
{
    QFile t_file( file );
    if ( !t_file.open( QIODevice::WriteOnly ) )
    {
        return ErrorHandler( ErrorHandler::FILEOPENERROR, QStringList() << file,
            "AnalyseResultTableModel", "writeDataFile" );
    }
    QTextStream t_stream( &t_file );

    for ( int col = 0; col < columnCount(); col ++ )
    {
        if ( col > 0 )
        {
            t_stream << "\t";
        }
        t_stream << m_header_list[ col ];
    }
    t_stream << endl;

    for ( int row = 0; row < rowCount(); row ++ )
    {
        for ( int col = 0; col < columnCount(); col ++ )
        {
            if ( col > 0 )
            {
                t_stream << "\t";
            }
            t_stream << item( row, col )->text();
        }
        t_stream << endl;
    }

    t_file .close();
    return ErrorHandler();
}

ResultTableView::ResultTableView( QWidget * parent ) :
    QTableView( parent )
{
    horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    verticalHeader()->setHidden( true );
    setAlternatingRowColors( true );
    setSortingEnabled( true );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
}
