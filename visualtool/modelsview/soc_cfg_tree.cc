#include "soc_cfg_tree.h"

QStringList SoCCfgModel::const_tilecfg_l1_view_list = QStringList() <<
	"Tile ID" << "Location" << "Features" << "Predictor" <<
    "Register File" << "Cache" << "Memory" << "Execution Unit" << "ASIC";
QStringList SoCCfgModel::const_routercfg_l1_view_list = QStringList() <<
	"Router ID" << "Location" << "Features";
QStringList SoCCfgModel::const_globalcfg_l1_view_list = QStringList() <<
	"Topology" << "Tile Features" << "Predictor" << "Register File" <<
    "Cache" << "Memory" << "Execution Unit" << "ASIC" << "Router Features";

QStringList SoCCfgModel::const_soccfg_view_list = QStringList() <<
	"X on Screen" << "Y on Screen" << "ID of NI" <<
	"Physical Port"
	<<
	"Tile Type" << "Tile Enable" << "Tile Frequency" << "ISA Type" <<
	"Application" << "Max. Instruction" << "Speed of Fetch" <<
	"ID of MPI" << "Size of Meta" << "Size of Return Stack" <<
	"BTB Set" << "BTB is Combination" << "Position of NI"
	<<
	"Undate Type" << "Redict Type" << "Size of BIMOD" << "Size of L1" <<
	"Size of L2" << "Size of L2 Hist" << "Enable L2 XOR"
	<<
	"First Chunk Delay" << "Neiber Chunk Delay" << "Instruction TLB Config." <<
	"Data TLB Config." << "TLB Missing Delay" << "Width of Memroy Bus"
	<<
	"Size of IFQ" << "Branch Penaly" << "Decoder Width" << "Issue Width" <<
	"Inorder Issue" << "Include Spec." << "Commit Width" << "RUU Size" <<
	"LSQ Size"
	<<
	"Data L1 Config." << "Data L2 Delay" << "Data L2 Config." << "Data L2 Delay" <<
	"Inst. L1 Config." << "Inst. L1 Delay" << "Inst. L2 Config." <<
	"Inst. L2 Delay" << "Enable Inst. Compress"
	<<
	"Integar Adder" << "integar Multiplexer" << "Float Adder" <<
	"Float Multiplexer" << "Memory Port"
	<<
	"Pipeline" << "Number of ports"
	<<
	"Number of Input VCs" << "Number of Output VCs" << "Direction" <<
	"Network Interface" << "Input Buffer Size" << "Output Buffer Size" <<
	"Neighbor ID" << "Neighbor Port"
	<<
	"X on Screen" << "Y on Screen"
	<<
    "Topology" << "Size X" << "Size Y" << "Network Frequency" << "Data Path Width"
    <<
    "ASIC Name" << "ASIC Type" << "ASIC Delay";

QString SoCCfgModel::const_routercfg_port_title = "Port Configuration";


QStringList SoCCfgModel::const_viewheader_list = QStringList() <<
	"Name" << "Value";

QStandardItem * SoCCfgModel::appendItem(
	long code, const QString &value, QStandardItem *parent )
{
	QStandardItem * name_item = new QStandardItem( const_soccfg_view_list[ code ] );
	if ( parent == 0 )
	{
		appendRow( QList< QStandardItem * >() <<
			name_item << new SoCCfgModelItem( code, value ) );
	}
	else
	{
		parent->appendRow( QList< QStandardItem * >() <<
			name_item << new SoCCfgModelItem( code, value ) );
	}
	return name_item;
}

#define appendItemDigital( code, value, parent ) \
	appendItem( code, QString::number( value ), parent )
#define appendItemString( code, value, parent ) \
	appendItem( code, QString::fromStdString( value ), parent )

void SoCCfgModel::setTileCfg( EsySoCCfgTile *cfg )
{
	clear();

	m_type = SoCCfgModel::SOCCFG_TILE;
	mp_tile_cfg = cfg;
	mp_network_cfg = 0;
	mp_router_cfg = 0;

	setConfigItems( m_type );
}

void SoCCfgModel::setRouterCfg( EsyNetCfgRouter *cfg )
{
	clear();

	m_type = SOCCFG_ROUTER;
	mp_router_cfg = cfg;
	mp_network_cfg = 0;
	mp_tile_cfg = 0;

	setConfigItems( m_type );
}

void SoCCfgModel::setGlobalCfg( EsySoCCfg *cfg )
{
	clear();

	m_type = SoCCfgModel::SOCCFG_GLOBAL;
	mp_tile_cfg = &( cfg->templateTile() );
	mp_network_cfg = &( cfg->network() );
	mp_router_cfg = &( cfg->network().templateRouter() );

	setConfigItems( m_type );
}

