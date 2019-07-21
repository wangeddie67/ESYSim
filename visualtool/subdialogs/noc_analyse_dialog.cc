#include "noc_analyse_dialog.h"

#define CYCLE_POINT_LIMIT 10000

QStringList AnalyseDialog::const_task_combobox_list = QStringList() <<
	"General\nStatstics" << "Injection\nvs. Time" << "Traffic\nvs. Time" <<
	"Distance\nCount" << "Latency\nCount" << "Traffic\nvs. Area" <<
	"Traffic\nWindow" << "Application\nInformation";

AnalyseDialog::AnalyseDialog() :
	ResultBaseDialog( RESULT_NONE ),
	m_network_cfg(), m_analyse_fname( "" ),
	mp_analyse_benchmark_thread( 0 ), mp_benchmark_reader( 0 ),
	mp_analyse_eventtrace_thread( 0 ), mp_eventtrace_reader( 0 ),
	m_app_event_list( 0 )
{
}
AnalyseDialog::AnalyseDialog(
	ResultTypeEnum type, const EsyNetCfg & cfg
) :
	ResultBaseDialog( type ),
	m_network_cfg( cfg ),
	m_analyse_fname( "" ),
	mp_analyse_benchmark_thread( 0 ),
	mp_benchmark_reader( 0 ),
	mp_analyse_eventtrace_thread( 0 ),
	mp_eventtrace_reader( 0 ),
	m_app_event_list( cfg.routerCount() )
{
	mp_result_plot = new AnalysePlot(
				m_network_cfg.size( EsyNetCfg::AX_X ),
				m_network_cfg.size( EsyNetCfg::AX_Y ),
				this );

	mp_task_combobox = new QComboBox();
	mp_task_combobox->setFixedSize(100, 50);
	for ( int i = 0; i < TASK_COUNT; i ++ )
	{
		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_task_combobox->insertItem( i, const_task_combobox_list[ i ], i );
		}
		if ( ( m_result_type == RESULT_ANALYSE_BENCHMARK &&
			   ( i == GENERAL || i == INJECT_TIME || i == DISTANCE ||
				 i == TRAFFIC_AREA || i == TRAFFIC_WINDOW ) ) )
		{
			mp_task_combobox->insertItem( i, const_task_combobox_list[ i ], i );
		}
		if ( ( m_result_type == RESULT_ANALYSE_WINDOW &&
			   i == TRAFFIC_WINDOW ) )
		{
			mp_task_combobox->insertItem( i, const_task_combobox_list[ i ], i );
		}
	}
	connect( mp_task_combobox, SIGNAL( currentIndexChanged(int) ),
			 this, SLOT( taskChangedHandler(int) ) );

	mp_traffic_window_label = new QLabel( "Window", this );
	mp_traffic_window_edit = new QLineEdit( "1000", this );
	mp_traffic_window_edit->setAlignment( Qt::AlignRight );
	mp_traffic_window_unit_label = new QLabel( "Cycle", this );
	mp_traffic_step_label = new QLabel( "Step", this );
	mp_traffic_step_edit = new QLineEdit( "1000", this );
	mp_traffic_step_edit->setAlignment( Qt::AlignRight );
	mp_traffic_step_unit_label = new QLabel( "Cycle", this );
	if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
		mp_traffic_window_edit->setReadOnly( true );
		mp_traffic_step_edit->setReadOnly( true );
	}

	QGridLayout * traffic_window_layout = new QGridLayout();
	traffic_window_layout->addWidget( mp_task_combobox, 0, 0, 2, 1 );
	traffic_window_layout->addWidget( mp_traffic_window_label,	  0, 1, 1, 1);
	traffic_window_layout->addWidget( mp_traffic_window_edit,	   0, 2, 1, 1);
	traffic_window_layout->addWidget( mp_traffic_window_unit_label, 0, 3, 1, 1);
	traffic_window_layout->addWidget( mp_traffic_step_label,	  1, 1, 1, 1);
	traffic_window_layout->addWidget( mp_traffic_step_edit,	   1, 2, 1, 1);
	traffic_window_layout->addWidget( mp_traffic_step_unit_label, 1, 3, 1, 1);
	traffic_window_layout->setSpacing( 5 );

	ToolBarBox * task_box = new ToolBarBox(
		"Analyse Task", -1, this );
	task_box->hBoxLayout()->addLayout( traffic_window_layout );

	mp_filter_panel = new AnalyseFilterPanel( &m_network_cfg, this );

	mp_control_panel = new AutomationControlPanel(
		AutomationControlPanel::AnalyseControl, "", "", this );
	connect( mp_control_panel, SIGNAL( analyseButtonClicked() ),
			 this, SLOT( analyseStartHandler() ) );

	mp_traffic_draw_label = new QLabel( "Figure Type" );
	mp_traffic_draw_combobox = new QComboBox();
	mp_traffic_draw_combobox ->addItems(
				QStringList() << "Bar chart" << "Spectrum" );
	connect( mp_traffic_draw_combobox, SIGNAL( currentIndexChanged(int) ),
			 this, SLOT( drawPlotHandler() ) );

	mp_traffic_type_label = new QLabel( "Traffic" );
	mp_traffic_type_combobox = new QComboBox();

	mp_traffic_type_combobox ->addItems( QStringList() <<
		"Injection" << "Accepted" );
	if ( m_result_type == RESULT_ANALYSE_EVENTTRACE ||
		 m_result_type == RESULT_ANALYSE_WINDOW )
	{
		mp_traffic_type_combobox ->addItems( QStringList() <<
			"Through" );
	}
	connect( mp_traffic_type_combobox, SIGNAL( currentIndexChanged(int) ),
			 this, SLOT( drawPlotHandler() ) );

	mp_gantt_app_label = new QLabel( "App Id" );
	mp_gantt_app_combobox = new QComboBox();
	mp_gantt_app_combobox ->addItem( "All" );
	connect( mp_gantt_app_combobox, SIGNAL( currentIndexChanged(int) ),
			 this, SLOT( drawPlotHandler() ) );

	QGridLayout * figure_config_layout = new QGridLayout() ;
	figure_config_layout->addWidget( mp_traffic_draw_label, 0, 0, 1, 1);
	figure_config_layout->addWidget( mp_traffic_draw_combobox, 0, 1, 1, 1);
	figure_config_layout->addWidget( mp_traffic_type_label, 1, 0, 1, 1);
	figure_config_layout->addWidget( mp_traffic_type_combobox, 1, 1, 1, 1);
	figure_config_layout->addWidget( mp_gantt_app_label, 0, 2, 1, 1 );
	figure_config_layout->addWidget( mp_gantt_app_combobox, 0, 3, 1, 1 );
	figure_config_layout->setSpacing( 5 );

	mp_figure_config_box = new ToolBarBox(
		"Figure Configure", -1, this );
	mp_figure_config_box->hBoxLayout()->addLayout( figure_config_layout );
	mp_figure_config_box->setVisible( false );

	mp_window_control_panel = new AutomationControlPanel(
		AutomationControlPanel::ReplayControl, "ms", "Step", this);
	mp_window_control_panel->setVisible( false );
	connect( mp_window_control_panel, SIGNAL(prevRunButtonClicked()), this,
			 SLOT( prevRun() ) );
	connect( mp_window_control_panel, SIGNAL(previousButtonClicked()), this,
			 SLOT( prevCycle() ) );
	connect( mp_window_control_panel, SIGNAL(stopButtonClicked()), this,
			 SLOT( stopRun() ) );
	connect( mp_window_control_panel, SIGNAL(nextButtonClicked()), this,
			 SLOT (nextCycle() ) );
	connect( mp_window_control_panel, SIGNAL(nextrunButtonClicked()), this,
			 SLOT( nextRun() ) );
	connect( mp_window_control_panel, SIGNAL(jumpButtonClicked()), this,
			 SLOT( jumpRun() ) );

	connect( mp_window_control_panel, SIGNAL( nextRunTimeOut() ), this, SLOT( nextRun() ) );
	connect( mp_window_control_panel, SIGNAL( prevRunTimeOut() ), this, SLOT( prevRun() ) );

	mp_axis_config_panel = new PlotAxisConfigPanel( this );
	connect( mp_result_plot, SIGNAL(axisScaleChanged(int, bool, QwtInterval)),
			 mp_axis_config_panel, SLOT(axisScaleHandler(int, bool, QwtInterval)) );
	connect( mp_axis_config_panel, SIGNAL(updateAxis(int,double,double)),
			 mp_result_plot, SLOT(updateAxesHandler(int,double,double)) );
	mp_axis_config_panel->setVisible( false );

	mp_size_config_panel = new PlotSizeConfigPanel( this );
	connect( mp_size_config_panel, SIGNAL(editingFinished()),
			 this, SLOT(plotSizeEditingFinishedHandler()) );
	connect( mp_result_plot, SIGNAL(sizeChanged()),
			 this, SLOT(plotResizeEventHandler()) );

	ResultTableView * result_table = new ResultTableView( this );
	mp_result_model = new ResultTableModel( this );
	result_table->setModel( mp_result_model );

	ToolBarButton * save_picture_button = new ToolBarButton(
		QIcon( "images/picture.png" ), "Picture", -1,
		ToolBarButton::LargeButton );
	connect( save_picture_button, SIGNAL(clicked()), this,
			 SLOT( savePicHandler() ) );
	ToolBarButton * save_data_button = new ToolBarButton(
		QIcon( "images/xls.png" ), "Data", -1,
		ToolBarButton::LargeButton );
	connect( save_data_button, SIGNAL(clicked()), this,
			 SLOT( saveDataHandler() ) );
	ToolBarBox * export_box = new ToolBarBox( "Export", -1, this );
	export_box->addLargeButton( save_picture_button );
	export_box->addLargeButton( save_data_button );

	ToolBarTab * control_tab = new ToolBarTab( "Control", 100, this );
	control_tab->addToolBarBox( task_box );
	control_tab->addToolBarBox( mp_filter_panel );
	control_tab->addToolBarBox( mp_control_panel );
	control_tab->addToolBarBox( mp_window_control_panel );
	mp_toolbar_tab_list.append( control_tab );

	ToolBarTab * axis_tab = new ToolBarTab( "Figure", 101, this );
	axis_tab->addToolBarBox( mp_figure_config_box );
	axis_tab->addToolBarBox( mp_axis_config_panel );
	axis_tab->addToolBarBox( mp_size_config_panel );
	axis_tab->addToolBarBox( export_box );
	mp_toolbar_tab_list.append( axis_tab );

	QVBoxLayout * config_layout = new QVBoxLayout();
	config_layout->addWidget( new QLabel( "Analyse Result" ) );
	config_layout->addWidget( result_table );

	QDialog * config_widget = new QDialog();
	config_widget->setLayout( config_layout );

	QSplitter * config_splitter = new QSplitter( Qt::Horizontal );
	config_splitter->addWidget( mp_result_plot );
	config_splitter->addWidget( config_widget );
	config_splitter->setStretchFactor( 0, 80 );
	config_splitter->setStretchFactor( 1, 20 );

	QHBoxLayout * dialog_layout = new QHBoxLayout();
	dialog_layout->addWidget( config_splitter );

	setLayout( dialog_layout );

	taskChangedHandler( GENERAL );
}

