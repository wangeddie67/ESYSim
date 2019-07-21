#include "network_cfg_tree.h"

QStringList NetworkCfgModel::const_networkcfg_view_list = QStringList() <<
    "Topology" << "Size" << "Data Path Width" << "Number of ports" << "Port";

QStringList NetworkCfgModel::const_routercfg_view_list = QStringList() <<
    "Position" << "Pipeline" << "Number of ports" << "Port";

QStringList NetworkCfgModel::const_portcfg_view_list = QStringList() <<
	"Num. of Input VCs" << "Num. of Output VCs" << "Direction" <<
	"Network Interface" << "Input Buffer Size" << "Output Buffer Size" <<
	"Neighbor ID" << "Neighbor Port";

QString NetworkCfgModel::const_portcfg_title = "Port Configuration";

QStringList NetworkCfgModel::const_viewheader_list = QStringList() <<
    "Name" << "Value";

void NetworkCfgModel::setNetworkCfg( EsyNetCfg *cfg )
{
    clear();

    m_type = NETWORKCFG_NETWORK;
    mp_network_cfg = cfg;
    mp_router_cfg = 0;

    appendRow( QList< QStandardItem * >() <<
        new QStandardItem(
            const_networkcfg_view_list[ NETWORKCFG_TOPOLOGY ] ) <<
        new QStandardItem( QString::fromStdString(
            EsyNetCfg::nocTopologyStrVector(
                mp_network_cfg->topology() ) ) ) );
    setSizeCfg( NETWORKCFG_SIZE );
    appendRow( QList< QStandardItem * >() <<
        new QStandardItem(
            const_networkcfg_view_list[ NETWORKCFG_DATA_WIDTH ] )<<
        new QStandardItem(
            QString::number( mp_network_cfg->dataPathWidth() ) ) );
    appendRow( QList< QStandardItem * >() <<
        new QStandardItem(
            const_networkcfg_view_list[ NETWORKCFG_PORTNUM ] )<<
		new QStandardItem( QString::number(
			mp_network_cfg->templateRouter().portNum() ) ) );
    setPortCfg( NETWORKCFG_PORT_START );
    emit dataChanged( item( 0, NETWORKCFG_ARGUMENT )->index(),
		item( NETWORKCFG_PORT_START +
			mp_network_cfg->templateRouter().portNum() - 1,
            NETWORKCFG_VALUE )->index() );
}

void NetworkCfgModel::setRouterCfg( EsyNetCfgRouter *cfg )
{
    clear();

    m_type = NETWORKCFG_ROUTER;
    mp_router_cfg = cfg;
    mp_network_cfg = 0;

    setSizeCfg( ROUTERCFG_POSITION );
    appendRow( QList< QStandardItem * >() <<
        new QStandardItem( const_routercfg_view_list[ ROUTERCFG_PIPELINE ] )
        << new QStandardItem(
            QString::number( mp_router_cfg->pipeCycle() ) ) );
    appendRow( QList< QStandardItem * >() <<
        new QStandardItem( const_routercfg_view_list[ ROUTERCFG_PORTNUM ] )
        << new QStandardItem(
            QString::number( mp_router_cfg->portNum() ) ) );
    setPortCfg( ROUTERCFG_PORT_START );
    emit dataChanged( item( 0, NETWORKCFG_ARGUMENT )->index(),
        item( ROUTERCFG_PORT_START + mp_router_cfg->portNum() - 1,
            NETWORKCFG_VALUE )->index() );
}

bool NetworkCfgModel::isValid() const
{
    if ( ( mp_network_cfg != 0x00 && m_type == NETWORKCFG_NETWORK ) ||
         ( mp_router_cfg != 0x00 && m_type == NETWORKCFG_ROUTER ) )
    {
        return true;
    }
    return false;
}