void SoCCfgModel::setConfigItems( SoCCfgType type )
{
	if ( !isValid() )
	{
		return;
	}

	// insert l1 item
	long l1_count = 0;
	QStringList l1_arguments;
	switch( type )
	{
	case SOCCFG_TILE:
		l1_count = TILECFG_L1_COUNT;
		l1_arguments = const_tilecfg_l1_view_list;
		break;
	case SOCCFG_ROUTER:
		l1_count = ROUTERCFG_PORT_START;
		l1_arguments = const_routercfg_l1_view_list;
		break;
	case SOCCFG_GLOBAL:
		l1_count = GLOBALCFG_PORT_START;
		l1_arguments = const_globalcfg_l1_view_list;
		break;
	}

	QList< QStandardItem * > l1_item_list;
	for ( int i = 0; i < l1_count; i ++ )
	{
		QStandardItem * t_argument = new QStandardItem( l1_arguments[ i ] );
		appendRow( QList< QStandardItem * >() <<
			t_argument << new QStandardItem( "" ) );
		l1_item_list.append( t_argument );
	}

	switch( type )
	{
	case SOCCFG_TILE:
		item( 0, SOCCFG_VALUE )->setText(
			QString::number( mp_tile_cfg->tileId() ) );
		break;
	case SOCCFG_ROUTER:
		item( 0, SOCCFG_VALUE )->setText(
			QString::number( mp_router_cfg->routerId() ) );
		break;
	case SOCCFG_GLOBAL: break;
	}

	QStandardItem * l1_item = 0;

	// insert topology items
	switch( type )
	{
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_TOPOLOGY ]; break;
	case SOCCFG_TILE:
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemString( TOPOLOGY_TOPOLOGY,
			mp_network_cfg->topologyStr(), l1_item );
		setSizeCfg( GLOBALCFG_TOPOLOGY, TOPOLOGY_SIZE_START, TOPOLOGY_SIZE_END );
		appendItemDigital( TOPOLOGY_FREQ,
			mp_network_cfg->networkFreq(), l1_item );
		appendItemDigital( TOPOLOGY_DATA_WIDTH,
			mp_network_cfg->dataPathWidth(), l1_item );
	}

	// insert location items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_LOCATION ]; break;
	case SOCCFG_GLOBAL:
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemDigital( TILELOC_AX_X, mp_tile_cfg->pos().first, l1_item );
		appendItemDigital( TILELOC_AX_Y, mp_tile_cfg->pos().second, l1_item );
		appendItemDigital( TILELOC_NI_ID, mp_tile_cfg->niId(), l1_item );
		appendItemDigital( TILELOC_NI_PHY, mp_tile_cfg->niPhy(), l1_item );
	}

	// insert feature items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_FEATURE ]; break;
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_TILE_FEATURE ]; break;
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemString( TILEFEA_TYPE, mp_tile_cfg->tileTypeStr(), l1_item );
		appendItem( TILEFEA_ENABLE, "", l1_item );
		appendItemDigital( TILEFEA_FREQ, mp_tile_cfg->tileFreq(), l1_item );
		appendItemString( TILEFEA_ISA_TYPE,
			mp_tile_cfg->coreIsaTypeStr(), l1_item );
		appendItemString( TILEFEA_APP_DIR, mp_tile_cfg->coreAppDir(), l1_item );
		appendItemDigital( TILEFEA_MAX_INST,
			mp_tile_cfg->coreMaxInst(), l1_item );
		appendItemDigital( TILEFEA_FETCH_SPEED,
			mp_tile_cfg->coreFetchSpeed(), l1_item );
		appendItemDigital( TILEFEA_MPI_ID, mp_tile_cfg->mpiId(), l1_item );
		appendItemDigital( TILEFEA_META_SIZE,
			mp_tile_cfg->metaSize(), l1_item );
		appendItemDigital( TILEFEA_RETURN_STACK_SIZE,
			mp_tile_cfg->returnStackSize(), l1_item );
		appendItemDigital( TILEFEA_BTB_SET, mp_tile_cfg->btbSet(), l1_item );
		appendItemDigital( TILEFEA_BTB_COMBINATION,
			mp_tile_cfg->btbCombination(), l1_item );
		appendItemString( TILEFEA_NI_POS, mp_tile_cfg->niPosStr(), l1_item );
	}

	// insert predictor items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_PREDICT ]; break;
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_PREDICT ]; break;
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemString( PREDCFG_UPDATE_TYPE,
			mp_tile_cfg->predUpdateType(), l1_item );
		appendItemString( PREDCFG_PREDICT_TYPE,
			mp_tile_cfg->predPredictType(), l1_item );
		appendItemDigital( PREDCFG_BIMOD_SIZE,
			mp_tile_cfg->predBimodSize(), l1_item );
		appendItemDigital( PREDCFG_L1_SIZE,
			mp_tile_cfg->predL1Size(), l1_item );
		appendItemDigital( PREDCFG_L2_SIZE,
			mp_tile_cfg->predL2Size(), l1_item );
		appendItemDigital( PREDCFG_L2_HIST_SIZE,
			mp_tile_cfg->predL2HistSize(), l1_item );
		appendItem( PREDCFG_L2_XOR_ENABLE, "", l1_item );
	}

	// insert memory items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_MEMORY ]; break;
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_MEMORY ]; break;
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemDigital( MEMCFG_FIRST_CHUNK_DELAY,
			mp_tile_cfg->firstChunkDelay(), l1_item );
		appendItemDigital( MEMCFG_NEIBER_CHUNK_DELAY,
			mp_tile_cfg->neiberChunkDelay(), l1_item );
		appendItemString( MEMCFG_TLB_ITLB_CFG,
			mp_tile_cfg->tlbItlbCfg(), l1_item );
		appendItemString( MEMCFG_TLB_DTLB_CFG,
			mp_tile_cfg->tlbDtlbCfg(), l1_item );
		appendItemDigital( MEMCFG_TLB_MISS_DELAY,
			mp_tile_cfg->tlbMissDelay(), l1_item );
		appendItemDigital( MEMCFG_BUS_WIDTH,
			mp_tile_cfg->memoryBusWidth(), l1_item );
	}

	// insert ruu items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_RUU ]; break;
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_RUU ]; break;
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemDigital( RUUCFG_IFQ_SIZE,
			mp_tile_cfg->ruuIfqSize(), l1_item );
		appendItemDigital( RUUCFG_BRANCH_PENALT,
			mp_tile_cfg->ruuBranchPenalt(), l1_item );
		appendItemDigital( RUUCFG_DECODER_WIDTH,
			mp_tile_cfg->ruuDecoderWidth(), l1_item );
		appendItemDigital( RUUCFG_ISSUE_WIDTH,
			mp_tile_cfg->ruuIssueWidth(), l1_item );
		appendItemDigital( RUUCFG_INORDER_ISSU,
			mp_tile_cfg->ruuInorderIssu(), l1_item );
		appendItemDigital( RUUCFG_INCLUDE_SPEC,
			mp_tile_cfg->ruuIncludeSpec(), l1_item );
		appendItemDigital( RUUCFG_COMMIT_WIDTH,
			mp_tile_cfg->ruuCommitWidth(), l1_item );
		appendItemDigital( RUUCFG_SIZE, mp_tile_cfg->ruuSize(), l1_item );
		appendItemDigital( RUUCFG_LSQ_SIZE, mp_tile_cfg->lsqSize(), l1_item );
	}

	// insert cache items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_CACHE ]; break;
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_CACHE ]; break;
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemString( CACHECFG_DL1_CFG,
			mp_tile_cfg->cacheDl1Cfg(), l1_item );
		appendItemDigital( CACHECFG_DL1_DELAY,
			mp_tile_cfg->cacheDl1Delay(), l1_item );
		appendItemString( CACHECFG_DL2_CFG,
			mp_tile_cfg->cacheDl2Cfg(), l1_item );
		appendItemDigital( CACHECFG_DL2_DELAY,
			mp_tile_cfg->cacheDl2Delay(), l1_item );
		appendItemString( CACHECFG_IL1_CFG,
			mp_tile_cfg->cacheIl1Cfg(), l1_item );
		appendItemDigital( CACHECFG_IL1_DELAY,
			mp_tile_cfg->cacheIl1Delay(), l1_item );
		appendItemString( CACHECFG_IL2_CFG,
			mp_tile_cfg->cacheIl2Cfg(), l1_item );
		appendItemDigital( CACHECFG_IL2_DELAY,
			mp_tile_cfg->cacheIl2Delay(), l1_item );
		appendItem( CACHECFG_I_COMPRESS_ENABLE, "", l1_item );
	}

	// insert execute items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_EXECUTE ]; break;
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_EXECUTE ]; break;
	case SOCCFG_ROUTER: l1_item = 0; break;
	}
	if ( l1_item != 0 )
	{
		appendItemDigital( EXECFG_INTEGAR_ADDER_UNIT,
			mp_tile_cfg->integarAdderUnit(), l1_item );
		appendItemDigital( EXECFG_INTEGAR_MULTI_UNIT,
			mp_tile_cfg->integarMultiUnit(), l1_item );
		appendItemDigital( EXECFG_FLOAT_ADDR_UNIT,
			mp_tile_cfg->floatAddrUnit(), l1_item );
		appendItemDigital( EXECFG_FLOAT_MULTI_UNIT,
			mp_tile_cfg->floatMultiUnit(), l1_item );
		appendItemDigital( EXECFG_MEMORY_PORT,
			mp_tile_cfg->memoryPort(), l1_item );
	}

	// insert location items
	switch( type )
	{
	case SOCCFG_TILE:
	case SOCCFG_GLOBAL: l1_item = 0; break;
	case SOCCFG_ROUTER: l1_item = l1_item_list[ ROUTERCFG_LOCATION ]; break;
	}
	if ( l1_item != 0 )
	{
		appendItemDigital( ROUTERLOC_AXIS_X,
			mp_router_cfg->pos().first, l1_item );
		appendItemDigital( ROUTERLOC_AXIS_Y,
			mp_router_cfg->pos().second, l1_item );
	}

	// insert feature items
	switch( type )
	{
	case SOCCFG_TILE: l1_item = 0; break;
	case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_ROUTER ]; break;
	case SOCCFG_ROUTER: l1_item = l1_item_list[ ROUTERCFG_FEATURE ]; break;
	}
	if ( l1_item != 0 )
	{
        appendItemDigital( ROUTERFEA_PIPELINE,
            mp_router_cfg->pipeCycle(), l1_item );
		appendItemDigital( ROUTERFEA_PORTNUM,
			mp_router_cfg->portNum(), l1_item );
	}

    switch( type )
    {
    case SOCCFG_TILE: l1_item = l1_item_list[ TILECFG_ASIC ]; break;
    case SOCCFG_GLOBAL: l1_item = l1_item_list[ GLOBALCFG_ASIC ]; break;
    case SOCCFG_ROUTER: l1_item = 0; break;
    }
    if ( l1_item != 0 )
    {
        appendItemString( ASICTILE_NAME,
            mp_tile_cfg->asicName(), l1_item );
        appendItemDigital( ASICTILE_TYPE,
            mp_tile_cfg->asicType(), l1_item );
        appendItemDigital( ASICTILE_DELAY,
            mp_tile_cfg->asicDelay(), l1_item );
    }

	switch( type )
	{
	case SOCCFG_TILE: break;
	case SOCCFG_GLOBAL:
		setPortCfg( GLOBALCFG_PORT_START );
		l1_count += mp_router_cfg->portNum(); break;
	case SOCCFG_ROUTER:
		setPortCfg( ROUTERCFG_PORT_START );
		l1_count += mp_router_cfg->portNum(); break;
	}
	emit dataChanged( item( 0, SOCCFG_ARGUMENT )->index(),
		item( l1_count - 1, SOCCFG_VALUE )->index() );
}