AnalyseDialog::~AnalyseDialog()
{
	if (mp_analyse_benchmark_thread != NULL)
	{
		mp_analyse_benchmark_thread ->quit();
		mp_analyse_benchmark_thread ->wait();
		delete mp_analyse_benchmark_thread;
	}
	if (mp_benchmark_reader != NULL)
	{
		delete mp_benchmark_reader;
	}
	if (mp_analyse_eventtrace_thread != NULL)
	{
		mp_analyse_eventtrace_thread ->quit();
		mp_analyse_eventtrace_thread ->wait();
		delete mp_analyse_eventtrace_thread;
	}
	if (mp_eventtrace_reader != NULL)
	{
		delete mp_eventtrace_reader;
	}
}

void AnalyseDialog::taskChangedHandler( int index )
{
	AnalyseFilterPanel::FliterFlags flag = 0x00;
	int task_index = mp_task_combobox->itemData( index ).toInt();

	if ( task_index == INJECT_TIME || task_index == DISTANCE || task_index == LATENCY )
	{
		flag |= AnalyseFilterPanel::FLITER_SOUR | AnalyseFilterPanel::FLITER_DEST;
	}
	if ( task_index == TRAFFIC_TIME )
	{
		flag |= AnalyseFilterPanel::FLITER_ROUTER;
	}
	if ( task_index == INJECT_TIME || task_index == TRAFFIC_TIME || task_index == DISTANCE ||
		 task_index == LATENCY || task_index == TRAFFIC_AREA )
	{
		flag |= AnalyseFilterPanel::FLITER_APP;
	}
	mp_filter_panel->setVisibleFlag( flag );

	bool windowenable = (task_index == TRAFFIC_WINDOW);
	mp_traffic_window_label->setVisible( windowenable );
	mp_traffic_window_edit->setVisible( windowenable );
	mp_traffic_window_unit_label->setVisible( windowenable );
	mp_traffic_step_label->setVisible( windowenable );
	mp_traffic_step_edit->setVisible( windowenable );
	mp_traffic_step_unit_label->setVisible( windowenable );
}

