#include "arch_analyse_dialog.h"

#define CYCLE_POINT_LIMIT 10000

QStringList AnalyseDialog::const_task_combobox_list = QStringList() <<
    "General\nStatstics" << "Time\nDistribution" << "Area\nDistribution" << "Windows";

QStringList AnalyseDialog::const_variable_list = QStringList() <<
    "L1 Inst\nCache Access" << "L1 Inst\nCache Hit" << "L1 Inst\nCache Miss" <<
    "L1 Data\nCache Access" << "L1 Data\nCache Hit" << "L1 Data\nCache Miss" <<
    "L2 Inst\nCache Access" << "L2 Inst\nCache Hit" << "L2 Inst\nCache Miss" <<
    "L2 Data\nCache Access" << "L2 Data\nCache Hit" << "L2 Data\nCache Miss" <<
    "Memory\nAccess" << "Message\nSend" << "Message\nReceive" << "Message\nProbe" <<
    "Integer Adder\nOperation" << "Integar Multipler\nOperation" <<
    "Float Adder\nOperation" << "Float Multipler\nOperation";

AnalyseDialog::AnalyseDialog() :
	ResultBaseDialog( RESULT_NONE ),
    m_soc_cfg(), m_analyse_fname( "" ),
    mp_analyse_record_thread( 0 ), mp_record_reader( 0 )
{
}
AnalyseDialog::AnalyseDialog( ResultTypeEnum type, const EsySoCCfg & cfg ) :
	ResultBaseDialog( type ),
    m_soc_cfg( cfg ), m_analyse_fname( "" ),
    mp_analyse_record_thread( 0 ), mp_record_reader( 0 )
{
	mp_result_plot = new AnalysePlot(
        m_soc_cfg.network().size( EsyNetCfg::AX_X ),
        m_soc_cfg.network().size( EsyNetCfg::AX_Y ),
        this );

	mp_task_combobox = new QComboBox();
	mp_task_combobox->setFixedSize(100, 50);
	for ( int i = 0; i < TASK_COUNT; i ++ )
	{
		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
			mp_task_combobox->insertItem( i, const_task_combobox_list[ i ], i );
		}
        if ( ( m_result_type == RESULT_ANALYSE_WINDOW && i == VAR_WINDOW ) )
		{
			mp_task_combobox->insertItem( i, const_task_combobox_list[ i ], i );
		}
	}
	connect( mp_task_combobox, SIGNAL( currentIndexChanged(int) ),
			 this, SLOT( taskChangedHandler(int) ) );

    mp_tile_label = new QLabel( "Tile ID" );
    mp_tile_combobox = new QComboBox();
    mp_tile_combobox->addItem( "All" );
    for ( size_t i = 0; i < m_soc_cfg.tileCount(); i ++ )
    {
        mp_tile_combobox->addItem( QString::number( i ) );
    }

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
    traffic_window_layout->addWidget( mp_tile_label, 0, 4, 1, 1 );
    traffic_window_layout->addWidget( mp_tile_combobox, 0, 5, 1, 1 );
    traffic_window_layout->setSpacing( 5 );

	ToolBarBox * task_box = new ToolBarBox(
		"Analyse Task", -1, this );
	task_box->hBoxLayout()->addLayout( traffic_window_layout );

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

    mp_variable_label = new QLabel( "Variable" );
    mp_variable_combobox = new QComboBox();
    mp_variable_combobox->setFixedSize(100, 50);

    mp_variable_combobox ->addItems( const_variable_list );
    connect( mp_variable_combobox, SIGNAL( currentIndexChanged(int) ),
			 this, SLOT( drawPlotHandler() ) );

	QGridLayout * figure_config_layout = new QGridLayout() ;
	figure_config_layout->addWidget( mp_traffic_draw_label, 0, 0, 1, 1);
	figure_config_layout->addWidget( mp_traffic_draw_combobox, 0, 1, 1, 1);
    figure_config_layout->addWidget( mp_variable_label, 1, 0, 1, 1);
    figure_config_layout->addWidget( mp_variable_combobox, 1, 1, 1, 1);
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
    if (mp_analyse_record_thread != NULL)
	{
        mp_analyse_record_thread ->quit();
        mp_analyse_record_thread ->wait();
        delete mp_analyse_record_thread;
	}
    if (mp_record_reader != NULL)
	{
        delete mp_record_reader;
	}
}

