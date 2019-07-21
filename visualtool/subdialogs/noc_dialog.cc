#include "noc_dialog.h"

// ---- constructor and destructor ---- //
NoCDialog::NoCDialog() : 
	ResultBaseDialog( RESULT_NONE ),
	// initialization
	m_watch_router( 0 ),
	m_trace_fname( "" ),
	m_current_step( 0 )
{
	mp_network_cfg = new EsyNetCfg();
	// initialize noc sence
	mp_noc_scene = new NoCScene( mp_network_cfg, new GraphicsConfigModel( this ), this );
    // add components
	addComponents();
	// connect view and secene
	mp_noc_view->setScene(mp_noc_scene);
}

NoCDialog::NoCDialog( ResultTypeEnum type, EsyNetCfg* network_cfg,
					  GraphicsConfigModel * graphics_cfg ) :
	// initialization value
	ResultBaseDialog( type ),
	mp_network_cfg( network_cfg ),
	// initialization
	m_watch_router( 0 ),
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
	mp_noc_scene = new NoCScene( mp_network_cfg, graphics_cfg, this );
	// add components
	addComponents();
	mp_noc_view->setScene(mp_noc_scene);

	if ( m_result_type == RESULT_DISPLAY_CONFIG )
	{
		mp_network_cfg_model->setNetworkCfg( mp_network_cfg );
		connect( mp_network_cfg_model, SIGNAL(updateScene()), this,
				 SLOT(applyNetworkConfigHandler() ) );
		connect( mp_router_cfg_model, SIGNAL(updateScene()), this,
				 SLOT(applyRouterConfigHandler() ) );
	}
}

NoCDialog::~NoCDialog()
{
	mp_noc_scene->clearNoCScene();
}
// ---- constructor and destructor ---- //