void AnalyseDialog::drawPlotHandler()
{
	mp_axis_config_panel->setVisible( m_current_task != GENERAL );

	mp_result_plot->enableAxis( QwtPlot::yLeft, m_current_task != GENERAL );

	bool yrightenable =
		 ( m_current_task == INJECT_TIME || m_current_task == TRAFFIC_TIME ||
		 ( (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
		   mp_traffic_draw_combobox->currentIndex() == 1 ) );
	mp_result_plot->enableAxis( QwtPlot::yRight, yrightenable );

	bool xtopenable = (
		   (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
		   mp_traffic_draw_combobox->currentIndex() == 1 );
	mp_result_plot->enableAxis( QwtPlot::xTop, xtopenable );

	bool xbottomable = ( !xtopenable && (m_current_task != GENERAL) );
	mp_result_plot->enableAxis( QwtPlot::xBottom, xbottomable );

	mp_result_plot->clearCurve();
	if ( m_current_task == INJECT_TIME )
	{
		mp_result_plot->attachCurve( "Packet", QwtPlot::yLeft,
			m_x_inject_time, m_y_inject_packet );
		mp_result_plot->attachCurve( "Flit", QwtPlot::yRight,
			m_x_inject_time, m_y_inject_flit );
	}
	if ( m_current_task == TRAFFIC_TIME )
	{
		mp_result_plot->attachCurve( "Packet Injected in Net.", QwtPlot::yLeft,
			m_x_traffic_time, m_y_traffic_inject );
		mp_result_plot->attachCurve( "Packet Accepted from Net.", QwtPlot::yLeft,
			m_x_traffic_time, m_y_traffic_accept );
		mp_result_plot->attachCurve( "Packet Sent by Routers", QwtPlot::yLeft,
			m_x_traffic_time, m_y_traffic_send );
		mp_result_plot->attachCurve( "Packet Received by Routers", QwtPlot::yLeft,
			m_x_traffic_time, m_y_traffic_receive );
		mp_result_plot->attachCurve( "Packet Buffered in Routers", QwtPlot::yRight,
			m_x_traffic_time, m_y_traffic_buffer );
	}
	if ( m_current_task == DISTANCE )
	{
		mp_result_plot->attachBarChart( "Counter", m_y_dist_count );
	}
	if ( m_current_task == LATENCY )
	{
		mp_result_plot->attachCurve( "Counter", QwtPlot::yLeft,
			m_x_lat_lat, m_y_lat_count );
	}
	if ( (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
		 mp_traffic_draw_combobox->currentIndex() == 0 )
	{
		switch ( mp_traffic_type_combobox->currentIndex() )
		{
		case 0:
			mp_result_plot->attachBarChart( "Injection", m_y_area_inject );
			break;
		case 1:
			mp_result_plot->attachBarChart( "Accept", m_y_area_accept );
			break;
		case 2:
			mp_result_plot->attachBarChart( "Through", m_y_area_through );
			break;
		}
	}
	if ( (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
		 mp_traffic_draw_combobox->currentIndex() == 1 )
	{
		switch ( mp_traffic_type_combobox->currentIndex() )
		{
		case 0:
			mp_result_plot->attachSpectrogram( m_y_area_inject,
				m_network_cfg.size( EsyNetCfg::AX_X ) );
			break;
		case 1:
			mp_result_plot->attachSpectrogram( m_y_area_accept,
				m_network_cfg.size( EsyNetCfg::AX_X ) );
			break;
		case 2:
			mp_result_plot->attachSpectrogram( m_y_area_through,
				m_network_cfg.size( EsyNetCfg::AX_X ) );
			break;
		}
	}
	if ( m_current_task == APPLICATION )
	{
		mp_result_plot->attachGantt( m_app_event_list, mp_gantt_app_combobox->currentIndex() - 1 );
	}

	bool wheelenable =
	   ( m_current_task == INJECT_TIME || m_current_task == TRAFFIC_TIME ||
		 m_current_task == DISTANCE || m_current_task == LATENCY ||
		 ( (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
		   mp_traffic_draw_combobox->currentIndex() == 0 ) ||
		 m_current_task == APPLICATION );
	mp_result_plot->setWheelEnable( wheelenable );

	bool matrixenable = (
		   (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
		   mp_traffic_draw_combobox->currentIndex() == 1 );
	mp_result_plot->setMatrixPickerEnable( matrixenable );

	bool ganttenable = ( m_current_task == APPLICATION );
	mp_result_plot->setGanttEnable( ganttenable );

	bool barchartenable = ( m_current_task == DISTANCE ||
		( (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
			mp_traffic_draw_combobox->currentIndex() == 0) );
	mp_result_plot->setBarChartEnable( barchartenable );

	bool trafficdrawenable = (
		 (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) &&
		 ( m_network_cfg.topology() ==
		   EsyNetCfg::NOC_TOPOLOGY_2DMESH ||
		   m_network_cfg.topology() ==
		   EsyNetCfg::NOC_TOPOLOGY_2DTORUS ) );
	mp_traffic_draw_label->setVisible( trafficdrawenable );
	mp_traffic_draw_combobox->setVisible( trafficdrawenable );

	bool traffictypeenable = (
		 (m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW) );
	mp_traffic_type_label->setVisible( traffictypeenable );
	mp_traffic_type_combobox->setVisible( traffictypeenable );

	bool ganttappenable = ( m_current_task == APPLICATION );
	mp_gantt_app_label->setVisible( ganttappenable );
	mp_gantt_app_combobox->setVisible( ganttappenable );

	bool windowenable = (m_current_task == TRAFFIC_WINDOW);
	mp_window_control_panel->setVisible( windowenable );

	mp_figure_config_box->setVisible( trafficdrawenable || traffictypeenable ||
									 ganttappenable || windowenable );

	mp_result_plot->setTitle( "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::xTop, "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft, "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::yRight, "" );
	switch ( m_current_task )
	{
	case GENERAL: break;
	case INJECT_TIME:
		mp_result_plot->setTitle( "Injection vs. Time" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "Time", "Cycle" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft,
						"Injected Packets", "Packet" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::yRight,
						"Injected Flits", "Flit" );
		break;
	case TRAFFIC_TIME:
		mp_result_plot->setTitle( "Traffic vs. Time" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "Time", "Cycle" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft,
						"Traffic", "Flit" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::yRight,
						"Flits in Buffer", "Flit" );
		break;
	case DISTANCE:
		mp_result_plot->setTitle( "Manhatton Distance Distribution" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "Distance" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft,
					"Packets Counter", "Packet" );
		break;
	case LATENCY:
		mp_result_plot->setTitle( "Latency Distribution" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom,
						"Latency", "Cycle" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft,
					"Packet Counter", "Packet" );
		break;
	case TRAFFIC_AREA:
	case TRAFFIC_WINDOW:
		mp_result_plot->setTitle( "Traffic vs. Area" );
		if ( mp_traffic_draw_combobox->currentIndex() == 0 )
		{
			mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "Router ID" );
			mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft,
							"Flits Counter", "Flit" );
		}
		else if ( mp_traffic_draw_combobox->currentIndex() == 1 )
		{
			mp_result_plot->setAxisTitleUnit( QwtPlot::xTop, "X Axis" );
			mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft, "Y Axis" );
			switch ( mp_traffic_type_combobox->currentIndex() )
			{
			case 0:
				mp_result_plot->setAxisTitleUnit( QwtPlot::yRight,
						  "Injected Flits", "Flit" );
				break;
			case 1:
				mp_result_plot->setAxisTitleUnit( QwtPlot::yRight,
						  "Accepted Flits", "Flit" );
				break;
			case 2:
				mp_result_plot->setAxisTitleUnit( QwtPlot::yRight,
						  "Switched Flits", "Flit" );
				break;
			}
		}
		break;
	case APPLICATION:
		mp_result_plot->setTitle( "Gantt Chart for Application" );
		mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "Time", "Cycle" );
		mp_result_plot->setAxisTitleUnit(QwtPlot::yLeft, "Process Element ID", "");
		break;
	default:
		break;
	}

	mp_result_plot->setFixedZAxis( false, QwtInterval() );
	mp_result_plot->setFixedYLeftAxis( false, QwtInterval() );
	if ( m_current_task == TRAFFIC_WINDOW &&
		 mp_traffic_draw_combobox->currentIndex() == 0 )
	{
		switch ( mp_traffic_type_combobox->currentIndex() )
		{
		case 0:
			mp_result_plot->setFixedYLeftAxis( true, QwtInterval( 0, m_max_inject ) );
			break;
		case 1:
			mp_result_plot->setFixedYLeftAxis( true, QwtInterval( 0, m_max_accept ) );
			break;
		case 2:
			mp_result_plot->setFixedYLeftAxis( true, QwtInterval( 0, m_max_through ) );
			break;
		}
	}
	else if ( m_current_task == TRAFFIC_WINDOW &&
		 mp_traffic_draw_combobox->currentIndex() == 1 )
	{
		switch ( mp_traffic_type_combobox->currentIndex() )
		{
		case 0:
			mp_result_plot->setFixedZAxis( true, QwtInterval( 0, m_max_inject ) );
			break;
		case 1:
			mp_result_plot->setFixedZAxis( true, QwtInterval( 0, m_max_accept ) );
			break;
		case 2:
			mp_result_plot->setFixedZAxis( true, QwtInterval( 0, m_max_through ) );
			break;
		}
	}

	mp_result_plot->fitWindow();
	mp_result_plot->update();
}

bool AnalyseDialog::setAnalyseFile( QString & fname )
{
	m_analyse_fname = fname;
	string filepath_str = m_analyse_fname.
			left( m_analyse_fname.lastIndexOf( "." ) ).toStdString();
	
	if ( m_result_type == RESULT_ANALYSE_BENCHMARK )
	{
		if ( m_analyse_fname .right( 1 ) == "b" )
		{
			mp_benchmark_reader = new EsyDataFileIStream< EsyDataItemBenchmark >(
				10000, filepath_str, BENCHMARK_EXTENSION, true );
		}
		else
		{
			mp_benchmark_reader = new EsyDataFileIStream< EsyDataItemBenchmark >(
				10000, filepath_str, BENCHMARK_EXTENSION, false );
		}
		if ( !mp_benchmark_reader )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input trace file (T1) " + fname );
			return false;
		}
		if ( !mp_benchmark_reader ->openSuccess() )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input trace file (T2) " + fname );
			return false;
		}	
		mp_control_panel->setLoadProgressBarRange( 0, mp_benchmark_reader->size() );
	}
	else if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
	{
		if ( m_analyse_fname .right( 1 ) == "b" )
		{
			mp_eventtrace_reader = new EsyDataFileIStream< EsyDataItemEventtrace >(
				10000, filepath_str, EVENTTRACE_EXTENSION, true );
		}
		else
		{
			mp_eventtrace_reader = new EsyDataFileIStream< EsyDataItemEventtrace >(
				10000, filepath_str, EVENTTRACE_EXTENSION, false );
		}
		if ( !mp_eventtrace_reader )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input trace file (T1) " + fname );
			return false;
		}
		if ( !mp_eventtrace_reader ->openSuccess() )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input trace file (T2) " + fname );
			return false;
		}	
		mp_control_panel->setLoadProgressBarRange( 0, mp_eventtrace_reader->size() );
	}
	else if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
		if ( m_analyse_fname .right( 1 ) == "b" )
		{
			mp_trafficwindow_reader = new EsyDataFileIStream< EsyDataItemTrafficWindow >(
				1000, filepath_str, WINDOW_EXTENSION, true );
		}
		else
		{
			mp_trafficwindow_reader = new EsyDataFileIStream< EsyDataItemTrafficWindow >(
				1000, filepath_str, WINDOW_EXTENSION, false );
		}
		if ( !mp_trafficwindow_reader )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input traffic window file (T1) " + fname );
			return false;
		}
		if ( !mp_trafficwindow_reader ->openSuccess() )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input trace file (T2) " + fname );
			return false;
		}
		mp_control_panel->setLoadProgressBarRange( 0, mp_trafficwindow_reader->size() );
	}
	return true;
}