void NetworkCfgModel::setSizeCfg( int row )
{
    if ( !isValid() )
    {
        return;
    }

    QStandardItem * size_item = item( row );
    if ( size_item == 0 )
    {
        if ( m_type == NETWORKCFG_NETWORK )
        {
            size_item = new QStandardItem(
                        const_networkcfg_view_list[ NETWORKCFG_SIZE ] );
        }
        else
        {
            size_item = new QStandardItem(
                        const_routercfg_view_list[ ROUTERCFG_POSITION ] );
        }
        appendRow( QList< QStandardItem * >() <<
                   size_item << new QStandardItem() );
    }
    int row_t = size_item->row();

    if ( m_type == NETWORKCFG_NETWORK )
    {
        if ( mp_network_cfg->dim() > 1 )
        {
            item( row_t, NETWORKCFG_VALUE )->setText( "" );
            for ( int i = 0; i < AXISCFG_ROW_COUNT ; i ++ )
            {
                if ( size_item->child( i ) == 0 )
                {
                    size_item->appendRow( QList< QStandardItem * >() <<
                        new QStandardItem( QString::fromStdString(
                            EsyNetCfg::nocAxisStrVector( i ) ) ) <<
                        new QStandardItem() );
                }

                long t_str;
                switch( i )
                {
                case 0: t_str = mp_network_cfg->size( AXISCFG_X ); break;
                case 1: t_str = mp_network_cfg->size( AXISCFG_Y ); break;
                }

                size_item->child( i, NETWORKCFG_VALUE )->setText(
                    QString::number( t_str ) );
            }
        }
        else
        {
            while ( size_item->child( 0, 0 ) )
            {
                QList< QStandardItem *> t_item = size_item->takeRow( 0 );
                for ( int i = 0; i < t_item.size(); i ++ )
                {
                    delete t_item[ i ];
                }
            }
            item( row_t, NETWORKCFG_VALUE )->setText(
                QString::number( mp_network_cfg->size( 0 ) ) );
        }
    }
    else
    {
        item( row_t, NETWORKCFG_VALUE )->setText( "" );
        for ( int i = 0; i < AXISCFG_ROW_COUNT ; i ++ )
        {
            if ( size_item->child( i ) == 0 )
            {
                size_item->appendRow( QList< QStandardItem * >() <<
                    new QStandardItem( QString::fromStdString(
                        EsyNetCfg::nocAxisStrVector( i ) ) ) <<
                    new QStandardItem() );
            }

            double t_val;
            switch( i )
            {
            case AXISCFG_X: t_val = mp_router_cfg->pos().first; break;
            case AXISCFG_Y: t_val = mp_router_cfg->pos().second; break;
            }

            size_item->child( i, NETWORKCFG_VALUE )->setText(
                QString::number( t_val ) );
        }
    }
}

void NetworkCfgModel::setPortCfg( int start_row )
{
    if ( !isValid() )
    {
        return;
    }

    long port_num = 0;
    long port_item_num = 0;
    if ( m_type == NETWORKCFG_NETWORK )
    {
		port_num = mp_network_cfg->templateRouter().portNum();
        port_item_num = PORTCFG_ROW_COUNT - 2;
    }
    else
    {
        port_num = mp_router_cfg->portNum();
        port_item_num = PORTCFG_ROW_COUNT;
    }

    for ( long port_index = 0; port_index < port_num; port_index ++ )
    {
        QStandardItem * port_item = item( port_index + start_row );
        if ( port_item == 0 )
        {
            port_item = new QStandardItem( const_portcfg_title );
            appendRow( QList< QStandardItem * >() << port_item <<
                new QStandardItem( QString::number( port_index ) ) );
            for ( long i = 0; i < port_item_num; i ++ )
            {
                port_item->appendRow( QList< QStandardItem * >() <<
                    new QStandardItem( const_portcfg_view_list[ i ] ) <<
                    new QStandardItem );
            }
        }

        EsyNetCfgPort port_cfg;
        if ( m_type == NETWORKCFG_NETWORK )
        {
			port_cfg = mp_network_cfg->templateRouter().port( port_index );
        }
        else
        {
            port_cfg = mp_router_cfg->port( port_index );
        }

        for ( int i = 0; i < port_item_num; i ++ )
        {
            QString t_str;
            switch ( i )
            {
			case PORTCFG_INPUT_VCNUM: t_str = QString::number(
					 port_cfg.inputVcNumber() ); break;
			case PORTCFG_OUTPUT_VCNUM: t_str = QString::number(
					 port_cfg.outputVcNumber() ); break;
			case PORTCFG_DIRECTION: t_str = QString::fromStdString(
                     port_cfg.portDirectionStr() ); break;
            case PORTCFG_NI: t_str = ""; break;
            case PORTCFG_INPUTBUFFER: t_str = QString::number(
                     port_cfg.inputBuffer() ); break;
            case PORTCFG_OUTPUTBUFFER: t_str = QString::number(
                     port_cfg.outputBuffer() ); break;
            case PORTCFG_NEIGHBORID: t_str = QString::number(
                     port_cfg.neighborRouter() ); break;
            case PORTCFG_NEIGHBORPORT: t_str = QString::number(
                     port_cfg.neighborPort() ); break;
            }
            port_item->child( i, NETWORKCFG_VALUE )->setText( t_str );
        }
    }
    while ( item( port_num + start_row ) )
    {
        QStandardItem * port_item = item( port_num + start_row );
        while ( port_item->child( 0 ) )
        {
            QList< QStandardItem *> t_item = port_item->takeRow( 0 );
            for ( int i = 0; i < t_item.size(); i ++ )
            {
                delete t_item[ i ];
            }
        }
        QList< QStandardItem *> t_item = takeRow( port_num + start_row );
        for ( int i = 0; i < t_item.size(); i ++ )
        {
            delete t_item[ i ];
        }
    }
}