void AnalyseDialog::taskChangedHandler( int index )
{
    int task_index = mp_task_combobox->itemData( index ).toInt();

    bool tileenable = (task_index == VAR_TIME);
    mp_tile_label->setVisible( tileenable );
    mp_tile_combobox->setVisible( tileenable );

    bool windowenable = (task_index == VAR_WINDOW);
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
         ( (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) &&
           ( mp_traffic_draw_combobox->currentIndex() == 1 ) );
	mp_result_plot->enableAxis( QwtPlot::yRight, yrightenable );

	bool xtopenable = (
           (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) &&
		   mp_traffic_draw_combobox->currentIndex() == 1 );
	mp_result_plot->enableAxis( QwtPlot::xTop, xtopenable );

	bool xbottomable = ( !xtopenable && (m_current_task != GENERAL) );
	mp_result_plot->enableAxis( QwtPlot::xBottom, xbottomable );

	mp_result_plot->clearCurve();
    if ( m_current_task == VAR_TIME )
	{
        int index = mp_variable_combobox->currentIndex();
        mp_result_plot->attachCurve( const_variable_list[ index ], QwtPlot::yLeft,
            m_x_time, m_y_variables[ index ] );
    }
    if ( ( m_current_task == VAR_AREA || m_current_task == VAR_WINDOW ) &&
		 mp_traffic_draw_combobox->currentIndex() == 0 )
	{
        int index = mp_variable_combobox->currentIndex();
        mp_result_plot->attachBarChart(
            const_variable_list[ index ], m_y_area_tile_variables[ index ] );
	}
    if ( (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) &&
		 mp_traffic_draw_combobox->currentIndex() == 1 )
	{
        int index = mp_variable_combobox->currentIndex();
        mp_result_plot->attachSpectrogram( m_y_area_variables[ index ],
            m_soc_cfg.network().size( EsyNetCfg::AX_X ) );
	}

	bool wheelenable =
       ( m_current_task == VAR_TIME ||
         ( (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) &&
           mp_traffic_draw_combobox->currentIndex() == 0 ) );
	mp_result_plot->setWheelEnable( wheelenable );

	bool matrixenable = (
           (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) &&
		   mp_traffic_draw_combobox->currentIndex() == 1 );
	mp_result_plot->setMatrixPickerEnable( matrixenable );

    bool barchartenable = (
        ( (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) &&
			mp_traffic_draw_combobox->currentIndex() == 0) );
	mp_result_plot->setBarChartEnable( barchartenable );

    mp_result_plot->setGanttEnable( false );

	bool trafficdrawenable = (
         (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) &&
         ( m_soc_cfg.network().topology() ==
		   EsyNetCfg::NOC_TOPOLOGY_2DMESH ||
           m_soc_cfg.network().topology() ==
		   EsyNetCfg::NOC_TOPOLOGY_2DTORUS ) );
	mp_traffic_draw_label->setVisible( trafficdrawenable );
	mp_traffic_draw_combobox->setVisible( trafficdrawenable );

    bool variableenable = (m_current_task == VAR_TIME ||
         (m_current_task == VAR_AREA || m_current_task == VAR_WINDOW) );
    mp_variable_label->setVisible( variableenable );
    mp_variable_combobox->setVisible( variableenable );

    bool windowenable = (m_current_task == VAR_WINDOW);
	mp_window_control_panel->setVisible( windowenable );

    mp_figure_config_box->setVisible( trafficdrawenable || variableenable
                                     || windowenable );

	mp_result_plot->setTitle( "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::xTop, "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft, "" );
	mp_result_plot->setAxisTitleUnit( QwtPlot::yRight, "" );
	switch ( m_current_task )
	{
	case GENERAL: break;
    case VAR_TIME:
        mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "Time", "Unit" );
        mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft, "Count", "" );
		break;
    case VAR_AREA:
    case VAR_WINDOW:
        mp_result_plot->setTitle( "" );
		if ( mp_traffic_draw_combobox->currentIndex() == 0 )
		{
            mp_result_plot->setAxisTitleUnit( QwtPlot::xBottom, "Tile ID" );
            mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft, "Count", "" );
		}
		else if ( mp_traffic_draw_combobox->currentIndex() == 1 )
		{
			mp_result_plot->setAxisTitleUnit( QwtPlot::xTop, "X Axis" );
			mp_result_plot->setAxisTitleUnit( QwtPlot::yLeft, "Y Axis" );
            mp_result_plot->setAxisTitleUnit( QwtPlot::yRight, "Count", "" );
		}
		break;
	default:
		break;
	}

	mp_result_plot->setFixedZAxis( false, QwtInterval() );
	mp_result_plot->setFixedYLeftAxis( false, QwtInterval() );
    if ( m_current_task == VAR_WINDOW &&
		 mp_traffic_draw_combobox->currentIndex() == 0 )
	{
        int index = mp_variable_combobox->currentIndex();
        mp_result_plot->setFixedYLeftAxis(
            true, QwtInterval( 0, m_y_window_var_max[ index ] ) );
	}
    else if ( m_current_task == VAR_WINDOW &&
		 mp_traffic_draw_combobox->currentIndex() == 1 )
	{
        int index = mp_variable_combobox->currentIndex();
        mp_result_plot->setFixedZAxis(
            true, QwtInterval( 0, m_y_window_var_max[ index ] ) );
	}

	mp_result_plot->fitWindow();
	mp_result_plot->update();
}