void AnalyseDialog::analyseStartHandler()
{
	AnalyseTaskEnum old_task = m_current_task;
	m_current_task = (AnalyseTaskEnum)mp_task_combobox->itemData(
				mp_task_combobox->currentIndex() ).toInt();
	if ( old_task == m_current_task )
	{
		mp_window_control_panel->stopButtonClickedHandler();
		mp_window_control_panel->enableAllComponent( false );
	}

	if ( m_current_task == GENERAL )
	{
		if ( m_result_type == RESULT_ANALYSE_BENCHMARK )
		{
			mp_analyse_benchmark_thread = new AnalyseBenchmarkThread(
				mp_benchmark_reader, &m_network_cfg,
				&m_total_packets_inject, &m_total_flits_inject,
				&m_total_inject_cycle, &m_total_distance, &m_max_distance
			);
		}
		else if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_analyse_eventtrace_thread = new AnalyseEventtraceThread(
				mp_eventtrace_reader, &m_network_cfg,
				&m_total_packets_inject, &m_total_packets_received,
				&m_total_flits_inject, &m_total_flits_received,
				&m_total_cycle, &m_total_inject_cycle,
				&m_total_distance, &m_max_distance,
				&m_total_latency, &m_max_latency,
				&m_total_app_req, &m_total_app_start, &m_total_app_exit,
				&finish_app_list);
		}
		
	}
	else if (m_current_task == INJECT_TIME)
	{
		int src_id = mp_filter_panel->srcId();
		int dst_id = mp_filter_panel->dstId();
		int app_id = mp_filter_panel->appId();
		if ( m_result_type == RESULT_ANALYSE_BENCHMARK )
		{
			mp_analyse_benchmark_thread = new AnalyseBenchmarkThread(
				mp_benchmark_reader, &m_network_cfg,
				src_id, dst_id, 0, m_total_inject_cycle,
				&m_y_inject_packet, &m_y_inject_flit
			); 
		}
		else if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_analyse_eventtrace_thread = new AnalyseEventtraceThread(
				mp_eventtrace_reader, &m_network_cfg,
				src_id, dst_id, app_id, m_total_inject_cycle,
                m_inject_cycle_point, m_inject_cycle_scaling,
				&m_y_inject_packet, &m_y_inject_flit
			); 
		}
	}
	else if (m_current_task == TRAFFIC_TIME)
	{
		int id = mp_filter_panel->routerId();
		int app_id = mp_filter_panel->appId();
		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_analyse_eventtrace_thread = new AnalyseEventtraceThread(
				mp_eventtrace_reader, &m_network_cfg,
				id, app_id, m_total_cycle,
                m_total_cycle_point, m_total_cycle_scaling,
				&m_y_traffic_inject, &m_y_traffic_accept,
				&m_y_traffic_send, &m_y_traffic_receive
			); 
		}
	}
	else if (m_current_task == DISTANCE || m_current_task == LATENCY )
	{
		int src_id = mp_filter_panel->srcId();
		int dst_id = mp_filter_panel->dstId();
		if ( m_result_type == RESULT_ANALYSE_BENCHMARK )
		{
			mp_analyse_benchmark_thread = new AnalyseBenchmarkThread(
				mp_benchmark_reader, &m_network_cfg,
				src_id, dst_id, 0, m_total_inject_cycle, &m_max_distance,
				&m_y_dist_count
			); 
		}
		else if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			int app_id = mp_filter_panel->appId();
			mp_analyse_eventtrace_thread = new AnalyseEventtraceThread(
				mp_eventtrace_reader, &m_network_cfg,
				src_id, dst_id, app_id,
				m_max_distance, m_max_latency, &m_y_dist_count, &m_y_lat_count
			); 
		}
	}
	else if (m_current_task == TRAFFIC_AREA)
	{
		if (m_result_type == RESULT_ANALYSE_BENCHMARK)
		{
			int id = mp_filter_panel->routerId();
			mp_analyse_benchmark_thread = new AnalyseBenchmarkThread(
				mp_benchmark_reader, &m_network_cfg,
				id, 0, m_total_inject_cycle,
				&m_y_area_inject, &m_y_area_accept
			);
		}
		else if (m_result_type == RESULT_ANALYSE_EVENTTRACE)
		{
			int app_id = mp_filter_panel->appId();
			mp_analyse_eventtrace_thread = new AnalyseEventtraceThread(
				mp_eventtrace_reader, &m_network_cfg, app_id,
				&m_y_area_inject, &m_y_area_accept, &m_y_area_through
			); 
		}
	}
	else if (m_current_task == TRAFFIC_WINDOW)
	{
		m_window_width = mp_traffic_window_edit->text().toInt();
		m_window_step = mp_traffic_step_edit->text().toInt();
		m_total_window = (m_total_cycle - m_window_width) / m_window_step;

		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_analyse_eventtrace_thread = new AnalyseEventtraceThread(
			   mp_eventtrace_reader, &m_network_cfg,
				m_window_step, m_window_width, m_total_cycle,
				&m_y_window_inject, &m_y_window_accept, &m_y_window_through
			);
		}
		else if ( m_result_type == RESULT_ANALYSE_WINDOW )
		{
			mp_analyse_trafficwindow_thread = new AnalyseTrafficWindowThread(
				mp_trafficwindow_reader, &m_network_cfg,
				&m_total_window, &m_window_width, &m_window_step,
				&m_max_inject, &m_max_accept, &m_max_through
			);
		}
	}
	else if (m_current_task == APPLICATION)
	{
		mp_analyse_eventtrace_thread = new AnalyseEventtraceThread(
			mp_eventtrace_reader, &m_network_cfg,
			-1, m_total_cycle, m_total_app_req,
			&m_app_req_time_list, &m_app_start_time_list, &m_app_exit_time_list,
			&m_app_event_list,
			&finish_app_list,
			&latency_list,
			&distance_list_AWMD,
			&distance_list_AMD);
	}
	
	if (m_result_type == RESULT_ANALYSE_BENCHMARK)
	{
		connect(mp_analyse_benchmark_thread, SIGNAL(finished()),
				this, SLOT(analyseFinishedHandler()));
		connect(mp_analyse_benchmark_thread, SIGNAL(progress(int)),
				mp_control_panel, SLOT(setLoadProgressBarValue(int)));
		mp_analyse_benchmark_thread ->start();
	}
	else if (m_result_type == RESULT_ANALYSE_EVENTTRACE )
	{
		connect(mp_analyse_eventtrace_thread, SIGNAL(finished()),
				this, SLOT(analyseFinishedHandler()));
		connect(mp_analyse_eventtrace_thread, SIGNAL(progress(int)),
				mp_control_panel, SLOT(setLoadProgressBarValue(int)));
		mp_analyse_eventtrace_thread ->start();
	}
	else if (m_result_type == RESULT_ANALYSE_WINDOW )
	{
		connect(mp_analyse_trafficwindow_thread, SIGNAL(finished()),
				this, SLOT(analyseFinishedHandler()));
		connect(mp_analyse_trafficwindow_thread, SIGNAL(progress(int)),
				mp_control_panel, SLOT(setLoadProgressBarValue(int)));
		mp_analyse_trafficwindow_thread ->start();
	}
}

