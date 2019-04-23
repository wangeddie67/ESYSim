#include "soc_dialog.h"

// ---- constructor and destructor ---- //
SoCDialog::SoCDialog() :
	ResultBaseDialog( RESULT_NONE ),
	m_watch_router( 0 ),
	m_trace_fname( "" ),
	m_current_step( 0 )
{
    mp_soc_cfg = new EsySoCCfg();
	// initialize noc sence
	mp_soc_scene = new SoCScene(
		mp_soc_cfg, new GraphicsConfigModel( this ), this );
	// add components
	addComponents();
	// connect view and secene
    mp_soc_view->setScene( mp_soc_scene );
}

SoCDialog::SoCDialog( ResultTypeEnum type, EsySoCCfg* soc_cfg,
					  GraphicsConfigModel * graphics_cfg ) :
	ResultBaseDialog( type ),
	mp_soc_cfg( soc_cfg ),
	m_watch_router( 0 ), m_watch_tile( 0 ),
	m_trace_fname( "" ), m_jump_flag( false ), m_current_step( 0 )
{
	switch ( m_result_type )
	{
	case RESULT_DISPLAY_CONFIG:
    case RESULT_ONLINE_EVENTTRACE:
    case RESULT_REPLAY_EVENTTRACE:
	case RESULT_REPLAY_PATH:
	case RESULT_PATH_OVERLAP:
	case RESULT_PORT_TRAFFIC: break;
	default: m_result_type = RESULT_NONE; break;
	}
	// initialize noc sence
    mp_soc_scene = new SoCScene( mp_soc_cfg, graphics_cfg, this );
	// add components
	addComponents();
    mp_soc_view->setScene( mp_soc_scene );

	if ( m_result_type == RESULT_DISPLAY_CONFIG )
	{
		connect( mp_global_cfg_model, SIGNAL(updateScene()), this,
				 SLOT(applyGlobalConfigHandler() ) );
		connect( mp_tile_cfg_model, SIGNAL(updateScene()), this,
                 SLOT(applyTileConfigHandler() ) );
		connect( mp_router_cfg_model, SIGNAL(updateScene()), this,
                 SLOT(applyRouterConfigHandler() ) );
	}
}

SoCDialog::~SoCDialog()
{
	mp_soc_scene->clearSoCScene();
}
// ---- constructor and destructor ---- //