// ---- add components ---- //
// add components
void NoCDialog::addComponents()
{
	// noc view
	mp_noc_view = new NoCView(this);

    if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
         m_result_type == RESULT_REPLAY_EVENTTRACE ||
		 m_result_type == RESULT_REPLAY_PATH ||
		 m_result_type == RESULT_PATH_OVERLAP ||
		 m_result_type == RESULT_PORT_TRAFFIC)
	{
		ToolBarTab * control_tab =
			new ToolBarTab( "Control", 100, this );

		if ( m_result_type == RESULT_REPLAY_PATH ||
			 m_result_type == RESULT_PATH_OVERLAP ||
			 m_result_type == RESULT_PORT_TRAFFIC )
		{
			mp_fliter_panel = new AnalyseFilterPanel( mp_network_cfg, this );

			AnalyseFilterPanel::FliterFlags flag;
			switch( m_result_type )
			{
			case RESULT_REPLAY_PATH:
				flag = AnalyseFilterPanel::FLITER_SOUR |
					AnalyseFilterPanel::FLITER_DEST | AnalyseFilterPanel::FLITER_INTER;
				break;
            case RESULT_PATH_OVERLAP:
				flag = AnalyseFilterPanel::FLITER_SOUR |
					AnalyseFilterPanel::FLITER_DEST | AnalyseFilterPanel::FLITER_INTER;
				break;
            case RESULT_PORT_TRAFFIC:
				flag =
					AnalyseFilterPanel::FLITER_SOUR | AnalyseFilterPanel::FLITER_DEST;
				break;
			default: flag = 0x00; break;
			}
			mp_fliter_panel->setVisibleFlag( flag );
			control_tab->addToolBarBox( mp_fliter_panel );
		}

        if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
             m_result_type == RESULT_REPLAY_EVENTTRACE ||
			 m_result_type == RESULT_REPLAY_PATH )
		{
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
            else if ( m_result_type == RESULT_REPLAY_PATH )
			{
				mp_control_panel = new AutomationControlPanel(
					AutomationControlPanel::ReplayControl, "ms", "Path", this );
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
		}
		else
		{
			mp_control_panel = new AutomationControlPanel(
				AutomationControlPanel::AnalyseControl, "", "", this );
			if ( m_result_type == RESULT_PATH_OVERLAP )
			{
				connect( mp_control_panel, SIGNAL(analyseButtonClicked()), this,
						 SLOT( loadPathtraceStart() ) );
			}
			else
			{
				connect( mp_control_panel, SIGNAL(analyseButtonClicked()), this,
						 SLOT( loadEventtraceStart() ) );
			}
		}

		control_tab->addToolBarBox( mp_control_panel );

        if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
             m_result_type == RESULT_REPLAY_EVENTTRACE )
		{
			mp_checkbox_mapping = new QCheckBox("Show Only\nMapping Event", this);
			connect( mp_control_panel, SIGNAL(anyButtonClicked()), this,
					 SLOT( disableMappingOnlyChecker() ) );

			ToolBarBox * checkbox_box = new ToolBarBox( "Mapping Only", -1, this );
			checkbox_box->hBoxLayout()->addWidget( mp_checkbox_mapping );
			control_tab->addToolBarBox( checkbox_box );
		}

		mp_toolbar_tab_list.append( control_tab );
	}

	QVBoxLayout * state_layout = new QVBoxLayout(this);

    if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
         m_result_type == RESULT_DISPLAY_CONFIG ||
		 m_result_type == RESULT_REPLAY_EVENTTRACE ||
		 m_result_type == RESULT_PATH_OVERLAP ||
		 m_result_type == RESULT_PORT_TRAFFIC ||
		 m_result_type == RESULT_REPLAY_PATH )
	{
		mp_view_tab_widget = new QTabWidget();
		state_layout->addWidget( mp_view_tab_widget );
	}

	if ( m_result_type == RESULT_DISPLAY_CONFIG )
	{
		NetworkCfgTreeView * network_cfg_view = new NetworkCfgTreeView( this );
		mp_network_cfg_model = new NetworkCfgModel( this );
		network_cfg_view->setModel( mp_network_cfg_model );
		network_cfg_view->setItemDelegate( new NetworkCfgDelegate(
			NetworkCfgModel::NETWORKCFG_NETWORK, this ) );
		mp_view_tab_widget->addTab( network_cfg_view, "Network" );

		NetworkCfgTreeView * router_cfg_view = new NetworkCfgTreeView( this );
		mp_router_cfg_model = new NetworkCfgModel( this );
		router_cfg_view->setModel( mp_router_cfg_model );
		router_cfg_view->setItemDelegate( new NetworkCfgDelegate(
			NetworkCfgModel::NETWORKCFG_ROUTER, this ) );
		mp_view_tab_widget->addTab( router_cfg_view, QString( "Router (%1)" ).
			arg( m_watch_router ) );
	}
    if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
         m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		// state table
		RouterStateTableView * input_view  = new RouterStateTableView( this );
		mp_router_input_model = new RouterStateModel(
					RouterStateModel::ROUTERSTATE_INPUT, this );
		input_view->setModel( mp_router_input_model );
		mp_view_tab_widget->addTab( input_view, QString( "Input Port (%1)" ).
							arg( m_watch_router ) );

		RouterStateTableView * output_view = new RouterStateTableView( this );
		mp_router_output_model = new RouterStateModel(
					RouterStateModel::ROUTERSTATE_OUTPUT, this );
		output_view->setModel( mp_router_output_model );
		mp_view_tab_widget->addTab( output_view, QString( "Output Port (%1)" ).
							arg( m_watch_router ) );
	}
	if ( m_result_type == RESULT_REPLAY_PATH ||
		 m_result_type == RESULT_PATH_OVERLAP ||
		 m_result_type == RESULT_PORT_TRAFFIC )
	{
		// path table
		PathTraceTableView * path_table  = new PathTraceTableView( this );
		mp_path_model = new PathTraceModel( this, mp_network_cfg );
		path_table->setModel( mp_path_model );
		if ( m_result_type == RESULT_PATH_OVERLAP ||
			 m_result_type == RESULT_PORT_TRAFFIC )
		{
			path_table->setSortingEnabled( true );
		}

		if ( m_result_type == RESULT_REPLAY_PATH )
		{
			mp_view_tab_widget->addTab( path_table, QString( "Path (%1)->(%2)" )
				.arg( 0 ).arg( 0 ) );
		}
		else
		{
			mp_view_tab_widget->addTab( path_table, QString( "Traffic (%1)" )
				.arg( m_watch_router ) );
		}
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

	mp_noc_splitter = new QSplitter( Qt::Horizontal, this );
	mp_noc_splitter->addWidget( mp_noc_view );
	mp_noc_splitter->addWidget( mp_state_widget );
	mp_noc_splitter->setStretchFactor( 0, 80 );
	mp_noc_splitter->setStretchFactor( 1, 20 );
	// noc layout
	mp_noc_layout = new QHBoxLayout( this );
	mp_noc_layout->addWidget( mp_noc_splitter );

	setLayout( mp_noc_layout );
	setSelectRouter();

	connect(mp_noc_view, SIGNAL(selectRouterEvent( QMouseEvent * )), this,
			SLOT(selectRouterEvent( QMouseEvent * ) ) );
	connect(mp_noc_view, SIGNAL(hotKeyEvent( QKeyEvent * )), this,
			SLOT(keyReleaseEventHandler( QKeyEvent * ) ) );
}


void NoCDialog::applyNetworkConfigHandler()
{
	mp_noc_scene->clearNoCScene();
	mp_network_cfg->updateRouter();
	mp_noc_scene->drawNoCScene();

	m_watch_router = 0;
	setSelectRouter();
}