void AnalyseDialog::analyseFinishedHandler()
{
	mp_control_panel->enableAllComponent( true );

	if ( m_result_type == RESULT_ANALYSE_BENCHMARK )
	{
		if (mp_analyse_benchmark_thread->isGood() == false)
		{
			delete mp_analyse_benchmark_thread;
			mp_analyse_benchmark_thread = NULL;
			return;
		}
		delete mp_analyse_benchmark_thread;
		mp_analyse_benchmark_thread = NULL;
		mp_control_panel->setLoadProgressBarValue( mp_benchmark_reader->size() );
	}
	else if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
	{
		if (mp_analyse_eventtrace_thread->isGood() == false)
		{
			delete mp_analyse_eventtrace_thread;
			mp_analyse_eventtrace_thread = NULL;
			return;
		}
		delete mp_analyse_eventtrace_thread;
		mp_analyse_eventtrace_thread = NULL;
		mp_control_panel->setLoadProgressBarValue( mp_eventtrace_reader->size() );
	}
	else if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
		if (mp_analyse_trafficwindow_thread->isGood() == false)
		{
			delete mp_analyse_trafficwindow_thread;
			mp_analyse_trafficwindow_thread = NULL;
			return;
		}
		delete mp_analyse_trafficwindow_thread;
		mp_analyse_trafficwindow_thread = NULL;
		mp_control_panel->setLoadProgressBarValue( mp_trafficwindow_reader->size() );

		mp_traffic_window_edit->setText( QString::number( m_window_width ) );
		mp_traffic_step_edit->setText( QString::number( m_window_step ) );
	}

	if ( m_current_task == GENERAL )
	{
		if ( m_result_type == RESULT_ANALYSE_BENCHMARK )
		{
			m_total_cycle = m_total_inject_cycle;
		}

        if ( m_total_cycle > CYCLE_POINT_LIMIT )
        {
            m_total_cycle_scaling = qRound( (double)(m_total_cycle) / CYCLE_POINT_LIMIT );
            m_total_cycle_point = m_total_cycle / m_total_cycle_scaling;
        }
        else
        {
            m_total_cycle_scaling = 1;
            m_total_cycle_point = m_total_cycle;
        }

        if ( m_total_inject_cycle > CYCLE_POINT_LIMIT )
        {
            m_inject_cycle_scaling = qRound( (double)(m_total_inject_cycle) / CYCLE_POINT_LIMIT );
            m_inject_cycle_point = m_total_inject_cycle / m_inject_cycle_scaling;
        }
        else
        {
            m_inject_cycle_scaling = 1;
            m_inject_cycle_point = m_total_inject_cycle;
        }

		// update data filed;

        m_x_inject_time.resize( m_inject_cycle_point + 1 );
        for ( int i = 0; i < m_inject_cycle_point + 1; i ++ )
		{
            m_x_inject_time[ i ] = i * m_inject_cycle_scaling;
		}
        m_x_traffic_time.resize( m_total_cycle_point + 1);
        for ( int i = 0; i < m_total_cycle_point + 1; i ++ )
		{
            m_x_traffic_time[ i ] = i * m_total_cycle_scaling;
		}
		m_x_dist_dist.resize( m_max_distance + 1);
		for ( int i = 0; i < m_max_distance + 1; i ++ )
		{
			m_x_dist_dist[ i ] = i;
		}
		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			m_x_lat_lat.resize( m_max_latency + 1);
			for ( int i = 0; i < m_max_latency + 1; i ++ )
			{
				m_x_lat_lat[ i ] = i;
			}
		}
		m_x_area_router.resize( m_network_cfg .routerCount() );
		for ( int i = 0; i < m_network_cfg .routerCount(); i ++ )
		{
			m_x_area_router[ i ] = i;
		}

		mp_filter_panel->setAppCount( m_total_app_start );
		mp_gantt_app_combobox->clear();
		mp_gantt_app_combobox->addItem( "All" );
		for ( int i = 0; i < m_total_app_start; i ++ )
		{
			mp_gantt_app_combobox->addItem( QString::number( i ) );
		}
	}
	else if (m_current_task == TRAFFIC_TIME)
	{
		m_y_traffic_buffer.resize( m_y_traffic_send .size() );
		m_y_traffic_buffer[ 0 ] = 0.0 +
				m_y_traffic_send[ 0 ] - m_y_traffic_receive[ 0 ];
		for ( int i = 1; i < m_y_traffic_inject.size(); i ++ )
		{
			m_y_traffic_buffer[ i ] = m_y_traffic_buffer[ i - 1 ] +
					m_y_traffic_send[ i ] - m_y_traffic_receive[ i ];
		}
	}
	else if (m_current_task == TRAFFIC_WINDOW )
	{
		if ( m_result_type != RESULT_ANALYSE_WINDOW )
		{
			m_max_inject = 0;
			m_max_accept = 0;
			m_max_through = 0;
			for ( int i = 0; i < m_total_window; i ++ )
			{
				for ( int r = 0; r < m_network_cfg.routerCount(); r ++ )
				{
					if ( m_y_window_inject[ r ][ i ] > m_max_inject )
					{
						m_max_inject = m_y_window_inject[ r ][ i ] ;
					}
					if ( m_y_window_accept[ r ][ i ]  > m_max_accept )
					{
						m_max_accept = m_y_window_accept[ r ][ i ];
					}
					if ( m_y_window_through[ r ][ i ]  > m_max_through )
					{
						m_max_through = m_y_window_through[ r ][ i ] ;
					}
				}
			}
		}

		m_current_window = -1;
		if ( m_result_type == RESULT_ANALYSE_WINDOW )
		{
			m_x_area_router.resize( m_network_cfg .routerCount() );
			for ( int i = 0; i < m_network_cfg .routerCount(); i ++ )
			{
				m_x_area_router[ i ] = i;
			}
			mp_trafficwindow_reader->reset();
			m_total_window --;
		}
		nextCycle();
		mp_window_control_panel->setLoadProgressBarRange( 0, m_total_window );

		m_run_error_flag = false;
		m_jump_flag = false;

		mp_window_control_panel->enableAllComponent( true );
	}
	else if (m_current_task == APPLICATION)
	{
		m_app_id_list.resize( m_total_app_req );
		for ( int i = 0; i < m_total_app_req; i ++ )
		{
			m_app_id_list[ i ] = i;
		}
	}

	drawPlotHandler();
	updateResultTable();
}