bool AnalyseDialog::setAnalyseFile( QString & fname )
{
	m_analyse_fname = fname;
	string filepath_str = m_analyse_fname.
			left( m_analyse_fname.lastIndexOf( "." ) ).toStdString();
	
    if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
	{
		if ( m_analyse_fname .right( 1 ) == "b" )
		{
            mp_record_reader = new EsyDataFileIStream< EsyDataItemSoCRecord >(
                10000, filepath_str, SOCRECORD_EXTENSION, true );
		}
		else
		{
            mp_record_reader = new EsyDataFileIStream< EsyDataItemSoCRecord >(
                10000, filepath_str, SOCRECORD_EXTENSION, false );
		}
        if ( !mp_record_reader )
		{
			QMessageBox::warning( this, "Wrong File",
                "Cannot open input record file (T1) " + fname );
			return false;
		}
        if ( !mp_record_reader ->openSuccess() )
		{
			QMessageBox::warning( this, "Wrong File",
                "Cannot open input record file (T2) " + fname );
			return false;
		}	
        mp_control_panel->setLoadProgressBarRange( 0, mp_record_reader->size() );
	}
	else if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
		if ( m_analyse_fname .right( 1 ) == "b" )
		{
            mp_recordwindow_reader = new EsyDataFileIStream< EsyDataItemRecordWindow >(
                1000, filepath_str, RECORDWIN_EXTENSION, true );
		}
		else
		{
            mp_recordwindow_reader = new EsyDataFileIStream< EsyDataItemRecordWindow >(
                1000, filepath_str, RECORDWIN_EXTENSION, false );
		}
        if ( !mp_recordwindow_reader )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input traffic window file (T1) " + fname );
			return false;
		}
        if ( !mp_recordwindow_reader ->openSuccess() )
		{
			QMessageBox::warning( this, "Wrong File",
				"Cannot open input trace file (T2) " + fname );
			return false;
		}
        mp_control_panel->setLoadProgressBarRange( 0, mp_recordwindow_reader->size() );
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
        if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
            mp_analyse_record_thread = new AnalyseRecordThread(
                mp_record_reader, &m_soc_cfg,
                &m_total_time, &m_total_variable
            );
		}
	}
    else if (m_current_task == VAR_TIME)
	{
        int tile_id = mp_tile_combobox->currentIndex() - 1;
        if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
            mp_analyse_record_thread = new AnalyseRecordThread(
                mp_record_reader, &m_soc_cfg, tile_id, m_total_time,
                m_time_point, m_time_scaling, &m_y_variables
            );
		}
	}
    else if (m_current_task == VAR_AREA)
	{
        if (m_result_type == RESULT_ANALYSE_EVENTTRACE)
		{
            mp_analyse_record_thread = new AnalyseRecordThread(
                mp_record_reader, &m_soc_cfg, m_total_time,
                &m_y_area_variables, &m_y_area_tile_variables
            );
		}
	}
    else if (m_current_task == VAR_WINDOW)
	{
		m_window_width = mp_traffic_window_edit->text().toInt();
		m_window_step = mp_traffic_step_edit->text().toInt();
        m_total_window = (m_total_time - m_window_width) / m_window_step;

		if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
		{
            mp_analyse_record_thread = new AnalyseRecordThread(
               mp_record_reader, &m_soc_cfg,
                m_window_step, m_window_width, m_total_time,
                &m_y_window_variables
            );
		}
		else if ( m_result_type == RESULT_ANALYSE_WINDOW )
		{
            mp_analyse_recordwindow_thread = new AnalyseRecordWindowThread(
                mp_recordwindow_reader, &m_soc_cfg,
				&m_total_window, &m_window_width, &m_window_step,
                &m_y_window_var_max
            );
		}
	}
	
    if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
	{
        connect(mp_analyse_record_thread, SIGNAL(finished()),
				this, SLOT(analyseFinishedHandler()));
        connect(mp_analyse_record_thread, SIGNAL(progress(int)),
				mp_control_panel, SLOT(setLoadProgressBarValue(int)));
        mp_analyse_record_thread ->start();
	}
    else if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
        connect(mp_analyse_recordwindow_thread, SIGNAL(finished()),
				this, SLOT(analyseFinishedHandler()));
        connect(mp_analyse_recordwindow_thread, SIGNAL(progress(int)),
				mp_control_panel, SLOT(setLoadProgressBarValue(int)));
        mp_analyse_recordwindow_thread ->start();
	}
}