int NetworkCfgModel::rowCount( const QModelIndex &parent ) const
{
    if ( !isValid() )
    {
        return 0;
    }

    if ( parent == QModelIndex() )
    {
        if ( m_type == NETWORKCFG_NETWORK )
        {
			return NETWORKCFG_PORT_START +
				mp_network_cfg->templateRouter().portNum();
        }
        else
        {
            return ROUTERCFG_PORT_START + mp_router_cfg->portNum();
        }
    }
    else if ( parent.row() == NETWORKCFG_SIZE &&
              m_type == NETWORKCFG_NETWORK )
    {
        if ( mp_network_cfg->topology() ==
             EsyNetCfg::NOC_TOPOLOGY_2DMESH ||
             mp_network_cfg->topology() ==
             EsyNetCfg::NOC_TOPOLOGY_2DTORUS )
        {
            return AXISCFG_ROW_COUNT;
        }
        else
        {
            return 0;
        }
    }
    else if ( parent.row() == ROUTERCFG_POSITION &&
              m_type == NETWORKCFG_ROUTER )
    {
        return AXISCFG_ROW_COUNT;
    }
    else if ( parent.row() >= NETWORKCFG_PORT_START &&
              m_type == NETWORKCFG_NETWORK )
    {
        return PORTCFG_ROW_COUNT - 2;
    }
    else if ( parent.row() >= ROUTERCFG_PORT_START &&
              m_type == NETWORKCFG_ROUTER )
    {
        return PORTCFG_ROW_COUNT;
    }
    else
    {
        return 0;
    }
}

int NetworkCfgModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );
    if ( !isValid() )
    {
        return 0;
    }
    return NETWORKCFG_COLUMN_COUNT;
}

QVariant NetworkCfgModel::data( const QModelIndex &index, int role ) const
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
    case Qt::EditRole:
        {
            return itemFromIndex( index )->text();
            break;
        }
    case Qt::CheckStateRole:
        {
            if ( m_type == NETWORKCFG_NETWORK )
            {
                if ( col == NETWORKCFG_VALUE && row == PORTCFG_NI &&
                     index.parent().row() >= NETWORKCFG_PORT_START )
                {
					if ( mp_network_cfg->templateRouter().port(
                        index.parent().row() - NETWORKCFG_PORT_START ).
                        networkInterface() )
                    {
                        return Qt::Checked;
                    }
                    else
                    {
                        return Qt::Unchecked;
                    }
                }
            }
            else
            {
                if ( col == NETWORKCFG_VALUE && row == PORTCFG_NI &&
                     index.parent().row() >= ROUTERCFG_PORT_START )
                {
                    if ( mp_router_cfg->port(
                        index.parent().row() - ROUTERCFG_PORT_START ).
                        networkInterface() )
                    {
                        return Qt::Checked;
                    }
                    else
                    {
                        return Qt::Unchecked;
                    }
                }
            }
        }
    }
    return QVariant();
}