void AnalyseDialog::updateResultTable()
{
	mp_result_model->clearData();
	if ( m_current_task == GENERAL )
	{
		mp_result_model->setHeaderList(
					QStringList() << "Name" << "Value" << "Unit" );

		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_result_model->appendDataItem(
						"Total Cycle", m_total_cycle, "cycles" );
		}
		mp_result_model->appendDataItem( "Total Injection Cycle",
			m_total_inject_cycle, "cycles" );
		mp_result_model->appendDataItem( "Total Packets Inject",
			m_total_packets_inject, "packets"  );
		mp_result_model->appendDataItem( "Total Flits Inject",
			m_total_flits_inject, "flits" );
		mp_result_model->appendDataItem( "Total Packet Inject Rate",
			(double)m_total_packets_inject/(double)m_total_cycle ,
			"packets/cycle" );
		mp_result_model->appendDataItem( "Total Flit Inject Rate",
			(double)m_total_flits_inject/(double)m_total_cycle,
			"flits/cycle" );
		mp_result_model->appendDataItem( "Average Packet Inject Rate",
			(double)m_total_packets_inject/(double)m_total_cycle/
				m_network_cfg .routerCount(),
			"packets/cycle/router" );
		mp_result_model->appendDataItem( "Average Flit Inject Rate",
			(double)m_total_flits_inject/(double)m_total_cycle/
				 m_network_cfg .routerCount(),
			"flits/cycle/router" );

		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_result_model->appendDataItem( "Total Packets Received",
				m_total_packets_received, "packets" );
			mp_result_model->appendDataItem( "Total Flits Received",
				m_total_flits_received, "flits" );
			mp_result_model->appendDataItem( "Packets Throughput",
				(double)m_total_packets_received/(double)m_total_cycle,
				"packet/cycle" );
			mp_result_model->appendDataItem( "Flits Throughput",
				(double)m_total_flits_received/(double)m_total_cycle,
				"flit/cycle" );
		}

		mp_result_model->appendDataItem( "Average Packet Length",
			(double)m_total_flits_inject / (double)m_total_packets_inject,
			"flits" );
		mp_result_model->appendDataItem( "Average Distance",
			(double)m_total_distance / (double)m_total_packets_inject,
			"" );
		mp_result_model->appendDataItem( "Max Distance", m_max_distance, "" );

		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_result_model->appendDataItem( "Average Latency",
				m_total_latency / m_total_packets_received,"cycles" );
			mp_result_model->appendDataItem( "Max Latency",
				m_max_latency, "cycles" );
		}

		if (m_result_type == RESULT_ANALYSE_EVENTTRACE)
				{
					//use finish_app_list to compute the 6 value
					// long m_total_app_req_new = 0;
					// long m_total_app_start_new = 0;
					// long m_total_app_exit_new = 0;
					double m_total_exec_time = 0; //for finished app
					double m_total_turn_time = 0; //for finished app
					double m_total_wait_time = 0; //for finished app
					if (finish_app_list.size() != 0)
					{
						for (QMap<long, QVector<double> >::iterator it = finish_app_list.begin();
							 it != finish_app_list.end();
							 ++it)
						{
							if ((it.value())[0] != -1)
							{
								//   m_total_app_req_new++;
							}
							if ((it.value())[1] != -1)
							{
								//	 m_total_app_start_new++;
							}
							if ((it.value())[2] != -1)
							{
								//	m_total_app_exit_new++;
								m_total_exec_time = m_total_exec_time + (it.value())[2] - (it.value())[1];
								m_total_turn_time = m_total_turn_time + (it.value())[2] - (it.value())[0];
								m_total_wait_time = m_total_wait_time + (it.value())[1] - (it.value())[0];
							}
						}
					}
					mp_result_model->appendDataItem("Total Application Request",
													m_total_app_req, "");
					mp_result_model->appendDataItem("Total Application Enter",
													m_total_app_start, "");
					mp_result_model->appendDataItem("Total Application Exit",
													m_total_app_exit, "");
					mp_result_model->appendDataItem("Execution Time(avg.)",
													m_total_exec_time / m_total_app_exit, "cycles");
					mp_result_model->appendDataItem("Turnaround Time(avg.)",
													m_total_turn_time / m_total_app_exit, "cycles");
					mp_result_model->appendDataItem("Waiting Sche Time(avg.)",
													m_total_wait_time / m_total_app_exit, "cycles");
				}
	}
	else if ( m_current_task == INJECT_TIME )
	{
		mp_result_model->appendDataVector( "Time", m_x_inject_time );
		mp_result_model->appendDataVector( "Packet", m_y_inject_packet );
		mp_result_model->appendDataVector( "Flit", m_y_inject_flit );
	}
	else if (m_current_task == TRAFFIC_TIME)
	{
		mp_result_model->appendDataVector( "Time", m_x_traffic_time );
		mp_result_model->appendDataVector( "Injected", m_y_traffic_inject );
		mp_result_model->appendDataVector( "Received", m_y_traffic_accept );
		mp_result_model->appendDataVector( "Send", m_y_traffic_send );
		mp_result_model->appendDataVector( "Receive", m_y_traffic_receive );
		mp_result_model->appendDataVector( "Buffered", m_y_traffic_buffer );
	}
	else if (m_current_task == DISTANCE)
	{
		mp_result_model->appendDataVector( "Distance", m_x_dist_dist );
		mp_result_model->appendDataVector( "Counter", m_y_dist_count );
	}
	else if (m_current_task == LATENCY)
	{
		mp_result_model->appendDataVector( "Latency", m_x_lat_lat );
		mp_result_model->appendDataVector( "Counter", m_y_lat_count );
	}
	else if ( m_current_task == TRAFFIC_AREA || m_current_task == TRAFFIC_WINDOW  )
	{
		mp_result_model->appendDataVector( "Id", m_x_area_router );
		mp_result_model->appendDataVector( "Injected", m_y_area_inject );
		mp_result_model->appendDataVector( "Accepted", m_y_area_accept );
		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE ||
			 m_result_type == RESULT_ANALYSE_WINDOW )
		{
			mp_result_model->appendDataVector( "Through", m_y_area_through );
		}
	}
	else if (m_current_task == APPLICATION)
	{
		mp_result_model->setHeaderList(QStringList() <<
			"App ID" << "Enter Scheduler\nTime" << "Start\nTime" <<
			"Finish\nTime" << "Execution\nTime" << "Turnaround\nTime" <<
			"AWD" << "AWMD" << "Average\nPacket Latency");
		for (QMap<long, QVector<double> >::iterator it = finish_app_list.begin();
			 it != finish_app_list.end();
			 ++it)
		{
			if ((it.value())[1] < 0)
			{
				mp_result_model->appendDataItem( QStringList() <<
					QString::number(it.key()) <<
					QString::number((it.value())[0]) << " - " << " - " <<
					" - " << " - " << " - " << " - " << " - ");
			}
			else if ((it.value())[2] < 0)
			{
				mp_result_model->appendDataItem( QStringList() <<
					QString::number(it.key()) <<
					QString::number((it.value())[0]) <<
					QString::number((it.value())[1]) <<
					" - " << " - " << " - " <<
					QString::number(distance_list_AMD[it.key()][1] / distance_list_AMD[it.key()][0]) <<
					QString::number(distance_list_AWMD[it.key()][1] / distance_list_AWMD[it.key()][0]) <<
					" - ");
			}
			else
			{
				mp_result_model->appendDataItem( QStringList() <<
					QString::number(it.key()) <<
					QString::number((it.value())[0]) <<
					QString::number((it.value())[1]) <<
					QString::number((it.value())[2]) <<
					QString::number((it.value())[2] - (it.value())[1]) <<
					QString::number((it.value())[2] - (it.value())[0]) <<
					QString::number(distance_list_AMD[it.key()][1] / distance_list_AMD[it.key()][0]) <<
					QString::number(distance_list_AWMD[it.key()][1] / distance_list_AWMD[it.key()][0]) <<
					QString::number(latency_list[it.key()][1] / latency_list[it.key()][0]));
			}
		}
	}
}