// ---- add components ---- //
// add components
void SoCDialog::addComponents()
{
	// noc view
	mp_soc_view = new SoCView(this);

    if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
         m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		ToolBarTab * control_tab =
			new ToolBarTab( "Control", 100, this );

        if ( m_result_type == RESULT_ONLINE_EVENTTRACE )
        {
            mp_control_panel = new AutomationControlPanel(
                AutomationControlPanel::OnlineControl, "ms", "Cycle", this );
        }
        else if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
        {
            mp_control_panel = new AutomationControlPanel(
                AutomationControlPanel::ReplayControl, "ms", "Cycle", this );
        }
		// watch router states
		connect( mp_control_panel, SIGNAL(prevRunButtonClicked()), this,
				 SLOT( prevRun() ) );
		connect( mp_control_panel, SIGNAL(previousButtonClicked()), this,
				 SLOT( prevCycle() ) );
		connect( mp_control_panel, SIGNAL(stopButtonClicked()), this,
				 SLOT( stopRun() ) );
		connect( mp_control_panel, SIGNAL(nextButtonClicked()), this,
				 SLOT (nextCycle() ) );
		connect( mp_control_panel, SIGNAL(nextrunButtonClicked()), this,
				 SLOT( nextRun() ) );
		connect( mp_control_panel, SIGNAL(jumpButtonClicked()), this,
				 SLOT( jumpRun() ) );

		connect( mp_control_panel, SIGNAL( nextRunTimeOut() ), this, SLOT( nextRun() ) );
		connect( mp_control_panel, SIGNAL( prevRunTimeOut() ), this, SLOT( prevRun() ) );

		control_tab->addToolBarBox( mp_control_panel );
		mp_toolbar_tab_list.append( control_tab );
	}

	QVBoxLayout * state_layout = new QVBoxLayout(this);

    if ( m_result_type == RESULT_DISPLAY_CONFIG )
	{
		state_layout->addWidget( new QLabel( "Parameters Configuration", this ) );

#define addSoCCfgTree( name, type ) \
SoCCfgTreeView * name##_view = new SoCCfgTreeView( this ); \
mp_##name##_model = new SoCCfgModel( this ); \
name##_view->setModel( mp_##name##_model ); \
name##_view->setItemDelegate( new SoCCfgDelegate( SoCCfgModel::type, this ) );

		addSoCCfgTree( global_cfg, SOCCFG_GLOBAL )
		mp_global_cfg_model->setGlobalCfg( mp_soc_cfg );
		addSoCCfgTree( tile_cfg, SOCCFG_TILE )
		addSoCCfgTree( router_cfg, SOCCFG_ROUTER )

		mp_tile_cfg_model->setTileCfg(
			&( mp_soc_cfg->tile( m_watch_tile ) ) );
		mp_router_cfg_model->setRouterCfg(
			&( mp_soc_cfg->network().router( m_watch_router ) ) );

        mp_config_tab_widget = new QTabWidget( this );
        mp_config_tab_widget->addTab( global_cfg_view, "Global" );
        mp_config_tab_widget->addTab( tile_cfg_view, "Tile" );
        mp_config_tab_widget->addTab( router_cfg_view, "Router" );

        state_layout->addWidget( mp_config_tab_widget );
	}
    else if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
              m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		state_layout->addWidget( new QLabel( "Tile Status", this ) );

		TileStateTableView * tile_state_view = new TileStateTableView( this );
		mp_tile_state_model = new TileStateModel( this );
		tile_state_view->setModel( mp_tile_state_model );

		mp_tile_state_model->setTileGraphicsItem(
			mp_soc_scene->tileItem( m_watch_tile ) );

        mp_config_tab_widget = new QTabWidget( this );
        mp_config_tab_widget->addTab( tile_state_view, "Tile Status" );

        state_layout->addWidget( mp_config_tab_widget );
	}

	mp_save_picture_button = new ToolBarButton(
		QIcon( "images/picture.png" ), "Picture", -1,
		ToolBarButton::LargeButton );
	connect( mp_save_picture_button, SIGNAL(clicked()), this,
			 SLOT( savePictureHandler() ) );
	ToolBarBox * export_box = new ToolBarBox( "Export", -1, this );
	export_box->addLargeButton( mp_save_picture_button );

	ToolBarTab * axis_tab = new ToolBarTab( "Figure", 101, this );
	axis_tab->addToolBarBox( export_box );
	mp_toolbar_tab_list.append( axis_tab );

	if ( m_result_type == RESULT_PATH_OVERLAP ||
		 m_result_type == RESULT_PORT_TRAFFIC )
	{
		state_layout->addStretch( 0 );
	}
	
	mp_state_widget = new QWidget( this );
	mp_state_widget->setLayout( state_layout );

	mp_soc_splitter = new QSplitter( Qt::Horizontal, this );
	mp_soc_splitter->addWidget( mp_soc_view );
	mp_soc_splitter->addWidget( mp_state_widget );
	mp_soc_splitter->setStretchFactor( 0, 80 );
	mp_soc_splitter->setStretchFactor( 1, 20 );
	// noc layout
	mp_soc_layout = new QHBoxLayout( this );
	mp_soc_layout->addWidget( mp_soc_splitter );

	setLayout( mp_soc_layout );
	setSelectComponent();

	connect(mp_soc_view, SIGNAL(selectRouterEvent( QMouseEvent * )), this,
			SLOT(selectRouterEvent( QMouseEvent * ) ) );
	connect(mp_soc_view, SIGNAL(hotKeyEvent( QKeyEvent * )), this,
			SLOT(keyReleaseEventHandler( QKeyEvent * ) ) );
}


void SoCDialog::applyGlobalConfigHandler()
{
	mp_soc_scene->clearSoCScene();
	mp_soc_cfg->updateSoC();
	mp_soc_scene->drawSoCScene();

	setSelectComponent();
}

void SoCDialog::applyTileConfigHandler()
{
    mp_soc_scene->clearSoCScene();
    mp_soc_scene->drawSoCScene();

    setSelectComponent();
}

void SoCDialog::applyRouterConfigHandler()
{
	mp_soc_scene->clearSoCScene();
    mp_soc_cfg->updateTile();
	mp_soc_scene->drawSoCScene();

	setSelectComponent();
}