void AnalyseDialog::analyseFinishedHandler()
{
	mp_control_panel->enableAllComponent( true );

    if ( m_result_type == RESULT_ANALYSE_EVENTTRACE )
	{
        if (mp_analyse_record_thread->isGood() == false)
		{
            delete mp_analyse_record_thread;
            mp_analyse_record_thread = NULL;
			return;
		}
        delete mp_analyse_record_thread;
        mp_analyse_record_thread = NULL;
        mp_control_panel->setLoadProgressBarValue( mp_record_reader->size() );
	}
	else if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
        if (mp_analyse_recordwindow_thread->isGood() == false)
		{
            delete mp_analyse_recordwindow_thread;
            mp_analyse_recordwindow_thread = NULL;
			return;
		}
        delete mp_analyse_recordwindow_thread;
        mp_analyse_recordwindow_thread = NULL;
        mp_control_panel->setLoadProgressBarValue( mp_recordwindow_reader->size() );

		mp_traffic_window_edit->setText( QString::number( m_window_width ) );
		mp_traffic_step_edit->setText( QString::number( m_window_step ) );
	}

	if ( m_current_task == GENERAL )
	{
        if ( m_total_time > CYCLE_POINT_LIMIT )
        {
            m_time_scaling = qRound( (double)(m_total_time) / CYCLE_POINT_LIMIT );
            m_time_point = m_total_time / m_time_scaling;
        }
        else
        {
            m_time_scaling = 1;
            m_time_point = m_total_time;
        }

		// update data filed;
        m_x_time.resize( m_time_point + 1 );
        for ( int i = 0; i < m_time_point + 1; i ++ )
		{
            m_x_time[ i ] = i * m_time_scaling;
		}
        m_x_area_tile.resize( m_soc_cfg.tileCount() );
        for ( int i = 0; i < m_x_area_tile.size(); i ++ )
        {
            m_x_area_tile[ i ] = i;
        }
	}
    else if (m_current_task == VAR_WINDOW )
	{
		if ( m_result_type != RESULT_ANALYSE_WINDOW )
		{
            for ( int var = 0; var < AnalyseRecordThread::VARIABLE_COUNT; var ++ )
            {
                for ( int i = 0; i < m_total_window; i ++ )
                {
                    m_y_window_var_max[ var ] = 0.0;
                    for ( size_t r = 0; r < m_soc_cfg.tileCount(); r ++ )
                    {
                        if ( m_y_window_variables[ var ][ r ][ i ] >
                             m_y_window_var_max[ var ] )
                        {
                            m_y_window_var_max[ var ] =
                             m_y_window_variables[ var ][ r ][ i ];
                        }
                    }
                }
            }
		}

		m_current_window = -1;
		if ( m_result_type == RESULT_ANALYSE_WINDOW )
		{
            m_x_area_tile.resize( m_soc_cfg.tileCount() );
            for ( size_t i = 0; i < m_soc_cfg.tileCount(); i ++ )
			{
                m_x_area_tile[ i ] = i;
			}
            mp_recordwindow_reader->reset();
			m_total_window --;
		}
        m_y_area_tile_variables.resize( AnalyseRecordThread::VARIABLE_COUNT );
        m_y_area_variables.resize( AnalyseRecordThread::VARIABLE_COUNT );
        for ( int var = 0; var < AnalyseRecordThread::VARIABLE_COUNT; var ++)
        {
            m_y_area_tile_variables[ var ].resize( m_soc_cfg.tileCount() );
            m_y_area_variables[ var ].resize( m_soc_cfg.network().routerCount() );
        }
		nextCycle();
		mp_window_control_panel->setLoadProgressBarRange( 0, m_total_window );

		m_run_error_flag = false;
		m_jump_flag = false;

		mp_window_control_panel->enableAllComponent( true );
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

        for ( int var = 0; var < AnalyseRecordThread::VARIABLE_COUNT; var ++ )
        {
			mp_result_model->appendDataItem(
                const_variable_list[ var ], m_total_variable[ var ], "" );
		}
	}
    else if ( m_current_task == VAR_TIME )
	{
        mp_result_model->appendDataVector( "Time", m_x_time );
        for ( int var = 0; var < AnalyseRecordThread::VARIABLE_COUNT; var ++ )
        {
            mp_result_model->appendDataVector(
                const_variable_list[ var ], m_y_variables[ var ] );
        }
	}
    else if ( m_current_task == VAR_AREA || m_current_task == VAR_WINDOW  )
	{
        mp_result_model->appendDataVector( "Id", m_x_area_tile );
        for ( int var = 0; var < AnalyseRecordThread::VARIABLE_COUNT; var ++ )
        {
            mp_result_model->appendDataVector(
                const_variable_list[ var ], m_y_area_tile_variables[ var ] );
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

    if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
        EsyDataItemRecordWindow window_item = mp_recordwindow_reader->getPrevItem();
        for ( size_t r = 0; r < m_soc_cfg.tileCount(); r ++ )
		{
            for ( int var = 0; var < AnalyseRecordWindowThread::VARIABLE_COUNT; var ++)
            {
                m_y_area_tile_variables[ var ][ r ] =
                    mp_analyse_recordwindow_thread->variable( window_item, var, r );
                m_y_area_variables[ var ][ m_soc_cfg.tile( r ).niId() ] =
                    mp_analyse_recordwindow_thread->variable( window_item, var, r );
            }
        }
	}
	else
    {
        for ( int r = 0; r < m_soc_cfg.network().routerCount(); r ++ )
		{
            for ( int var = 0; var < AnalyseRecordThread::VARIABLE_COUNT; var ++)
            {
                m_y_area_tile_variables[ var ][ r ]  =
                        m_y_window_variables[ var ][ r ][ m_current_window ];
                m_y_area_variables[ var ][ m_soc_cfg.tile( r ).niId() ] =
                     m_y_window_variables[ var ][ r ][ m_current_window ];
            }
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

    if ( m_result_type == RESULT_ANALYSE_WINDOW )
	{
        EsyDataItemRecordWindow window_item = mp_recordwindow_reader->getNextItem();
        for ( size_t r = 0; r < m_soc_cfg.tileCount(); r ++ )
        {
            for ( int var = 0; var < AnalyseRecordWindowThread::VARIABLE_COUNT; var ++)
            {
                m_y_area_tile_variables[ var ][ r ] =
                    mp_analyse_recordwindow_thread->variable( window_item, var, r );
                m_y_area_variables[ var ][ m_soc_cfg.tile( r ).niId() ] =
                    mp_analyse_recordwindow_thread->variable( window_item, var, r );
            }
        }
	}
	else
    {
        for ( size_t r = 0; r < m_soc_cfg.tileCount(); r ++ )
		{
            for ( int var = 0; var < AnalyseRecordThread::VARIABLE_COUNT; var ++)
            {
                m_y_area_tile_variables[ var ][ r ]  =
                        m_y_window_variables[ var ][ r ][ m_current_window ];
                m_y_area_variables[ var ][ m_soc_cfg.tile( r ).niId() ] =
                     m_y_window_variables[ var ][ r ][ m_current_window ];
            }
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