void NoCDialog::applyRouterConfigHandler()
{
	mp_noc_scene->clearNoCScene();
	mp_noc_scene->drawNoCScene();
	setSelectRouter();
}

void NoCDialog::savePictureHandler()
{
	QString t_pic_path = QFileDialog::getSaveFileName( this,
		"Save Picture", global_work_direction,
		tr("png file (*.png);;All file (*.*)") );
	if ( t_pic_path == "" )
		return;

	QImage t_image( mp_noc_scene->sceneRect().width(),
		mp_noc_scene->sceneRect().height(), QImage::Format_RGB32 );
	QPainter t_painter( &t_image );
	mp_noc_scene->render( &t_painter );
	t_image.save( t_pic_path );
}

// ---- I/O functions ---- //

// ---- slot functions ---- //
void NoCDialog::keyReleaseEventHandler(QKeyEvent * event)
{
	switch( event->key() )
	{
		case Qt::Key_Backspace: prevCycle(); break;
		case Qt::Key_Return:
		case Qt::Key_Enter: nextCycle(); break;
		case Qt::Key_Up: 
			if ( mp_network_cfg->topology() ==
				 EsyNetCfg::NOC_TOPOLOGY_2DMESH ||
				 mp_network_cfg->topology() ==
				 EsyNetCfg::NOC_TOPOLOGY_2DTORUS )
			{
				if ( m_watch_router >=
					 mp_network_cfg->size( EsyNetCfg::AX_X ) )
				{
					m_watch_router -=
							mp_network_cfg->size( EsyNetCfg::AX_X );
				}
			}
			else
			{
				if ( m_watch_router > 0 )
				{
					m_watch_router --;
				}
			}
			break;
		case Qt::Key_Down: 
			if ( mp_network_cfg->topology() ==
				 EsyNetCfg::NOC_TOPOLOGY_2DMESH ||
				 mp_network_cfg->topology() ==
				 EsyNetCfg::NOC_TOPOLOGY_2DTORUS )
			{
				if ( m_watch_router < mp_network_cfg->routerCount() -
					 mp_network_cfg->size( EsyNetCfg::AX_X ) - 1 )
				{
					m_watch_router +=
						 mp_network_cfg->size( EsyNetCfg::AX_X );
				}
			}
			else
			{
				if ( m_watch_router <
					 mp_network_cfg->size( EsyNetCfg::AX_X ) - 1 )
				{
					m_watch_router ++;
				}
			}
			break;
		case Qt::Key_Left: 
			if ( m_watch_router > 0 )
				m_watch_router --;
			break;
		case Qt::Key_Right: 
			if ( mp_network_cfg->topology() ==
				 EsyNetCfg::NOC_TOPOLOGY_2DMESH ||
				 mp_network_cfg->topology() ==
				 EsyNetCfg::NOC_TOPOLOGY_2DTORUS )
			{
				if ( m_watch_router < mp_network_cfg->routerCount() - 1 )
				{
					m_watch_router ++;
				}
			}
			else
			{
				if ( m_watch_router <
					 mp_network_cfg->size( EsyNetCfg::AX_X ) - 1 )
				{
					m_watch_router ++;
				}
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
			setSelectRouter();
			mp_noc_view->centerOn( mp_noc_scene->routerItem( m_watch_router )->pos() );
		default: break;
	}
}

void NoCDialog::selectRouterEvent(QMouseEvent * event)
{
	Q_UNUSED(event);
	
	m_watch_router = mp_noc_scene->selectedRouter();
	setSelectRouter();
}

void NoCDialog::setSelectRouter()
{
	mp_noc_scene->setSelectRouterNoCScene( m_watch_router );
	if ( m_result_type == RESULT_DISPLAY_CONFIG )
	{
		mp_view_tab_widget->setTabText( 1,
			QString( "Router (%1)" ).arg( m_watch_router ) );
	}
    else if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
              m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		mp_view_tab_widget->setTabText( 0,
			QString( "Input Port (%1)" ).arg( m_watch_router ) );
		mp_view_tab_widget->setTabText( 1,
			QString( "Output Port (%1)" ).arg( m_watch_router ) );
	}
	else if ( m_result_type == RESULT_PATH_OVERLAP ||
		 m_result_type == RESULT_PORT_TRAFFIC )
	{
		mp_view_tab_widget->setTabText( 0,
			QString( "Traffic (%1)" ).arg( m_watch_router ) );
	}

	if (m_result_type == RESULT_DISPLAY_CONFIG)
	{
		mp_router_cfg_model->setRouterCfg(
					&( mp_network_cfg->router( m_watch_router ) ) );
	}
    else if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
              m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		mp_router_input_model->setRouterGraphicsItem(
			mp_noc_scene->routerItem( m_watch_router ) );
		mp_router_output_model->setRouterGraphicsItem(
			mp_noc_scene->routerItem( m_watch_router ) );
	}
	else if ( m_result_type == RESULT_PATH_OVERLAP ||
			  m_result_type == RESULT_PORT_TRAFFIC )
	{
		mp_path_model->setPathPair(
			mp_noc_scene->routerItem( m_watch_router )->pathPairList() );
	}
}
// ---- slot functions ---- //