void SoCDialog::savePictureHandler()
{
	QString t_pic_path = QFileDialog::getSaveFileName( this,
		"Save Picture", global_work_direction,
		tr("png file (*.png);;All file (*.*)") );
	if ( t_pic_path == "" )
		return;

	QImage t_image( mp_soc_scene->sceneRect().width(),
		mp_soc_scene->sceneRect().height(), QImage::Format_RGB32 );
	QPainter t_painter( &t_image );
	mp_soc_scene->render( &t_painter );
	t_image.save( t_pic_path );
}

// ---- I/O functions ---- //

// ---- slot functions ---- //
void SoCDialog::keyReleaseEventHandler(QKeyEvent * event)
{
	switch( event->key() )
	{
		case Qt::Key_Backspace: prevCycle(); break;
		case Qt::Key_Return:
		case Qt::Key_Enter: nextCycle(); break;
		case Qt::Key_Up: case Qt::Key_Left:
			if ( m_watch_tile > 0 )
			{
				m_watch_tile --;
			}
			break;
		case Qt::Key_Down: case Qt::Key_Right:
			if ( m_watch_tile < (long)mp_soc_cfg->tileCount() - 1 )
			{
				m_watch_tile ++;
			}
			break;
		default: break;
	}

	switch( event->key() )
	{
		case Qt::Key_Up: 
		case Qt::Key_Down: 
		case Qt::Key_Left: 
		case Qt::Key_Right: 
			mp_soc_scene->setSelectTile( m_watch_tile );
			setSelectComponent();
			mp_soc_view->centerOn( mp_soc_scene->routerItem( m_watch_router )->pos() );
		default: break;
	}
}

void SoCDialog::selectRouterEvent(QMouseEvent * event)
{
	Q_UNUSED(event);

	setSelectComponent();
}

void SoCDialog::setSelectComponent()
{
	bool changed = false;
	if ( m_watch_tile != mp_soc_scene->selectedTile() &&
		 mp_soc_scene->selectedTile() >= 0 )
	{
		m_watch_tile = mp_soc_scene->selectedTile();
		mp_soc_scene->setSelectTile( m_watch_tile );
		changed = true;
	}
	else if ( m_watch_router != mp_soc_scene->selectedRouter() &&
			  mp_soc_scene->selectedRouter() >= 0  )
	{
		m_watch_router = mp_soc_scene->selectedRouter();
		mp_soc_scene->setSelectRouter( m_watch_router );
		changed = true;
	}

	if ( changed )
	{
		if ( mp_soc_scene->selectedTile() >= 0 )
		{
			m_watch_tile = mp_soc_scene->selectedTile();
		}
		if ( mp_soc_scene->selectedRouter() >= 0 )
		{
			m_watch_router = mp_soc_scene->selectedRouter();
		}
		if ( m_result_type == RESULT_DISPLAY_CONFIG )
		{
			mp_tile_cfg_model->setTileCfg(
				&( mp_soc_cfg->tile( m_watch_tile ) ) );
			mp_router_cfg_model->setRouterCfg(
				&( mp_soc_cfg->network().router( m_watch_router ) ) );
		}
        else if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
		{
			mp_tile_state_model->setTileGraphicsItem(
				mp_soc_scene->tileItem( m_watch_tile ) );
			mp_control_panel->setCycleLabel( QString("%1/%2")
				.arg( mp_soc_scene->simCycle( m_watch_tile ) )
				.arg( m_total_step_list[ m_watch_tile ] ) );
		}
        else if ( m_result_type == RESULT_ONLINE_EVENTTRACE )
        {
            mp_tile_state_model->setTileGraphicsItem(
                mp_soc_scene->tileItem( m_watch_tile ) );
            mp_control_panel->setCycleLabel( QString("%1")
                .arg( mp_soc_scene->simCycle( m_watch_tile ) ) );
        }
    }
}

bool SoCDialog::setRecord( const QString & fname )
{
	// open file and check
	m_trace_fname = fname;
	string filepath_str = m_trace_fname.
			left( m_trace_fname.lastIndexOf(".") ).toStdString();

	if ( m_trace_fname.right( 1 ) == "b" )
	{
		m_record_reader = new EsyDataFileIStream< EsyDataItemSoCRecord >(
			10000, filepath_str, SOCRECORD_EXTENSION, true );
	}
	else
	{
		m_record_reader = new EsyDataFileIStream< EsyDataItemSoCRecord >(
			10000, filepath_str, SOCRECORD_EXTENSION, false );
	}
	if ( !m_record_reader )
	{
		QMessageBox::warning( this, "Wrong File",
			"Cannot open input record file (T1) " + m_trace_fname );
		return false;
	}
	if ( !m_record_reader->openSuccess() )
	{
		QMessageBox::warning( this, "Wrong File",
			"Cannot open input record file (T2) " + m_trace_fname );
		return false;
	}

	return true;
}