bool SoCCfgModel::isValid() const
{
	if ( ( mp_tile_cfg != 0x00 && m_type == SOCCFG_TILE ) ||
		 ( mp_router_cfg != 0x00 && m_type == SOCCFG_ROUTER ) ||
		 ( mp_network_cfg != 0x00 && m_type == SOCCFG_GLOBAL ) )
	{
		return true;
	}
	return false;
}

void SoCCfgModel::setSizeCfg( int l1_row, int start_row, int end_row )
{
	if ( !isValid() )
	{
		return;
	}

	int l2_start_row = start_row - TOPOLOGY_TOPOLOGY;

	QStandardItem * l1_item = item( l1_row );
	if ( l1_item == 0 )
	{
		return;
	}

	for ( int i = 0; i < mp_network_cfg->dim(); i ++ )
	{
		SoCCfgModelItem * t_item =
			(SoCCfgModelItem *)(l1_item->child( i + l2_start_row, SOCCFG_VALUE ));
		if ( t_item != 0 && t_item->itemCode() == start_row + i )
		{
			t_item->setText( QString::number( mp_network_cfg->size( i ) ) );
		}
		else
		{
			l1_item->insertRow( i + l2_start_row, QList< QStandardItem * >() <<
				new QStandardItem( QString::fromStdString(
					EsyNetCfg::nocAxisStrVector( i ) ) ) <<
				new SoCCfgModelItem( start_row + i, QString::number(
					mp_network_cfg->size( i ) ) ) );
		}
	}

	for ( int i = l2_start_row + mp_network_cfg->dim();
				i < l1_item->rowCount(); i ++ )
	{
		SoCCfgModelItem * t_item =
			(SoCCfgModelItem *)(l1_item->child( i, SOCCFG_VALUE ));
		if ( t_item->itemCode() >= start_row && t_item->itemCode() <= end_row )
		{
			QList< QStandardItem *> t_item_list = l1_item->takeRow( i );
			for ( int i = 0; i < t_item_list.size(); i ++ )
			{
				delete t_item_list[ i ];
			}
		}
	}
}