QVariant NetworkCfgModel::headerData( int section,
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

bool NetworkCfgModel::setData( const QModelIndex &index,
                               const QVariant &value, int role)
{
    if ( !isValid() )
    {
        return false;
    }

    int row = index.row();
    int col = index.column();

    switch( role )
    {
    case Qt::EditRole:
        {
            itemFromIndex( index )->setText( QString::number( value.toInt() ) );

            if ( col == NETWORKCFG_VALUE )
            {
                if ( index.parent() == QModelIndex() )
                {
                    if ( row < NETWORKCFG_PORT_START &&
                         m_type == NETWORKCFG_NETWORK )
                    {
                        switch( row )
                        {
                        case NETWORKCFG_TOPOLOGY :
                            itemFromIndex( index )->setText(
                                QString::fromStdString(
                                    EsyNetCfg::nocTopologyStrVector(
                                        value.toInt() ) ) );
                            mp_network_cfg->setTopology(
                                (EsyNetCfg::NoCTopology)
                                    value.toInt() );
                            setSizeCfg( NETWORKCFG_SIZE );
                            break;
                        case NETWORKCFG_SIZE :
                            mp_network_cfg->setSize( 0,
                                value.toString().toInt() );
                            break;
                        case NETWORKCFG_DATA_WIDTH:
                            mp_network_cfg->setDataPathWidth( value.toInt() );
                            break;
                        case NETWORKCFG_PORTNUM :
							mp_network_cfg->templateRouter().
								resizePort( value.toInt() );
                            setPortCfg( NETWORKCFG_PORT_START );
                            break;
                        }
                    }
                    else if ( row < ROUTERCFG_PORT_START &&
                         m_type == NETWORKCFG_ROUTER )
                    {
                        switch( row )
                        {
                        case ROUTERCFG_PIPELINE :
                            itemFromIndex( index )->setText(
                                QString::number( value.toDouble() ) );
                            mp_router_cfg->setPipeCycle( value.toDouble() );
                            break;
                        case ROUTERCFG_PORTNUM :
                            mp_router_cfg->resizePort( value.toInt() );
                            setPortCfg( ROUTERCFG_PORT_START );
                            break;
                        }
                    }
                }
                else if ( index.parent().parent() == QModelIndex() )
                {
                    if ( index.parent().row() == NETWORKCFG_SIZE &&
                         m_type == NETWORKCFG_NETWORK )
                    {
                        mp_network_cfg->setSize( row, value.toInt() );
                    }
                    if ( index.parent().row() == ROUTERCFG_POSITION &&
                         m_type == NETWORKCFG_ROUTER )
                    {
                        itemFromIndex( index )->setText(
                            QString::number( value.toDouble() ) );
                        switch ( row )
                        {
                        case 0: mp_router_cfg->setPosFirst( value.toDouble() );
                            break;
                        case 1: mp_router_cfg->setPosSecond( value.toDouble() );
                            break;
                        }
                    }
                    else if ( ( index.parent().row() >= NETWORKCFG_PORT_START
                                && m_type == NETWORKCFG_NETWORK ) ||
                              ( index.parent().row() >= ROUTERCFG_PORT_START
                                && m_type == NETWORKCFG_ROUTER ) )
                    {
                        EsyNetCfgPort * port_cfg;
                        if ( m_type == NETWORKCFG_NETWORK )
                        {
							port_cfg = &( mp_network_cfg->templateRouter().port(
                                index.parent().row() - NETWORKCFG_PORT_START ) );
                        }
                        else
                        {
                            port_cfg = &( mp_router_cfg->port(
                                index.parent().row() - ROUTERCFG_PORT_START ) );
                        }
                        switch( row )
                        {
						case PORTCFG_INPUT_VCNUM :
							port_cfg->setInputVcNumber( value.toInt() ); break;
						case PORTCFG_OUTPUT_VCNUM :
							port_cfg->setOutputVcNumber( value.toInt() ); break;
						case PORTCFG_DIRECTION :
                            port_cfg->setPortDirection(
                                (EsyNetCfgPort::RouterPortDirection)
                                    value.toInt() );
                            itemFromIndex( index )->setText(
                                QString::fromStdString(
                                    port_cfg->portDirectionStr() ) );
                             break;
                        case PORTCFG_NI : break;
                        case PORTCFG_INPUTBUFFER:
                            port_cfg->setInputBuffer( value.toInt() ); break;
                        case PORTCFG_OUTPUTBUFFER:
                            port_cfg->setOutputBuffer( value.toInt() ); break;
                        case PORTCFG_NEIGHBORID:
                            port_cfg->setNeighborRouter( value.toInt() ); break;
                        case PORTCFG_NEIGHBORPORT:
                            port_cfg->setNeighborPort( value.toInt() ); break;
                        }
                    }
                }

                if ( m_type == NETWORKCFG_NETWORK )
                {
                    emit dataChanged(
                        invisibleRootItem()->child( 0,
                            NETWORKCFG_ARGUMENT )->index(),
                        invisibleRootItem()->child( NETWORKCFG_PORT_START +
							mp_network_cfg->templateRouter().portNum() - 1,
                            NETWORKCFG_VALUE )->index() );
                }
                else
                {
                    emit dataChanged(
                        invisibleRootItem()->child( 0,
                            NETWORKCFG_ARGUMENT )->index(),
                        invisibleRootItem()->child( ROUTERCFG_PORT_START +
                            mp_router_cfg->portNum() - 1,
                            NETWORKCFG_VALUE )->index() );
                }
                emit updateScene();
                return true;
            }
            return false;
        }
    case Qt::CheckStateRole:
        {
            if ( col == NETWORKCFG_VALUE && row == PORTCFG_NI &&
                 ( ( index.parent().row() >= NETWORKCFG_PORT_START &&
                     m_type == NETWORKCFG_NETWORK ) ||
                   ( index.parent().row() >= ROUTERCFG_PORT_START &&
                     m_type == NETWORKCFG_ROUTER ) ) )
            {
                itemFromIndex( index )->setCheckState(
                    (Qt::CheckState)value.toInt() );
                if ( m_type == NETWORKCFG_NETWORK )
                {
					mp_network_cfg->templateRouter().port(
                        index.parent().row() - NETWORKCFG_PORT_START ).
                        setNetworkInterface( value.toInt() );
                }
                else
                {
                    mp_router_cfg->port(
                        index.parent().row() - ROUTERCFG_PORT_START ).
                        setNetworkInterface( value.toInt() );
                }
                emit dataChanged( index, index );
                emit updateScene();
                return true;
            }
            return false;
        }
    }

    return false;
}

Qt::ItemFlags NetworkCfgModel::flags( const QModelIndex &index ) const
{
    if ( !isValid() )
    {
        return 0;
    }

    if ( !index.isValid() )
    {
        return 0;
    }

    if ( index.column() == NETWORKCFG_ARGUMENT )
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else if ( index.parent() == QModelIndex() &&
              item( index.row(), 0 )->hasChildren() )
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else if ( ( ( index.parent().row() >= NETWORKCFG_PORT_START &&
                  m_type == NETWORKCFG_NETWORK ) ||
                ( index.parent().row() >= ROUTERCFG_PORT_START &&
                  m_type == NETWORKCFG_ROUTER ) ) &&
              index.row() == PORTCFG_NI )
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable |
               Qt::ItemIsUserCheckable;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
}

QWidget * NetworkCfgDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED( option )

    if ( index.column() == NetworkCfgModel::NETWORKCFG_VALUE )
    {
        if ( index.parent() == QModelIndex() )
        {
            if ( index.row() == NetworkCfgModel::NETWORKCFG_TOPOLOGY &&
                 m_type == NetworkCfgModel::NETWORKCFG_NETWORK )
            {
                QComboBox * box = new QComboBox( parent );
                box->setFrame( false );
                for ( int i = 0; i < EsyNetCfg::NOC_TOPOLOGY_COUNT;
                      i ++ )
                {
                    box->addItem( QString::fromStdString(
                         EsyNetCfg::nocTopologyStrVector( i ) ) );
                }
                return box;
            }
            else if ( index.row() == NetworkCfgModel::ROUTERCFG_PIPELINE &&
                      m_type == NetworkCfgModel::NETWORKCFG_ROUTER )
            {
                NumLineEdit *box = new NumLineEdit( NumLineEdit::DOUBLE_NUM,
                                                    "", parent );
                box->setFrame( false );
                return box;
            }
            else
            {
                QSpinBox * box = new QSpinBox( parent );
                box->setFrame( false );
				box->setMinimum( 0 );
                return box;
            }
        }
        else if ( index.parent().parent() == QModelIndex() )
        {
            if ( ( ( index.parent().row() >=
                     NetworkCfgModel::NETWORKCFG_PORT_START &&
                     m_type == NetworkCfgModel::NETWORKCFG_NETWORK ) ||
                   ( index.parent().row() >=
                     NetworkCfgModel::ROUTERCFG_PORT_START &&
                     m_type == NetworkCfgModel::NETWORKCFG_ROUTER ) ) &&
                 index.row() == NetworkCfgModel::PORTCFG_DIRECTION )
            {
                QComboBox * box = new QComboBox( parent );
                box->setFrame( false );
                for ( int i = 0; i < EsyNetCfgPort::ROUTER_PORT_NUM; i ++ )
                {
                    box->addItem( QString::fromStdString(
                        EsyNetCfgPort::portDirectionStrVector( i ) ) );
                }

                return box;
            }
            else if ( ( ( index.parent().row() >=
                          NetworkCfgModel::NETWORKCFG_PORT_START &&
                          m_type == NetworkCfgModel::NETWORKCFG_NETWORK ) ||
                        ( index.parent().row() >=
                          NetworkCfgModel::ROUTERCFG_PORT_START &&
                          m_type == NetworkCfgModel::NETWORKCFG_ROUTER ) ) &&
                      index.row() == NetworkCfgModel::PORTCFG_NI )
            {
                return 0;
            }
            else if ( index.parent().row() ==
                      NetworkCfgModel::ROUTERCFG_POSITION &&
                      m_type == NetworkCfgModel::NETWORKCFG_ROUTER )
            {
                NumLineEdit *box = new NumLineEdit( NumLineEdit::DOUBLE_NUM,
                                                    "", parent );
                box->setFrame( false );
                return box;
            }
            else
            {
                QSpinBox * box = new QSpinBox( parent );
                box->setFrame( false );
				box->setMinimum( 0 );
                return box;
            }
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

void NetworkCfgDelegate::setEditorData( QWidget *editor,
                                        const QModelIndex &index ) const
{
    QString t_str = index.model()->data(index, Qt::DisplayRole).toString();

    if ( index.column() == NetworkCfgModel::NETWORKCFG_VALUE )
    {
        if ( index.parent() == QModelIndex() )
        {
            if ( index.row() == NetworkCfgModel::NETWORKCFG_TOPOLOGY &&
                 m_type == NetworkCfgModel::NETWORKCFG_NETWORK )
            {
                QComboBox *box = (QComboBox *)(editor);
                box->setCurrentText( t_str );
            }
            else if ( index.row() == NetworkCfgModel::ROUTERCFG_PIPELINE &&
                      m_type == NetworkCfgModel::NETWORKCFG_ROUTER )
            {
                NumLineEdit *box = (NumLineEdit *)(editor);
                box->setText( t_str );
            }
            else
            {
                QSpinBox * box = (QSpinBox *)(editor);
                box->setValue( t_str.toInt() );
            }
        }
        else if ( index.parent().parent() == QModelIndex() )
        {
            if ( ( ( index.parent().row() >=
                     NetworkCfgModel::NETWORKCFG_PORT_START &&
                     m_type == NetworkCfgModel::NETWORKCFG_NETWORK ) ||
                   ( index.parent().row() >=
                     NetworkCfgModel::ROUTERCFG_PORT_START &&
                     m_type == NetworkCfgModel::NETWORKCFG_ROUTER ) ) &&
                 index.row() == NetworkCfgModel::PORTCFG_DIRECTION )
            {
                QComboBox *box = (QComboBox *)(editor);
                box->setCurrentText( t_str );
            }
            else if ( ( ( index.parent().row() >=
                          NetworkCfgModel::NETWORKCFG_PORT_START &&
                          m_type == NetworkCfgModel::NETWORKCFG_NETWORK ) ||
                        ( index.parent().row() >=
                          NetworkCfgModel::ROUTERCFG_PORT_START &&
                          m_type == NetworkCfgModel::NETWORKCFG_ROUTER ) ) &&
                      index.row() == NetworkCfgModel::PORTCFG_NI )
            {
            }
            else if ( index.parent().row() ==
                      NetworkCfgModel::ROUTERCFG_POSITION &&
                      m_type == NetworkCfgModel::NETWORKCFG_ROUTER )
            {
                NumLineEdit *box = (NumLineEdit *)(editor);
                box->setText( t_str );
            }
            else
            {
                QSpinBox * box = (QSpinBox *)(editor);
                box->setValue( t_str.toInt() );
            }
        }
    }
}

void NetworkCfgDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant t_val;

    if ( index.column() == NetworkCfgModel::NETWORKCFG_VALUE )
    {
        if ( index.parent() == QModelIndex() )
        {
            if ( index.row() == NetworkCfgModel::NETWORKCFG_TOPOLOGY &&
                 m_type == NetworkCfgModel::NETWORKCFG_NETWORK )
            {
                QComboBox *box = (QComboBox *)(editor);
                t_val = box->currentIndex();
            }
            else if ( index.row() == NetworkCfgModel::ROUTERCFG_PIPELINE &&
                      m_type == NetworkCfgModel::NETWORKCFG_ROUTER )
            {
                NumLineEdit *box = (NumLineEdit *)(editor);
                t_val = box->text();
            }
            else
            {
                QSpinBox * box = (QSpinBox *)(editor);
                t_val = box->value();
            }
        }
        else if ( index.parent().parent() == QModelIndex() )
        {
            if ( ( ( index.parent().row() >=
                     NetworkCfgModel::NETWORKCFG_PORT_START &&
                     m_type == NetworkCfgModel::NETWORKCFG_NETWORK ) ||
                   ( index.parent().row() >=
                     NetworkCfgModel::ROUTERCFG_PORT_START &&
                     m_type == NetworkCfgModel::NETWORKCFG_ROUTER ) ) &&
                 index.row() == NetworkCfgModel::PORTCFG_DIRECTION )
            {
                QComboBox *box = (QComboBox *)(editor);
                t_val = box->currentIndex();
            }
            else if ( ( ( index.parent().row() >=
                          NetworkCfgModel::NETWORKCFG_PORT_START &&
                          m_type == NetworkCfgModel::NETWORKCFG_NETWORK ) ||
                        ( index.parent().row() >=
                          NetworkCfgModel::ROUTERCFG_PORT_START &&
                          m_type == NetworkCfgModel::NETWORKCFG_ROUTER ) ) &&
                      index.row() == NetworkCfgModel::PORTCFG_NI )
            {
            }
            else if ( index.parent().row() ==
                      NetworkCfgModel::ROUTERCFG_POSITION &&
                      m_type == NetworkCfgModel::NETWORKCFG_ROUTER )
            {
                NumLineEdit *box = (NumLineEdit *)(editor);
                t_val = box->text();
            }
            else
            {
                QSpinBox * box = (QSpinBox *)(editor);
                t_val = box->value();
            }
        }
    }

    model->setData( index, t_val, Qt::EditRole );
}

void NetworkCfgDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED( index )
    editor->setGeometry(option.rect);
}

QSize NetworkCfgDelegate::sizeHint( const QStyleOptionViewItem &option,
                                    const QModelIndex &index ) const
{
    Q_UNUSED( option )
    Q_UNUSED( index )

    return QSize( 100, 25 );
}

NetworkCfgTreeView::NetworkCfgTreeView( QWidget * parent ) :
    QTreeView( parent )
{
    setAlternatingRowColors( true );
    header()->setSectionResizeMode( QHeaderView::Interactive );
}