// ---- animation functions ---- //
// set event trace
bool NoCDialog::setEventtrace( const QString & fname )
{
	// open file and check
	m_trace_fname = fname;
	string filepath_str = m_trace_fname.
			left( m_trace_fname.lastIndexOf(".") ).toStdString();

	if ( m_trace_fname.right( 1 ) == "b" )
	{
		m_event_trace_reader = new EsyDataFileIStream< EsyDataItemEventtrace >(
			10000, filepath_str, EVENTTRACE_EXTENSION, true );
	}
	else
	{
		m_event_trace_reader = new EsyDataFileIStream< EsyDataItemEventtrace >(
			10000, filepath_str, EVENTTRACE_EXTENSION, false );
	}
	if ( !m_event_trace_reader )
	{
		QMessageBox::warning( this, "Wrong File",
			"Cannot open input trace file (T1) " + m_trace_fname );
		return false;
	}
	if ( !m_event_trace_reader->openSuccess() )
	{
		QMessageBox::warning( this, "Wrong File",
			"Cannot open input trace file (T2) " + m_trace_fname );
		return false;
	}

	return true;
}

void NoCDialog::loadEventtraceStart()
{
	mp_control_panel->setLoadProgressBarRange( 0, m_event_trace_reader->size() );

	if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
    {
        mp_load_event_trace_thread = new AnalyseEventtraceThread(
            m_event_trace_reader, mp_network_cfg, &m_total_step );
    }
	else
	{
		mp_load_event_trace_thread = new AnalyseEventtraceThread(
			m_event_trace_reader, mp_network_cfg,
            mp_fliter_panel->srcId(), mp_fliter_panel->dstId(),
			mp_fliter_panel->appId(),
			&m_total_app,
			&m_analyse_path_hop_list
			);
    }
    connect( mp_load_event_trace_thread, SIGNAL( finished() ),
             this, SLOT( loadEventtraceFinishedHandler() ) );
    connect( mp_load_event_trace_thread, SIGNAL( progress(int) ),
             mp_control_panel, SLOT( setLoadProgressBarValue(int) ) );

    mp_load_event_trace_thread->start();
}

void NoCDialog::loadEventtraceFinishedHandler()
{
	if ( mp_load_event_trace_thread->isGood() == false )
	{
		delete mp_load_event_trace_thread;
		m_event_trace_reader->close();
		delete m_event_trace_reader;
		return;
	}
	delete mp_load_event_trace_thread;

	if ( m_result_type == RESULT_PORT_TRAFFIC )
	{
		mp_noc_scene->setPathTraceModelItemVector( m_analyse_path_hop_list );
		mp_fliter_panel->setAppCount( m_total_app );
	}
	else
	{
		// if no item
		if ( m_total_step == 0 )
		{
			m_event_trace_reader->close();
			delete m_event_trace_reader;
			return;
		}

		// reset pointer
		m_event_trace_reader->reset();
		// set variable
		m_current_step = 0;

		mp_control_panel->setCycleLabel( QString("%1/%2").arg(0).arg(m_total_step) );
	}
	mp_control_panel->enableAllComponent( true );
}

bool NoCDialog::setPathtrace( const QString & fname )
{
	// open file and check
	m_trace_fname = fname;
	string filepath_str = m_trace_fname.left(
				m_trace_fname.lastIndexOf(".") ).toStdString();

	if ( m_trace_fname.right( 1 ) == "b" )
	{
		m_path_trace_reader = new EsyDataFileIStream< EsyDataItemPathtrace >(
			10000, filepath_str, PATHTRACE_EXTENSION, true );
	}
	else
	{
		m_path_trace_reader = new EsyDataFileIStream< EsyDataItemPathtrace >(
			10000, filepath_str, PATHTRACE_EXTENSION, false );
	}
	if ( !m_path_trace_reader )
	{
		QMessageBox::warning( this, "Wrong File",
			"Cannot open input trace file (T1) " + m_trace_fname );
		return false;
	}
	if ( !m_path_trace_reader->openSuccess() )
	{
		QMessageBox::warning( this, "Wrong File",
			"Cannot open input trace file (T2) " + m_trace_fname );
		return false;
	}

	return true;
}

