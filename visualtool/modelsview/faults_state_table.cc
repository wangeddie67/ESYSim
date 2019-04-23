#include "faults_state_table.h"

int FaultStateModel::rowCount( const QModelIndex &parent ) const
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

int FaultStateModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    return 1;
}

QVariant FaultStateModel::data( const QModelIndex &index, int role ) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        if ( itemFromIndex( index )->text() == "0" )
        {
            return QString( "Living" );
        }
        else
        {
            return QString( "Faulty" );
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    case Qt::TextColorRole:
        if ( itemFromIndex( index )->text() == "0" )
        {
            return QBrush( Qt::black );
        }
        else
        {
            return QBrush( Qt::red );
        }
        break;
    }
    return QVariant();
}

QVariant FaultStateModel::headerData(
    int section, Qt::Orientation orientation, int role) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        if ( orientation == Qt::Horizontal )
        {
            return QString( "States Value" );
        }
        else
        {
            return QString::number( section );
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    }
    return QVariant();
}

Qt::ItemFlags FaultStateModel::flags( const QModelIndex &index ) const
{
    Q_UNUSED( index )

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void FaultStateModel::setStatesVector( const vector< long > & value )
{
    clear();
    for ( size_t i = 0; i < value.size(); i ++ )
    {
        appendRow( QList< QStandardItem * >() <<
            new QStandardItem( QString::number( value[ i ] ) ) );
    }
    emit dataChanged( index(0, 0), index( rowCount() -1, 0 ) );
}

vector< long > FaultStateModel::getStatesVector()
{
    vector< long > t_state;
    for ( int i = 0; i < rowCount(); i ++ )
    {
        t_state.push_back( item( i, 0 )->text().toInt() );
    }
    return t_state;
}

void FaultStateModel::resizeStates( int size )
{
    if ( size < rowCount() )
    {
        for ( int i = size; i < rowCount(); i ++ )
        {
            if ( item( i, 0 ) )
            {
                delete item( i, 0 );
            }
        }
        removeRows( size, rowCount() - size );
    }

    if ( size > rowCount() )
    {
        for ( int i = rowCount(); i < size; i ++ )
        {
            if ( item( i, 0 ) == 0 )
            {
                appendRow( QList< QStandardItem * >() <<
                    new QStandardItem( "0" ) );
            }
        }
    }

    emit dataChanged( index(0, 0), index( rowCount() -1, 0 ) );
}

void FaultStateModel::setStateFaulty( int state, bool faulty )
{
    if ( item( state, 0 ) )
    {
        if ( faulty )
        {
            item( state, 0 )->setText( "1" );
        }
        else
        {
            item( state, 0 )->setText( "0" );
        }
    }
}

void FaultStateModel::changeItemStates( const QModelIndex &index )
{
    if ( itemFromIndex( index )->text() == "0" )
    {
        itemFromIndex( index )->setText( "1" );
    }
    else
    {
        itemFromIndex( index )->setText( "0" );
    }
}

int FaultMatrixModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );

    return m_matrix_size;
}

int FaultMatrixModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );

    return m_matrix_size;
}

QVariant FaultMatrixModel::data( const QModelIndex &index, int role ) const
{
    switch( role )
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return itemFromIndex( index )->text();
    case Qt::TextAlignmentRole:
        return Qt::AlignRight + Qt::AlignVCenter;
    case Qt::TextColorRole:
        double prob = 0.0;
        for ( int i = 0; i < m_matrix_size; i ++ )
        {
            if ( item( index.row(), i ) )
            {
                prob += item( index.row(), i )->text().toDouble();
            }
        }
        if ( prob == 1.0 )
        {
            return QBrush( Qt::black );
        }
        else
        {
            return QBrush( Qt::red );
        }
        break;
    }
    return QVariant();
}

bool FaultMatrixModel::setData(
    const QModelIndex & index, const QVariant & value, int role )
{
    switch( role )
    {
    case Qt::EditRole:
        itemFromIndex( index )->setText( QString::number( value.toDouble() ) );
        break;
    }
    return true;
}

QVariant FaultMatrixModel::headerData( int section,
                          Qt::Orientation orientation, int role) const
{
    Q_UNUSED( orientation );

    switch( role )
    {
    case Qt::DisplayRole:
        return QString::number( section );
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
        break;
    }
    return QVariant();
}

Qt::ItemFlags FaultMatrixModel::flags( const QModelIndex &index ) const
{
    Q_UNUSED( index )

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void FaultMatrixModel::setStatesMatrix(
        const vector< vector< double > > & value  )
{
    clear();
    m_matrix_size = (int)value.size();

    for ( size_t i = 0; i < value.size(); i ++ )
    {
        for ( size_t j = 0; j < value[ i ].size(); j  ++ )
        {
            setItem( (int)i, (int)j, new QStandardItem(
                         QString::number( value[ (int)i ][ (int)j ] ) ) );
        }
    }
    emit dataChanged( index(0, 0), index( rowCount() - 1, columnCount() - 1 ) );
}

vector< vector< double > > FaultMatrixModel::getStatesMatrix()
{
    vector< vector< double > >  t_matrix;
    for ( int i = 0; i < m_matrix_size; i ++ )
    {
        vector< double > t_matrix_row;
        for ( int j = 0; j < m_matrix_size; j  ++ )
        {
            t_matrix_row.push_back( item( i, j )->text().toDouble() );
        }
        t_matrix.push_back( t_matrix_row );
    }

    return t_matrix;
}

void FaultMatrixModel::resizeStates( int size )
{
    long oldsize = m_matrix_size;
    m_matrix_size = size;
    int m_large_size = (size < oldsize)?oldsize:size;
    for ( int i = 0; i < m_large_size; i ++ )
    {
        for ( int j = 0; j < m_large_size; j ++ )
        {
            if ( item( i, j ) == 0 )
            {
                if ( j == 0 )
                {
                    setItem( i, j, new QStandardItem( "1" ) );
                }
                else
                {
                    setItem( i, j, new QStandardItem( "0" ) );
                }
            }
            if ( i >= size || j >= size )
            {
                delete item( i, j );
            }
        }
    }
    if (oldsize > size)
    {
        removeRows( size, oldsize - size );
        removeColumns( size, oldsize - size );
    }

    emit dataChanged( index(0, 0), index( rowCount() -1, columnCount() -1 ) );
}

void FaultMatrixModel::setTransferProb( int current, int next, double prob )
{
    if ( item( current, next ) )
    {
        item( current, next )->setText( QString::number( prob ) );
    }
}

FaultStateTableView::FaultStateTableView(QWidget *parent) :
    QTableView( parent )
{
    horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    setAlternatingRowColors( true );
}

FaultMatrixTableView::FaultMatrixTableView(QWidget *parent) :
    QTableView( parent )
{
    horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
    verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    setAlternatingRowColors( true );
}