void SoCDialog::loadRecordStart()
{
	mp_control_panel->setLoadProgressBarRange( 0, m_record_reader->size() );

	if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		mp_load_record_thread = new AnalyseRecordThread(
			m_record_reader, mp_soc_cfg, &m_total_step, &m_total_step_list );
	}

	connect( mp_load_record_thread, SIGNAL( finished() ),
			 this, SLOT( loadRecordFinishedHandler() ) );
	connect( mp_load_record_thread, SIGNAL( progress(int) ),
			 mp_control_panel, SLOT( setLoadProgressBarValue(int) ) );

	mp_load_record_thread->start();
}

void SoCDialog::loadRecordFinishedHandler()
{
	if ( mp_load_record_thread->isGood() == false )
	{
		delete mp_load_record_thread;
		m_record_reader->close();
		delete m_record_reader;
		return;
	}
	delete mp_load_record_thread;

	if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		// if no item
		if ( m_total_step == 0 )
		{
			m_record_reader->close();
			delete m_record_reader;
			return;
		}

		for ( int i = 0; i < m_total_step_list.size(); i ++ )
		{
			if ( m_total_step_list[ i ] < 0 )
			{
				mp_soc_scene->disableTileSelectable( i );
			}
		}

		// reset pointer
		m_record_reader->reset();
		// set variable
		m_current_step = 0;

		mp_control_panel->setCycleLabel(
			QString("%1/%2").arg(0).arg( m_total_step_list[ m_watch_tile ] ) );
	}
	mp_control_panel->enableAllComponent( true );
}

void SoCDialog::setCommand(const QString & direction, const QString & command)
{
    m_process_dialog = new ProcessDialog(0, direction, command, true);
    mp_config_tab_widget->addTab( m_process_dialog, "Process" );
    m_process_dialog->start();

   // set variable
    m_current_step = 0;
    m_total_step = 2147483647LL;
    mp_control_panel->setCycleLabel( QString("%1").arg(0) );
    mp_control_panel->enableAllComponent( true );
}