void NoCDialog::loadPathtraceStart()
{
	mp_control_panel->setLoadProgressBarRange( 0, m_path_trace_reader->size() );

	if ( m_result_type == RESULT_REPLAY_PATH )
	{
		mp_load_path_trace_thread = new AnalysePathtraceThread(
					m_path_trace_reader, &m_total_step );
	}
	else
	{
		mp_load_path_trace_thread = new AnalysePathtraceThread(
			m_path_trace_reader, &m_analyse_path_hop_list,
			mp_fliter_panel->srcId(), mp_fliter_panel->dstId(),
			mp_fliter_panel->routerId(),
			mp_fliter_panel->inputPhy(), mp_fliter_panel->inputVc(),
			mp_fliter_panel->outputPhy(), mp_fliter_panel->outputVc() );
	}
	connect( mp_load_path_trace_thread, SIGNAL( finished() ),
			 this, SLOT( loadPathtraceFinishedHandler() ) );
	connect( mp_load_path_trace_thread, SIGNAL( progress(int) ),
			 mp_control_panel, SLOT( setLoadProgressBarValue(int) ) );

	mp_load_path_trace_thread->start();
}

void NoCDialog::loadPathtraceFinishedHandler()
{
	if ( mp_load_path_trace_thread->isGood() == false )
	{
		delete mp_load_path_trace_thread;
		m_path_trace_reader->close();
		delete m_path_trace_reader;
		return;
	}
	delete mp_load_path_trace_thread;

	if ( m_result_type == RESULT_REPLAY_PATH )
	{
		// if no item
		if ( m_total_step == 0 )
		{
			m_path_trace_reader->close();
			delete m_path_trace_reader;
			return;
		}

		// reset pointer
		m_path_trace_reader->reset();
		// set variable
		m_current_step = 0;

		mp_control_panel->setCycleLabel( QString("%1/%2").arg(0).arg(m_total_step) );
		mp_control_panel->initTimer();
	}
	else if ( m_result_type == RESULT_PATH_OVERLAP )
	{
		mp_noc_scene->setPathTraceModelItemVector( m_analyse_path_hop_list );
	}
	mp_control_panel->enableAllComponent( true );
}

void NoCDialog::setCommand(const QString & direction, const QString & command)
{
    m_process_dialog = new ProcessDialog(0, direction, command, true);
    mp_view_tab_widget->addTab( m_process_dialog, "Process" );
    m_process_dialog->start();

   // set variable
    m_current_step = 0;
    m_total_step = 2147483647LL;
    mp_control_panel->setCycleLabel( QString("%1").arg(0) );
    mp_control_panel->enableAllComponent( true );
}