void AnalyseDialog::savePicHandler()
{
	QString t_pic_path = QFileDialog::getSaveFileName( this, "Save Picture",
		global_work_direction, "png file (*.png);;All file (*.*)" );
	if ( t_pic_path == "" )
	{
		return;
	}

	QImage t_image( mp_result_plot->rect().width(),
		mp_result_plot->rect().height(), QImage::Format_RGB32 );
	QwtPlotRenderer renderer;
	renderer.renderTo( mp_result_plot, t_image );
	t_image.save( t_pic_path );
}

void AnalyseDialog::saveDataHandler()
{
	QString t_data_path = QFileDialog::getSaveFileName( this,
		"Save Data", global_work_direction,
		"excel file (*.xls);;All file (*.*)" );
	if ( t_data_path == "" )
	{
		return;
	}

	ErrorHandler t_error = mp_result_model->writeDataFile( t_data_path );
	if ( t_error.hasError() )
	{
		QMessageBox::warning( this, t_error.title(), t_error.text() );
		return;
	}
}

// control
// previous cycle
void AnalyseDialog::prevCycle()
{
	// if it is the head
	if ( m_current_window == 0 )
	{
		return;
	}
	m_current_window --;

	m_y_area_inject.resize( m_network_cfg.routerCount() );
	m_y_area_accept.resize( m_network_cfg.routerCount() );
	m_y_area_through.resize( m_network_cfg.routerCount() );
	if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
		EsyDataItemTrafficWindow window_item = mp_trafficwindow_reader->getPrevItem();
		for ( int r = 0; r < m_network_cfg.routerCount(); r ++ )
		{
			m_y_area_inject[ r ]  = window_item.injectTraffic()[ r ];
			m_y_area_accept[ r ]  = window_item.acceptTraffic()[ r ];
			m_y_area_through[ r ] = window_item.throughTraffic()[ r ];
		}
	}
	else
	{
		for ( int r = 0; r < m_network_cfg.routerCount(); r ++ )
		{
			m_y_area_inject[ r ]  = m_y_window_inject[ r ][ m_current_window ];
			m_y_area_accept[ r ]  = m_y_window_accept[ r ][ m_current_window ];
			m_y_area_through[ r ] = m_y_window_through[ r ][ m_current_window ];
		}
	}

	// update show
	mp_window_control_panel->setCycleLabel(
		QString( "%1/%2" ).arg( m_current_window ).arg( m_total_window ) );
	mp_window_control_panel->setLoadProgressBarValue( m_current_window );
	if ( !m_jump_flag )
	{
		drawPlotHandler();
		updateResultTable();
	}
	mp_result_plot->setTitle( QString("Traffic vs Area (%1 - %2)")
		.arg(m_current_window * m_window_step).arg(m_current_window * m_window_step + m_window_width) );
}
// next cycle
void AnalyseDialog::nextCycle()
{
	// if it is the end
	if ( m_current_window + 1 > m_total_window )
	{
		return;
	}
	// read to next cycle and handle event
	m_current_window ++;

	m_y_area_inject.resize( m_network_cfg.routerCount() );
	m_y_area_accept.resize( m_network_cfg.routerCount() );
	m_y_area_through.resize( m_network_cfg.routerCount() );
	if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
		EsyDataItemTrafficWindow window_item = mp_trafficwindow_reader->getNextItem();
		for ( int r = 0; r < m_network_cfg.routerCount(); r ++ )
		{
			m_y_area_inject[ r ]  = window_item.injectTraffic()[ r ];
			m_y_area_accept[ r ]  = window_item.acceptTraffic()[ r ];
			m_y_area_through[ r ] = window_item.throughTraffic()[ r ];
		}
	}
	else
	{
		for ( int r = 0; r < m_network_cfg.routerCount(); r ++ )
		{
			m_y_area_inject[ r ]  = m_y_window_inject[ r ][ m_current_window ];
			m_y_area_accept[ r ]  = m_y_window_accept[ r ][ m_current_window ];
			m_y_area_through[ r ] = m_y_window_through[ r ][ m_current_window ];
		}
	}

	// update show
	mp_window_control_panel->setCycleLabel(
		QString( "%1/%2" ).arg( m_current_window ).arg( m_total_window ) );
	mp_window_control_panel->setLoadProgressBarValue( m_current_window );
	if ( !m_jump_flag )
	{
		drawPlotHandler();
		updateResultTable();
	}
	mp_result_plot->setTitle( QString("Traffic vs Area (%1 - %2)")
		.arg(m_current_window * m_window_step).arg(m_current_window * m_window_step + m_window_width) );
}
// continue run
void AnalyseDialog::nextRun()
{
	m_run_error_flag = false;
	nextCycle();

	if ( (m_current_window + 1 > m_total_window) || m_run_error_flag ||
		 (m_jump_flag && ( m_current_window > mp_window_control_panel->jumpValue() ) ) )
	{
		stopRun();
	}
	else
	{
		mp_window_control_panel->updateNextTimer();
	}
}
// continue run
void AnalyseDialog::prevRun()
{
	m_run_error_flag = false;
	prevCycle();

	if ( (m_current_window == 0) || m_run_error_flag ||
		 (m_jump_flag && ( m_current_window <= mp_window_control_panel->jumpValue() ) ) )
	{
		stopRun();
	}
	else
	{
		mp_window_control_panel->updatePrevTimer();
	}
}
// stop run
void AnalyseDialog::stopRun()
{
	if ( m_jump_flag )
	{
		drawPlotHandler();
		updateResultTable();
	}

	m_jump_flag = false;

	mp_window_control_panel->stopButtonClickedHandler();
}

void AnalyseDialog::jumpRun()
{
	m_jump_flag = true;
	if ( m_current_window > mp_window_control_panel->jumpValue() )
	{
		prevRun();
	}
	else if ( m_current_window < mp_window_control_panel->jumpValue() )
	{
		nextRun();
	}
}

void AnalyseDialog::keyReleaseEventHandler(QKeyEvent * event)
{
	switch( event->key() )
	{
		case Qt::Key_Backspace: prevCycle(); break;
		case Qt::Key_Return:
		case Qt::Key_Enter: nextCycle(); break;
		default: break;
	}
}

void AnalyseDialog::plotSizeEditingFinishedHandler()
{
	if (mp_size_config_panel->isFixed())
	{
		mp_result_plot->resize( mp_size_config_panel->plotSize() );
	}
}

void AnalyseDialog::plotResizeEventHandler()
{
	if (mp_size_config_panel->isFixed())
	{
		mp_result_plot->resize( mp_size_config_panel->plotSize() );
	}
	else
	{
		mp_size_config_panel->setPlotSize( mp_result_plot->size() );
	}
}