// control
// previous cycle
void SoCDialog::prevCycle()
{
	// if it is the head
	if ( m_current_step == 0 )
	{
		return;
	}
	m_current_step --;

	if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		EsyDataItemSoCRecord prev_hop_item = m_record_reader->getPrevItem();
		m_current_step = prev_hop_item.hop();
		m_record_reader->getNextItem();
        while ( !m_record_reader->bof() )
		{
			EsyDataItemSoCRecord t_item = m_record_reader->getPrevItem();

			if ( t_item.hop() < m_current_step )
			{
				m_record_reader->getNextItem();
				break;
			}
			if ( !recordCheck( t_item ) )
			{
				m_run_error_flag = true;
				break;
			}
			mp_soc_scene->recordPrevHandler( t_item );
		}
		// update show
		mp_control_panel->setCycleLabel( QString("%1/%2")
			.arg( mp_soc_scene->simCycle( m_watch_tile ) )
			.arg( m_total_step_list[ m_watch_tile ] ) );
		mp_control_panel->setLoadProgressBarValue( m_record_reader->pos() );
		if ( !m_jump_flag )
		{
			updateComponents();
		}
	}
}
// next cycle
void SoCDialog::nextCycle()
{
	// if it is the end
	if ( m_current_step + 1 > m_total_step)
	{
		return;
	}
	// read to next cycle and handle event
	m_current_step ++;

	if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		EsyDataItemSoCRecord next_hop_item = m_record_reader->getNextItem();
		m_current_step = next_hop_item.hop() + 1;
		m_record_reader->getPrevItem();
		while ( !m_record_reader->eof() )
		{
			EsyDataItemSoCRecord t_item = m_record_reader->getNextItem();
			if ( t_item.hop() >= m_current_step )
			{
				m_record_reader->getPrevItem();
				break;
			}
			if ( !recordCheck( t_item ) )
			{
				m_run_error_flag = true;
				break;
			}
			mp_soc_scene->recordNextHandler( t_item );
		}
		mp_control_panel->setCycleLabel( QString("%1/%2")
			.arg( mp_soc_scene->simCycle( m_watch_tile ) )
			.arg( m_total_step_list[ m_watch_tile ] ) );
		mp_control_panel->setLoadProgressBarValue( m_record_reader->pos() );
		if ( !m_jump_flag )
		{
			updateComponents();
		}
	}
    else if ( m_result_type == RESULT_ONLINE_EVENTTRACE )
    {
        if (m_process_dialog->recordList().size() > 0)
        {
            EsyDataItemSoCRecord next_hop_item = m_process_dialog->recordList().first();
            m_current_step = next_hop_item.hop() + 1;

            while ( m_process_dialog->recordList().size() > 0 )
            {
                EsyDataItemSoCRecord t_item = m_process_dialog->recordList().first();
                if ( t_item.hop() >= m_current_step )
                {
                    break;
                }
                m_process_dialog->recordList().removeFirst();
                if ( !recordCheck( t_item ) )
                {
                    m_run_error_flag = true;
                    break;
                }
                mp_soc_scene->recordNextHandler( t_item );
            }
            mp_control_panel->setCycleLabel( QString("%1")
                .arg( mp_soc_scene->simCycle( m_watch_tile )) );
            mp_control_panel->setLoadProgressBarValue( m_process_dialog->progress() );
            if ( !m_jump_flag )
            {
                updateComponents();
            }
        }
        if ( m_process_dialog->isWaitForSyn() &&
             m_process_dialog->recordList().size() == 0 )
        {
            m_process_dialog->synchornizeNext();
        }
    }
}
// continue run
void SoCDialog::nextRun()
{
	m_run_error_flag = false;
	nextCycle();

    if ( m_result_type == RESULT_ONLINE_EVENTTRACE )
    {
        if ( (m_process_dialog->isFinished() &&
              m_process_dialog->recordList().size() == 0) ||
             (m_jump_flag && ( m_current_step > mp_control_panel->jumpValue() ) ) )
        {
            stopRun();
        }
        else
        {
            mp_control_panel->updateNextTimer();
        }
    }
    else
    {
        if ( (m_current_step + 1 > m_total_step) || m_run_error_flag ||
             (m_jump_flag && ( mp_soc_scene->simCycle( m_jump_watch_tile ) >
                mp_control_panel->jumpValue() ) ) )
        {
            stopRun();
        }
        else
        {
            mp_control_panel->updateNextTimer();
        }
    }
}
// continue run
void SoCDialog::prevRun()
{
	m_run_error_flag = false;
	prevCycle();

	if ( (m_current_step == 0) || m_run_error_flag ||
		 (m_jump_flag && ( mp_soc_scene->simCycle( m_jump_watch_tile ) <=
			mp_control_panel->jumpValue() ) ) )
	{
		stopRun();
	}
	else
	{
		mp_control_panel->updatePrevTimer();
	}
}
// stop run
void SoCDialog::stopRun()
{
	if ( m_jump_flag )
	{
		updateComponents();
	}

	m_jump_flag = false;

	mp_control_panel->stopButtonClickedHandler();
}

void SoCDialog::jumpRun()
{
	m_jump_flag = true;
	m_jump_watch_tile = m_watch_tile;
	if ( mp_soc_scene->simCycle( m_jump_watch_tile ) >
		mp_control_panel->jumpValue() )
	{
        if ( m_result_type != RESULT_ONLINE_EVENTTRACE )
        {
            prevRun();
        }
        else
        {
            stopRun();
        }
    }
	else if ( mp_soc_scene->simCycle( m_jump_watch_tile ) <
		mp_control_panel->jumpValue() )
	{
		nextRun();
	}
}

bool SoCDialog::recordCheck(const EsyDataItemSoCRecord &recorditem)
{
	if ( recorditem.actualId() < 0 ||
		recorditem.actualId() >= (int)mp_soc_cfg->tileCount() )
	{
		QMessageBox::warning( this, "Record Error",
			QString( "Record Tile %1 is not exist." )
				.arg( (int)recorditem.actualId() ) );
		return false;
	}
	return true;
}

void SoCDialog::updateComponents()
{
    if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
         m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		mp_soc_scene->update( mp_soc_scene->sceneRect() );
		mp_soc_scene->UpdateTooltip();
		mp_tile_state_model->update();
	}
}