// control
// previous cycle
void NoCDialog::prevCycle()
{
	// if it is the head
	if ( m_current_step == 0 )
	{
		return;
	}
	m_current_step --;
	
	if ( m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		// decide the time to stop
		if ( mp_checkbox_mapping->isChecked() )
		{
            while ( !m_event_trace_reader->bof() )
			{
				EsyDataItemEventtrace t_item = m_event_trace_reader->getPrevItem();
				if ( t_item.type() == ET_PE_CHANGE )  //not Mapping event , skip
				{
					 m_current_step = t_item.time();// handle this item
					 m_event_trace_reader->getNextItem();
					 break;
				}
			}
		}

        while ( !m_event_trace_reader->bof() )
		{
			EsyDataItemEventtrace t_item = m_event_trace_reader->getPrevItem();

			if ( t_item.time() < m_current_step )
			{
				m_event_trace_reader->getNextItem();
				break;
			}
			if ( !(mp_checkbox_mapping->isChecked() && (t_item.type() != ET_PE_CHANGE) ) )
			{
                if ( !eventCheck( t_item, false ) )
                {
                    m_run_error_flag = true;
                    break;
                }
                mp_noc_scene->eventPrevHandler( t_item );
			}
		}
		mp_noc_scene->eventTraceDeadlockCheck();
		// update show
		mp_control_panel->setCycleLabel(
					QString("%1/%2").arg(m_current_step).arg(m_total_step) );
		mp_control_panel->setLoadProgressBarValue( m_event_trace_reader->pos() );
		if ( !m_jump_flag )
		{
			updateComponents();
		}
	}
    else if ( m_result_type == RESULT_REPLAY_PATH )
	{
        if ( !m_path_trace_reader->bof() )
		{
			EsyDataItemPathtrace path_trace_item = m_path_trace_reader->getPrevItem();
			if ( !pathCheck( path_trace_item ) )
			{
				m_run_error_flag = true;
			}
			else
			{
				mp_control_panel->setCycleLabel(
					QString("%1/%2").arg(m_current_step).arg(m_total_step) );
				mp_control_panel->setLoadProgressBarValue( m_path_trace_reader->pos() );
				if ( !m_jump_flag )
				{
					pathHandler( path_trace_item );
					updateComponents();
				}
			}
		}
	}
}
// next cycle
void NoCDialog::nextCycle()
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
		// decide the time to stop
		if ( mp_checkbox_mapping->isChecked() )
		{
			while ( !m_event_trace_reader->eof() )
			{
				EsyDataItemEventtrace t_item = m_event_trace_reader->getNextItem();
				if ( t_item.type() == ET_PE_CHANGE )  //not Mapping event , skip
				{
					 m_current_step = t_item.time() + 1;// handle this item
					 m_event_trace_reader->getPrevItem();
					 break;
				}
			}
		}

		while ( !m_event_trace_reader->eof() )
		{
			EsyDataItemEventtrace t_item = m_event_trace_reader->getNextItem();
			if ( t_item.time() >= m_current_step )
			{
				m_event_trace_reader->getPrevItem();
				break;
			}
			if ( !(mp_checkbox_mapping->isChecked() && (t_item.type() != ET_PE_CHANGE) ) )
			{
                if ( !eventCheck( t_item, true ) )
                {
                    m_run_error_flag = true;
                    break;
                }
                mp_noc_scene->eventNextHandler( t_item );
			}
		}
		mp_noc_scene->eventTraceDeadlockCheck();
		mp_control_panel->setCycleLabel(
			QString("%1/%2").arg(m_current_step).arg(m_total_step) );
		mp_control_panel->setLoadProgressBarValue( m_event_trace_reader->pos() );
		if ( !m_jump_flag )
		{
			updateComponents();
		}
	}
    else if ( m_result_type == RESULT_ONLINE_EVENTTRACE )
    {
        while ( m_process_dialog->eventList().size() > 0 )
        {
            EsyDataItemEventtrace t_item =
                m_process_dialog->eventList().first();
            if ( t_item.time() >= m_current_step )
            {
                break;
            }
            m_process_dialog->eventList().removeFirst();
            if ( !(mp_checkbox_mapping->isChecked() && (t_item.type() != ET_PE_CHANGE) ) )
            {
                if ( !eventCheck( t_item, true ) )
                {
                    m_run_error_flag = true;
                    break;
                }
                mp_noc_scene->eventNextHandler( t_item );
            }
        }
        mp_noc_scene->eventTraceDeadlockCheck();
        mp_control_panel->setCycleLabel( QString("%1").arg(m_current_step) );
        mp_control_panel->setLoadProgressBarValue( m_process_dialog->progress() );
        if ( !m_jump_flag )
        {
            updateComponents();
        }
        if ( m_process_dialog->isWaitForSyn() &&
             m_process_dialog->eventList().size() == 0 )
        {
            m_process_dialog->synchornizeNext();
        }
    }
    else if ( m_result_type == RESULT_REPLAY_PATH )
	{
		if ( !m_path_trace_reader->eof() )
		{
			EsyDataItemPathtrace path_trace_item = m_path_trace_reader->getNextItem();
			if ( !pathCheck( path_trace_item ) )
			{
				m_run_error_flag = true;
			}
			else
			{
				// update show
				mp_control_panel->setCycleLabel(
					QString("%1/%2").arg(m_current_step).arg(m_total_step) );
				mp_control_panel->setLoadProgressBarValue( m_path_trace_reader->pos() );
				if ( !m_jump_flag )
				{
					pathHandler( path_trace_item );
					updateComponents();
				}
			}
		}
	}
}
// continue run
void NoCDialog::nextRun()
{
	m_run_error_flag = false;
	nextCycle();

    if ( m_result_type == RESULT_ONLINE_EVENTTRACE )
    {
        if ( (m_process_dialog->isFinished() &&
              m_process_dialog->eventList().size() == 0) ||
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
             (m_jump_flag && ( m_current_step > mp_control_panel->jumpValue() ) ) )
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
void NoCDialog::prevRun()
{
	m_run_error_flag = false;
	prevCycle();

	if ( (m_current_step == 0) || m_run_error_flag ||
		 (m_jump_flag && ( m_current_step <= mp_control_panel->jumpValue() ) ) )
	{
            stopRun();
	}
	else
	{
		mp_control_panel->updatePrevTimer();
	}
}
// stop run
void NoCDialog::stopRun()
{
	if ( m_jump_flag )
	{
		updateComponents();
	}

	m_jump_flag = false;

	mp_control_panel->stopButtonClickedHandler();
}

void NoCDialog::jumpRun()
{
    m_jump_flag = true;
    if ( m_current_step > mp_control_panel->jumpValue() )
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
	else if ( m_current_step < mp_control_panel->jumpValue() )
	{
        nextRun();
	}
}

void NoCDialog::updateComponents()
{
    if ( m_result_type == RESULT_ONLINE_EVENTTRACE ||
         m_result_type == RESULT_REPLAY_EVENTTRACE )
	{
		mp_noc_scene->update( mp_noc_scene->sceneRect() );
		mp_noc_scene->UpdateTooltip();
		mp_router_input_model->update();
		mp_router_output_model->update();
	}
	else if ( m_result_type == RESULT_REPLAY_PATH )
	{
		mp_noc_scene->update( mp_noc_scene->sceneRect() );
	}
}

void NoCDialog::pathHandler( EsyDataItemPathtrace & eventitem )
{
	mp_path_model->setPathTrace( eventitem.pathhoplist() );
	mp_noc_scene->pathHandler( eventitem );
	mp_view_tab_widget->setTabText( 0, QString( "Path (%1)->(%2)" )
		.arg((int)eventitem.src() ).arg((int)eventitem.dst() ) );
}

bool NoCDialog::eventCheck( const EsyDataItemEventtrace & eventitem, bool forward )
{
	if ( eventitem.type() >= ET_PE_CHANGE )
	{
		return true;
	}
	if ( eventitem.type() < ET_FT_INJECTION )
	{
		if ( eventitem.src() >= 0 )
		{
			if ( eventitem.src() >= mp_network_cfg->routerCount() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Event Source Router %1 is not exist." )
					  .arg( (int)eventitem.src() ) );
				return false;
			}
		}
		if ( eventitem.src() >= 0 && eventitem.srcPc() >= 0 )
		{
			if ( eventitem.srcPc() >=
				 mp_network_cfg->router( eventitem.src() ).portNum() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Source Router %1, Port %2 is not exist." )
					   .arg( (int)eventitem.src() ).arg( (int)eventitem.srcPc() ) );
				return false;
			}
		}
		if ( eventitem.src() >= 0 && eventitem.srcPc() >= 0 && eventitem.srcVc() >= 0 )
		{
			if ( eventitem.srcVc() < 0 || eventitem.srcVc() >=
				 mp_network_cfg->router( eventitem.src() )
					 .port( eventitem.srcPc() ).maxVcNumber() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Source Router %1, Port %2, VC %3 is not exist." )
						.arg( (int)eventitem.src() ).arg( (int)eventitem.srcPc() )
						.arg( (int)eventitem.srcVc() ) );
				return false;
			}
		}
		if ( eventitem.dst() >= 0 )
		{
			if ( eventitem.dst() >= mp_network_cfg->routerCount() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Event Destination Router %1 is not exist." )
						.arg( (int)eventitem.dst() ) );
				return false;
			}
		}
		if ( eventitem.dst() >= 0 && eventitem.dstPc() >= 0 )
		{
			if ( eventitem.dstPc() >=
				 mp_network_cfg->router( eventitem.dst() ).portNum() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Destination Router %1, Port %2 is not exist." )
						.arg( (int)eventitem.dst() ).arg( (int)eventitem.dstPc() ) );
				return false;
			}
		}
		if ( eventitem.dst() >= 0 && eventitem.dstPc() >= 0 && eventitem.dstVc() >= 0 )
		{
			if ( eventitem.dstVc() < 0 || eventitem.dstVc() >=
				 mp_network_cfg->router( eventitem.dst() ).
				 port( eventitem.dstPc() ).maxVcNumber() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Destination Router %1, Port %2, VC %3 is not exist." )
						.arg( (int)eventitem.dst() ).arg( (int)eventitem.dstPc() )
						.arg( (int)eventitem.dstVc() ) );
				return false;
			}
		}
		if ( eventitem.flitSrc() >= 0 && eventitem.type() != ET_VC_STATE )
		{
			if ( eventitem.flitSrc() >= mp_network_cfg->routerCount() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Flit Source Router %1 is not exist." )
						.arg( eventitem.flitSrc() ) );
				return false;
			}
		}
		if ( eventitem.flitDst() >= 0 && eventitem.type() != ET_VC_STATE )
		{
			if ( eventitem.flitDst() >= mp_network_cfg->routerCount() )
			{
				QMessageBox::warning( this, "Event Error",
					QString( "Flit Destination Router %1 is not exist." )
						.arg( eventitem.flitDst() ) );
				return false;
			}
		}
	}
	else
	{
		if ( eventitem.faultyRouter() >= 0 )
		{
			if ( eventitem.faultyRouter() >= mp_network_cfg->routerCount() )
			{
                QMessageBox::warning( this, "Fault Error",
					QString( "Faulty Router %1 is not exist." )
                      .arg( (int)eventitem.faultyRouter() ) );
                return false;
			}
		}
		if ( eventitem.faultyPort() >= 0 )
		{
			if ( eventitem.faultyPort() >=
				 mp_network_cfg->router( eventitem.faultyRouter() ).portNum() )
			{
				QMessageBox::warning( this, "Fault Error",
					QString( "Faulty Router %1, Port %2 is not exist." )
					   .arg( (int)eventitem.faultyRouter() )
					   .arg( (int)eventitem.faultyPort() ) );
				return false;
			}
		}
		if ( eventitem.faultyPort() >= 0 && eventitem.faultyVC() >= 0 )
		{
			if ( eventitem.faultyVC() < 0 || eventitem.faultyVC() >=
				 mp_network_cfg->router( eventitem.faultyRouter() )
					 .port( eventitem.faultyPort() ).maxVcNumber() )
			{
				QMessageBox::warning( this, "Fault Error",
					QString( "Faulty Router %1, Port %2, VC %3 is not exist." )
						.arg( (int)eventitem.faultyRouter() )
						.arg( (int)eventitem.faultyPort() )
						.arg( (int)eventitem.faultyVC() ) );
				return false;
			}
		}
	}