void SoCCfgModel::setPortCfg( int start_row )
{
	if ( !isValid() )
	{
		return;
	}

	long port_num = mp_router_cfg->portNum();
	for ( long port_index = 0; port_index < port_num; port_index ++ )
	{
		EsyNetCfgPort port_cfg = mp_router_cfg->port( port_index );

		QStandardItem * port_item = item( port_index + start_row );
		if ( port_item == 0 )
		{
			port_item = new QStandardItem( const_routercfg_port_title );
			appendRow( QList< QStandardItem * >() << port_item <<
				new QStandardItem( QString::number( port_index ) ) );
			appendItemDigital( PORTCFG_IN_VCNUM, port_cfg.inputVcNumber(),
				port_item );
			appendItemDigital( PORTCFG_OUT_VCNUM, port_cfg.outputVcNumber(),
				port_item );
			appendItemString( PORTCFG_DIRECTION,
				port_cfg.portDirectionStr(), port_item );
			appendItem( PORTCFG_NI, "", port_item );
			appendItemDigital( PORTCFG_INPUTBUFFER,
				port_cfg.inputBuffer(), port_item );
			appendItemDigital( PORTCFG_OUTPUTBUFFER,
				port_cfg.outputBuffer(), port_item );
			if ( m_type != SOCCFG_GLOBAL )
			{
				appendItemDigital( PORTCFG_NEIGHBORID,
					port_cfg.neighborRouter(), port_item );
				appendItemDigital( PORTCFG_NEIGHBORPORT,
					port_cfg.neighborPort(), port_item );
			}
		}
		else
		{
			for ( int i = 0; i < port_item->rowCount(); i ++ )
			{
				QString t_str;
				SoCCfgModelItem * t_item =
					(SoCCfgModelItem *)port_item->child( i, SOCCFG_VALUE );
				switch ( t_item->itemCode() )
				{
				case PORTCFG_IN_VCNUM: t_str = QString::number(
						 port_cfg.inputVcNumber() ); break;
				case PORTCFG_OUT_VCNUM: t_str = QString::number(
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
				t_item->setText( t_str );
			}
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

int SoCCfgModel::rowCount( const QModelIndex &parent ) const
{
	if ( !isValid() )
	{
		return 0;
	}

	if ( parent == QModelIndex() )
	{
		switch( m_type )
		{
		case SOCCFG_TILE: return TILECFG_L1_COUNT;
		case SOCCFG_ROUTER:
			return ROUTERCFG_PORT_START + mp_router_cfg->portNum();
		case SOCCFG_GLOBAL:
			return GLOBALCFG_PORT_START + mp_router_cfg->portNum();
		default: return 0;
		}
	}
	else if ( parent.row() < TILECFG_L1_COUNT &&
			  m_type == SOCCFG_TILE )
	{
		int t_row = 0;
		switch ( parent.row() )
		{
		case TILECFG_ID:       t_row = 0; break;
		case TILECFG_LOCATION: t_row = TILELOC_COUNT; break;
		case TILECFG_FEATURE:  t_row = TILEFEA_COUNT; break;
		case TILECFG_PREDICT:  t_row = PREDCFG_COUNT; break;
		case TILECFG_RUU:      t_row = RUUCFG_COUNT; break;
		case TILECFG_CACHE:    t_row = CACHECFG_COUNT; break;
		case TILECFG_MEMORY:   t_row = MEMCFG_COUNT; break;
		case TILECFG_EXECUTE:  t_row = EXECFG_COUNT; break;
        case TILECFG_ASIC:     t_row = ASICTILE_COUNT; break;
		}
		return t_row;
	}
	else if ( parent.row() < ROUTERCFG_PORT_START &&
			  m_type == SOCCFG_ROUTER )
	{
		int t_row = 0;
		switch ( parent.row() )
		{
		case ROUTERCFG_ID:       t_row = 0; break;
		case ROUTERCFG_LOCATION: t_row = ROUTERLOC_COUNT; break;
		case ROUTERCFG_FEATURE:  t_row = ROUTERFEA_COUNT; break;
		}
		return t_row;
	}
	else if ( parent.row() >= ROUTERCFG_PORT_START &&
			  m_type == SOCCFG_ROUTER )
	{
		return PORTCFG_ROW_COUNT;
	}
	else if ( parent.row() < GLOBALCFG_PORT_START &&
			  m_type == SOCCFG_GLOBAL )
	{
		int t_row = 0;
		switch ( parent.row() )
		{
		case GLOBALCFG_TOPOLOGY:
			t_row = TOPOLOGY_SIZE_START + mp_network_cfg->dim() - 1; break;
		case GLOBALCFG_TILE_FEATURE: t_row = TILEFEA_COUNT; break;
		case GLOBALCFG_PREDICT:      t_row = PREDCFG_COUNT; break;
		case GLOBALCFG_RUU:          t_row = RUUCFG_COUNT; break;
		case GLOBALCFG_CACHE:        t_row = CACHECFG_COUNT; break;
		case GLOBALCFG_MEMORY:       t_row = MEMCFG_COUNT; break;
		case GLOBALCFG_EXECUTE:      t_row = EXECFG_COUNT; break;
        case GLOBALCFG_ASIC:         t_row = ASICTILE_COUNT; break;
        case GLOBALCFG_ROUTER:       t_row = ROUTERFEA_COUNT; break;
		}
		return t_row;
	}
	else if ( parent.row() >= GLOBALCFG_PORT_START &&
			  m_type == SOCCFG_GLOBAL )
	{
		return PORTCFG_ROW_COUNT - 2;
	}
	else
	{
		return 0;
	}
}

int SoCCfgModel::columnCount( const QModelIndex &parent ) const
{
	Q_UNUSED( parent );
	if ( !isValid() )
	{
		return 0;
	}
	return SOCCFG_COLUMN_COUNT;
}

#define dataBool( var ) \
{ \
	if ( var ) \
	{ \
		return Qt::Checked; \
	} \
	else \
	{ \
		return Qt::Unchecked; \
	} \
}

QVariant SoCCfgModel::data( const QModelIndex &index, int role ) const
{
	if ( !isValid() )
	{
		return QVariant();
	}

	switch( role )
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		return itemFromIndex( index )->text();
		break;
	case Qt::CheckStateRole:
		if ( index.parent() == QModelIndex() ||
			 index.column() == SoCCfgModel::SOCCFG_ARGUMENT )
		{
			return QVariant();
		}
		switch ( ( (SoCCfgModelItem *)itemFromIndex( index ) )->itemCode() )
		{
		case TILEFEA_ENABLE:
			dataBool( mp_tile_cfg->tileEnable() ) break;
		case PREDCFG_L2_XOR_ENABLE:
			dataBool( mp_tile_cfg->predL2XorEnable() ) break;
		case CACHECFG_I_COMPRESS_ENABLE:
			dataBool( mp_tile_cfg->cacheICompressEnable() ) break;
		case PORTCFG_NI:
			if ( m_type == SOCCFG_ROUTER )
			{
				dataBool( mp_router_cfg->port(
					index.parent().row() - ROUTERCFG_PORT_START ).
					networkInterface() )
			}
			else
			{
				dataBool( mp_router_cfg->port(
					index.parent().row() - GLOBALCFG_PORT_START ).
					networkInterface() )
			}
			break;
		default: return QVariant(); break;
		}
	}
	return QVariant();
}

QVariant SoCCfgModel::headerData( int section,
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

#define setDataInt( fun ) \
{ \
	itemFromIndex( index )->setText( QString::number( value.toInt() ) ); \
	fun( value.toInt() ); \
}

#define setDataDouble( fun ) \
{ \
	itemFromIndex( index )->setText( QString::number( value.toDouble() ) ); \
	fun( value.toDouble() ); \
}

#define setDataString( fun ) \
{ \
	itemFromIndex( index )->setText( value.toString() ); \
	fun( value.toString().toStdString() ); \
}

#define setDataEnum( fun, enumtype, enumstrfun ) \
{ \
	itemFromIndex( index )->setText( QString::fromStdString( \
		enumstrfun( value.toInt() ) ) ); \
	fun( (enumtype)value.toInt() ); \
}

bool SoCCfgModel::setData( const QModelIndex &index,
	const QVariant &value, int role)
{
	if ( !isValid() || index.parent() == QModelIndex() ||
		 index.column() == SoCCfgModel::SOCCFG_ARGUMENT )
	{
		return false;
	}

	switch( role )
	{
	case Qt::EditRole:
		switch ( ( (SoCCfgModelItem *)itemFromIndex( index ) )->itemCode() )
		{
		case TILELOC_AX_X:
			setDataDouble( mp_tile_cfg->setPosFirst ) break;
		case TILELOC_AX_Y:
			setDataDouble( mp_tile_cfg->setPosSecond ) break;
		case TILELOC_NI_ID:
			setDataInt( mp_tile_cfg->setNiId ) break;
		case TILELOC_NI_PHY:
			setDataInt( mp_tile_cfg->setNiPhy ) break;
		case TILEFEA_NI_POS:
			setDataEnum( mp_tile_cfg->setNiPos, EsySoCCfgTile::NIPosition,
			EsySoCCfgTile::niPosStrVector ) break;

		case TILEFEA_TYPE:
			setDataEnum( mp_tile_cfg->setTileType, EsySoCCfgTile::TileType,
            EsySoCCfgTile::tileTypeStrVector ) break;
		case TILEFEA_FREQ:
			setDataInt( mp_tile_cfg->setTileFreq ) break;
		case TILEFEA_ISA_TYPE:
			setDataEnum( mp_tile_cfg->setCoreIsaType, EsySoCCfgTile::ISAType,
			EsySoCCfgTile::coreIsaTypeStrVector ) break;
		case TILEFEA_APP_DIR:
			setDataString( mp_tile_cfg->setCoreAppDir ) break;
		case TILEFEA_MAX_INST:
			setDataInt( mp_tile_cfg->setCoreMaxInst ) break;
		case TILEFEA_FETCH_SPEED:
			setDataInt( mp_tile_cfg->setCoreFetchSpeed ) break;
		case TILEFEA_MPI_ID:
			setDataInt( mp_tile_cfg->setMpiId ) break;
		case TILEFEA_META_SIZE:
			setDataInt( mp_tile_cfg->setMetaSize ) break;
		case TILEFEA_RETURN_STACK_SIZE:
			setDataInt( mp_tile_cfg->setReturnStackSize ) break;
		case TILEFEA_BTB_SET:
			setDataInt( mp_tile_cfg->setBtbSet ) break;
		case TILEFEA_BTB_COMBINATION:
			setDataInt( mp_tile_cfg->setBtbCombination ) break;

		case PREDCFG_UPDATE_TYPE:
			setDataString( mp_tile_cfg->setPredUpdateType ) break;
		case PREDCFG_PREDICT_TYPE:
			setDataString( mp_tile_cfg->setPredPredictType ) break;
		case PREDCFG_BIMOD_SIZE:
			setDataInt( mp_tile_cfg->setPredBimodSize ) break;
		case PREDCFG_L1_SIZE:
			setDataInt( mp_tile_cfg->setPredL1Size ) break;
		case PREDCFG_L2_SIZE:
			setDataInt( mp_tile_cfg->setPredL2Size ) break;
		case PREDCFG_L2_HIST_SIZE:
			setDataInt( mp_tile_cfg->setPredL2HistSize ) break;

		case MEMCFG_FIRST_CHUNK_DELAY:
			setDataInt( mp_tile_cfg->setFirstChunkDelay ) break;
		case MEMCFG_NEIBER_CHUNK_DELAY:
			setDataInt( mp_tile_cfg->setNeiberChunkDelay ) break;
		case MEMCFG_TLB_ITLB_CFG:
			setDataString( mp_tile_cfg->setTlbItlbCfg ) break;
		case MEMCFG_TLB_DTLB_CFG:
			setDataString( mp_tile_cfg->setTlbDtlbCfg ) break;
		case MEMCFG_TLB_MISS_DELAY:
			setDataInt( mp_tile_cfg->settlbMissDelay ) break;
		case MEMCFG_BUS_WIDTH:
			setDataInt( mp_tile_cfg->setMemoryBusWidth ) break;

		case RUUCFG_IFQ_SIZE:
			setDataInt( mp_tile_cfg->setRuuIfqSize ) break;
		case RUUCFG_BRANCH_PENALT:
			setDataInt( mp_tile_cfg->setRuuBranchPenalt ) break;
		case RUUCFG_DECODER_WIDTH:
			setDataInt( mp_tile_cfg->setRuuDecoderWidth ) break;
		case RUUCFG_ISSUE_WIDTH:
			setDataInt( mp_tile_cfg->setRuuIssueWidth ) break;
		case RUUCFG_INORDER_ISSU:
			setDataInt( mp_tile_cfg->setRuuInorderIssu ) break;
		case RUUCFG_INCLUDE_SPEC:
			setDataInt( mp_tile_cfg->setRuuIncludeSpec ) break;
		case RUUCFG_COMMIT_WIDTH:
			setDataInt( mp_tile_cfg->setRuuCommitWidth ) break;
		case RUUCFG_SIZE:
			setDataInt( mp_tile_cfg->setRuuSize ) break;
		case RUUCFG_LSQ_SIZE:
			setDataInt( mp_tile_cfg->setLsqSize ) break;

		case CACHECFG_DL1_CFG:
			setDataString( mp_tile_cfg->setCacheDl1Cfg ) break;
		case CACHECFG_DL1_DELAY:
			setDataInt( mp_tile_cfg->setCacheDl1Delay ) break;
		case CACHECFG_DL2_CFG:
			setDataString( mp_tile_cfg->setCacheDl2Cfg ) break;
		case CACHECFG_DL2_DELAY:
			setDataInt( mp_tile_cfg->setCacheDl2Delay ) break;
		case CACHECFG_IL1_CFG:
			setDataString( mp_tile_cfg->setCacheIl1Cfg ) break;
		case CACHECFG_IL1_DELAY:
			setDataInt( mp_tile_cfg->setCacheIl1Delay ) break;
		case CACHECFG_IL2_CFG:
			setDataString( mp_tile_cfg->setCacheIl2Cfg ) break;
		case CACHECFG_IL2_DELAY:
			setDataInt( mp_tile_cfg->setCacheIl2Delay ) break;

		case EXECFG_INTEGAR_ADDER_UNIT:
			setDataInt( mp_tile_cfg->setIntegarAdderUnit ) break;
		case EXECFG_INTEGAR_MULTI_UNIT:
			setDataInt( mp_tile_cfg->setIntegarMultiUnit ) break;
		case EXECFG_FLOAT_ADDR_UNIT:
			setDataInt( mp_tile_cfg->setFloatAddrUnit ) break;
		case EXECFG_FLOAT_MULTI_UNIT:
			setDataInt( mp_tile_cfg->setFloatMultiUnit ) break;
		case EXECFG_MEMORY_PORT:
			setDataInt( mp_tile_cfg->setMemoryPort ) break;

		case ROUTERFEA_PIPELINE:
			setDataDouble( mp_router_cfg->setPipeCycle ) break;
		case ROUTERFEA_PORTNUM:
			itemFromIndex( index )->setText( QString::number( value.toInt() ) );
			mp_router_cfg->resizePort( value.toInt() );
			if ( m_type == SOCCFG_ROUTER )
			{
				setPortCfg( ROUTERCFG_PORT_START );
			}
			else
			{
				setPortCfg( GLOBALCFG_PORT_START );
			}
			break;

		case ROUTERLOC_AXIS_X:
			setDataDouble( mp_router_cfg->setPosFirst ) break;
		case ROUTERLOC_AXIS_Y:
			setDataDouble( mp_router_cfg->setPosSecond ) break;

		case TOPOLOGY_TOPOLOGY:
			setDataEnum( mp_network_cfg->setTopology, EsyNetCfg::NoCTopology,
			EsyNetCfg::nocTopologyStrVector )
			setSizeCfg( GLOBALCFG_TOPOLOGY, TOPOLOGY_SIZE_START, TOPOLOGY_SIZE_END );
			break;
		case TOPOLOGY_FREQ:
			setDataInt( mp_network_cfg->setNetworkFreq ) break;
		case TOPOLOGY_DATA_WIDTH:
			setDataInt( mp_network_cfg->setDataPathWidth ) break;
		case TOPOLOGY_SIZE_START:
			itemFromIndex( index )->setText( QString::number( value.toInt() ) );
			mp_network_cfg->setSize( EsyNetCfg::AX_X, value.toInt() ); break;
		case TOPOLOGY_SIZE_END:
			itemFromIndex( index )->setText( QString::number( value.toInt() ) );
			mp_network_cfg->setSize( EsyNetCfg::AX_Y, value.toInt() ); break;

        case ASICTILE_NAME:
            setDataString( mp_tile_cfg->setAsicName ) break;
        case ASICTILE_TYPE:
            setDataInt( mp_tile_cfg->setAsicType ) break;
        case ASICTILE_DELAY:
            setDataInt( mp_tile_cfg->setAsicDelay ) break;
        }

		if ( ( m_type == SOCCFG_GLOBAL
			&& index.parent().row() >= GLOBALCFG_PORT_START ) ||
			( m_type == SOCCFG_ROUTER
			&& index.parent().row() >= ROUTERCFG_PORT_START )  )
		{
			EsyNetCfgPort * port_cfg;
			if ( m_type == SOCCFG_ROUTER )
			{
				port_cfg = &( mp_router_cfg->port(
					index.parent().row() - ROUTERCFG_PORT_START ) );
			}
			else
			{
				port_cfg = &( mp_router_cfg->port(
					index.parent().row() - GLOBALCFG_PORT_START ) );
			}

			itemFromIndex( index )->setText( QString::number( value.toInt() ) );

			switch ( ( (SoCCfgModelItem *)itemFromIndex( index ) )->itemCode() )
			{
			case PORTCFG_IN_VCNUM :
				port_cfg->setInputVcNumber( value.toInt() ); break;
			case PORTCFG_OUT_VCNUM :
				port_cfg->setOutputVcNumber( value.toInt() ); break;
			case PORTCFG_DIRECTION :
				port_cfg->setPortDirection(
					(EsyNetCfgPort::RouterPortDirection) value.toInt() );
				itemFromIndex( index )->setText(
					QString::fromStdString( port_cfg->portDirectionStr() ) );
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

		{
			long l1_count;
			switch( m_type )
			{
			case SOCCFG_TILE: l1_count = TILECFG_L1_COUNT; break;
			case SOCCFG_ROUTER: l1_count = ROUTERCFG_PORT_START +
						mp_router_cfg->portNum(); break;
			case SOCCFG_GLOBAL: l1_count = GLOBALCFG_PORT_START +
						mp_router_cfg->portNum(); break;
			}
			emit dataChanged( item( 0, SOCCFG_ARGUMENT )->index(),
				item( l1_count - 1, SOCCFG_VALUE )->index() );
		}
		emit updateScene();
		return true;
		break;
	case Qt::CheckStateRole:
		{
			itemFromIndex( index )->setCheckState( (Qt::CheckState)value.toInt() );
			switch ( ( (SoCCfgModelItem *)itemFromIndex( index ) )->itemCode() )
			{
			case TILEFEA_ENABLE:
				setDataInt( mp_tile_cfg->setTileEnable ) break;
			case PREDCFG_L2_XOR_ENABLE:
				setDataInt( mp_tile_cfg->setPredL2XorEnable ) break;
			case CACHECFG_I_COMPRESS_ENABLE:
				setDataInt( mp_tile_cfg->setCacheICompressEnable ) break;
			case PORTCFG_NI:
				if ( m_type == SOCCFG_ROUTER )
				{
					mp_router_cfg->port(
						index.parent().row() - ROUTERCFG_PORT_START ).
						setNetworkInterface( value.toInt() );
				}
				else
				{
					mp_router_cfg->port(
						index.parent().row() - GLOBALCFG_PORT_START ).
						setNetworkInterface( value.toInt() );
				}
				break;
			default: break;
			}
			emit dataChanged( index, index );
			emit updateScene();
			return true;
		}
	}

	return false;
}

Qt::ItemFlags SoCCfgModel::flags( const QModelIndex &index ) const
{
	if ( !isValid() )
	{
		return 0;
	}

	if ( index.column() == SOCCFG_ARGUMENT )
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	else if ( index.parent() == QModelIndex() &&
			  item( index.row(), 0 )->hasChildren() )
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	else
	{
		switch ( ( (SoCCfgModelItem *)itemFromIndex( index ) )->itemCode() )
		{
		case TILEFEA_ENABLE:
		case PREDCFG_L2_XOR_ENABLE:
		case CACHECFG_I_COMPRESS_ENABLE:
		case PORTCFG_NI:
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
		default:
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
		}
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

#define createComboBox( itemcount, itemfun ) \
{ \
	QComboBox * box = new QComboBox( parent ); \
	box->setFrame( false ); \
	for ( int i = 0; i < itemcount; i ++ ) \
	{ \
		box->addItem( QString::fromStdString( itemfun( i ) ) ); \
	} \
	return box; \
} \

QWidget * SoCCfgDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED( option )

	if ( index.parent() == QModelIndex() ||
		index.column() == SoCCfgModel::SOCCFG_ARGUMENT )
	{
		return 0;
	}

	SoCCfgModel * t_model = ( (SoCCfgModel *)index.model() );
	SoCCfgModelItem * t_item =
		(SoCCfgModelItem *)( t_model->itemFromIndex( index ) );
	switch ( t_item->itemCode() )
	{
	case SoCCfgModel::TILEFEA_NI_POS:
		createComboBox( EsySoCCfgTile::NIPOS_COUNT,
			EsySoCCfgTile::niPosStrVector ) break;
	case SoCCfgModel::TILEFEA_TYPE:
		createComboBox( EsySoCCfgTile::TILE_COUNT,
			EsySoCCfgTile::tileTypeStrVector ) break;
	case SoCCfgModel::TILEFEA_ISA_TYPE:
		createComboBox( EsySoCCfgTile::ISA_COUNT,
			EsySoCCfgTile::coreIsaTypeStrVector ) break;
	case SoCCfgModel::PORTCFG_DIRECTION:
		createComboBox( EsyNetCfgPort::ROUTER_PORT_NUM,
			EsyNetCfgPort::portDirectionStrVector ) break;
	case SoCCfgModel::TOPOLOGY_TOPOLOGY:
		createComboBox( EsyNetCfg::NOC_TOPOLOGY_COUNT,
			EsyNetCfg::nocTopologyStrVector ) break;
	case SoCCfgModel::CACHECFG_DL1_CFG: case SoCCfgModel::CACHECFG_DL2_CFG:
	case SoCCfgModel::CACHECFG_IL1_CFG: case SoCCfgModel::CACHECFG_IL2_CFG:
	case SoCCfgModel::MEMCFG_TLB_ITLB_CFG: case SoCCfgModel::MEMCFG_TLB_DTLB_CFG:
	case SoCCfgModel::PREDCFG_UPDATE_TYPE: case SoCCfgModel::PREDCFG_PREDICT_TYPE:
    case SoCCfgModel::TILEFEA_APP_DIR:  case SoCCfgModel::ASICTILE_NAME:
		{
			QLineEdit *box = new QLineEdit( "", parent );
			box->setFrame( false );
			return box;
		}
	case SoCCfgModel::TILELOC_AX_X: case SoCCfgModel::TILELOC_AX_Y:
	case SoCCfgModel::ROUTERLOC_AXIS_X: case SoCCfgModel::ROUTERLOC_AXIS_Y:
		{
			NumLineEdit *box = new NumLineEdit( NumLineEdit::DOUBLE_NUM,
				"", parent );
			box->setFrame( false );
			return box;
		}
	default:
		{
			QSpinBox * box = new QSpinBox( parent );
			box->setFrame( false );
			box->setMinimum( 0 );
			box->setMaximum( 2147483647 );
			return box;
		}
	}

	return 0;
}

void SoCCfgDelegate::setEditorData( QWidget *editor,
	const QModelIndex &index ) const
{
	if ( index.parent() == QModelIndex() ||
		index.column() == SoCCfgModel::SOCCFG_ARGUMENT )
	{
		return;
	}

	QString t_str = index.model()->data(index, Qt::DisplayRole).toString();

	SoCCfgModel * t_model = ( (SoCCfgModel *)index.model() );
	SoCCfgModelItem * t_item =
		(SoCCfgModelItem *)( t_model->itemFromIndex( index ) );
	switch ( t_item->itemCode() )
	{
	case SoCCfgModel::TILEFEA_NI_POS:
	case SoCCfgModel::TILEFEA_TYPE:
	case SoCCfgModel::TILEFEA_ISA_TYPE:
	case SoCCfgModel::PORTCFG_DIRECTION:
	case SoCCfgModel::TOPOLOGY_TOPOLOGY:
		{
			QComboBox *box = (QComboBox *)(editor);
			box->setCurrentText( t_str );
			break;
		}
	case SoCCfgModel::CACHECFG_DL1_CFG: case SoCCfgModel::CACHECFG_DL2_CFG:
	case SoCCfgModel::CACHECFG_IL1_CFG: case SoCCfgModel::CACHECFG_IL2_CFG:
	case SoCCfgModel::MEMCFG_TLB_ITLB_CFG: case SoCCfgModel::MEMCFG_TLB_DTLB_CFG:
	case SoCCfgModel::PREDCFG_UPDATE_TYPE: case SoCCfgModel::PREDCFG_PREDICT_TYPE:
    case SoCCfgModel::TILEFEA_APP_DIR:  case SoCCfgModel::ASICTILE_NAME:
		{
			QLineEdit *box = (QLineEdit *)(editor);
			box->setText( t_str );
			break;
		}
	case SoCCfgModel::TILELOC_AX_X: case SoCCfgModel::TILELOC_AX_Y:
	case SoCCfgModel::ROUTERLOC_AXIS_X: case SoCCfgModel::ROUTERLOC_AXIS_Y:
		{
			NumLineEdit *box = (NumLineEdit *)(editor);
			box->setText( t_str );
			break;
		}
	default:
		{
			QSpinBox * box = (QSpinBox *)(editor);
			box->setValue( t_str.toInt() );
			break;
		}
	}
}

void SoCCfgDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
	QVariant t_val;

	if ( index.parent() == QModelIndex() ||
		index.column() == SoCCfgModel::SOCCFG_ARGUMENT )
	{
		return;
	}

	SoCCfgModel * t_model = ( (SoCCfgModel *)index.model() );
	SoCCfgModelItem * t_item =
		(SoCCfgModelItem *)( t_model->itemFromIndex( index ) );
	switch ( t_item->itemCode() )
	{
	case SoCCfgModel::TILEFEA_NI_POS:
	case SoCCfgModel::TILEFEA_TYPE:
	case SoCCfgModel::TILEFEA_ISA_TYPE:
	case SoCCfgModel::PORTCFG_DIRECTION:
	case SoCCfgModel::TOPOLOGY_TOPOLOGY:
		{
			QComboBox *box = (QComboBox *)(editor);
			t_val = box->currentIndex();
			break;
		}
	case SoCCfgModel::CACHECFG_DL1_CFG: case SoCCfgModel::CACHECFG_DL2_CFG:
	case SoCCfgModel::CACHECFG_IL1_CFG: case SoCCfgModel::CACHECFG_IL2_CFG:
	case SoCCfgModel::MEMCFG_TLB_ITLB_CFG: case SoCCfgModel::MEMCFG_TLB_DTLB_CFG:
	case SoCCfgModel::PREDCFG_UPDATE_TYPE: case SoCCfgModel::PREDCFG_PREDICT_TYPE:
    case SoCCfgModel::TILEFEA_APP_DIR: case SoCCfgModel::ASICTILE_NAME:
		{
			QLineEdit *box = (QLineEdit *)(editor);
			t_val = box->text();
			break;
		}
	case SoCCfgModel::TILELOC_AX_X: case SoCCfgModel::TILELOC_AX_Y:
	case SoCCfgModel::ROUTERLOC_AXIS_X: case SoCCfgModel::ROUTERLOC_AXIS_Y:
		{
			NumLineEdit *box = (NumLineEdit *)(editor);
			t_val = box->text();
			break;
		}
	default:
		{
			QSpinBox * box = (QSpinBox *)(editor);
			t_val = box->value();
			break;
		}
	}

	model->setData( index, t_val, Qt::EditRole );
}

void SoCCfgDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED( index )
	editor->setGeometry(option.rect);
}

QSize SoCCfgDelegate::sizeHint( const QStyleOptionViewItem &option,
	const QModelIndex &index ) const
{
	Q_UNUSED( option )
	Q_UNUSED( index )

	return QSize( 100, 25 );
}

SoCCfgTreeView::SoCCfgTreeView( QWidget * parent ) :
	QTreeView( parent )
{
	setAlternatingRowColors( true );
	header()->setSectionResizeMode( QHeaderView::Interactive );
}