/*    QString t_error_message;
    if ( !mp_noc_scene->eventCheck( eventitem, t_error_message, forward ) )
    {
        int temp = QMessageBox::warning( this, "Event Error", t_error_message, "Continue", "Stop", QString::null, 0 );
        if ( temp == 0 )
        {
            return true;
        }
        else
        {
            return false;
        }
    }*/
    return true;
}

bool NoCDialog::pathCheck( const EsyDataItemPathtrace & pathitem )
{
	if ( pathitem.src() < 0 || pathitem.src() >= mp_network_cfg->routerCount() )
	{
		QMessageBox::warning( this, "Path Error",
			QString( "Path Source Router %1 is not exist." )
				.arg( (int)pathitem.src() ) );
		return false;
	}
	if ( pathitem.dst() < 0 || pathitem.dst() >= mp_network_cfg->routerCount() )
	{
		QMessageBox::warning( this, "Path Error",
			QString( "Path Destination Router %1 is not exist." )
				.arg( (int)pathitem.dst() ) );
		return false;
	}
	vector< EsyDataItemHop > hoplist = pathitem.pathhoplist();
	for ( size_t i = 0; i < hoplist.size(); i ++ )
	{
		if ( hoplist[ i ].addr() < 0 ||
			 hoplist[ i ].addr() >= mp_network_cfg->routerCount() )
		{
			QMessageBox::warning( this, "Path Error",
				QString( "Hop Router %1 is not exist." )
					.arg( (int)hoplist[i].addr() ) );
			return false;
		}
		if ( hoplist[ i ].inputPc() < 0 || hoplist[ i ].inputPc() >=
			 mp_network_cfg->router( hoplist[ i ].addr() ).portNum() )
		{
			QMessageBox::warning( this, "Path Error",
				 QString( "Hop Router %1, Input Port %2 is not exist." )
					.arg( (int)hoplist[ i ].addr() )
					.arg( (int)hoplist[ i ].inputPc() ) );
			return false;
		}
		if ( hoplist[i].inputVc() < 0 || hoplist[i].inputVc() >=
			 mp_network_cfg->router( hoplist[ i ].addr() )
				 .port( hoplist[ i ].inputPc() ).maxVcNumber() )
		{
			QMessageBox::warning( this, "Path Error",
				QString( "Hop Router %1, Input Port %2, VC %3 is not exist." )
					.arg( (int)hoplist[ i ].addr() )
					.arg( (int)hoplist[ i ].inputPc() )
					.arg( (int)hoplist[ i ].inputVc() ) );
			return false;
		}
		if ( hoplist[ i ].outputPc() < 0 || hoplist[ i ].outputPc() >=
			 mp_network_cfg->router( hoplist[ i ].addr() ).portNum() )
		{
			QMessageBox::warning( this, "Path Error",
				QString( "Hop Router %1, Input Port %2 is not exist." )
					.arg( (int)hoplist[ i ].addr()  )
					.arg( (int)hoplist[ i ].outputPc() ) );
			return false;
		}
		if ( hoplist[i].outputVc() < 0 || hoplist[i].outputVc() >=
			 mp_network_cfg->router( hoplist[ i ].addr() )
				.port( hoplist[ i ].outputPc() ).maxVcNumber() )
		{
			QMessageBox::warning( this, "Path Error",
				QString( "Hop Router %1, Input Port %2, VC %3 is not exist." )
					.arg( (int)hoplist[ i ].addr() )
					.arg( (int)hoplist[ i ].outputPc() )
					.arg( (int)hoplist[ i ].outputVc() ) );
			return false;
		}
	}
	return true;
}

