// **** **** **** **** **** **** **** **** **** **** **** **** **** //
//	  Visual Network-on-chip Development Studio				   //
//																  //
//  File name	: integrator.h									 //
//  Last Modified: 2014.07.20									   //
//  Purpose	  : implement class integrator					   //
//  Author	   : Jun-shi Wang									 //
//  Organization : Green Computing and Communication @ UESTC-SCIE   //
//  VisualNoC is Copyright(c)UESTC-SCIE-GCC 2014,				   //
//																  //
//  All rights reserved, and is distributed as free software under  //
//  the license in the file "License",which is included in the	  //
//  distribution.												   //
// **** **** **** **** **** **** **** **** **** **** **** **** **** //

#include "archwindow.h"

#include "subdialogs/link_tool_dialog.h"
#include "subdialogs/tools_para_dialog.h"
#include "subdialogs/soc_dialog.h"
#include "subdialogs/arch_analyse_dialog.h"
#include "subdialogs/faults_config_dialog.h"
#include "subdialogs/graphics_config_dialog.h"

// ---- constructor and destructor ---- //
// constructor
Integrator::Integrator(QWidget *parent, Qt::WindowFlags flags) :
	RibbonMainWindow( parent, flags )
{
	// add menu bar
	addToolBarWidget();
	// add spliter structure
	addStructure();
	// init
	setOperateState( OPERATE_NOFILE );

	mp_linktool_model = new LinkToolModel( this );
	ErrorHandler t_error = mp_linktool_model->readLinkToolFile();
	if ( t_error.hasError() )
	{
		QMessageBox::warning( this, t_error.title(), t_error.text(),
							  QMessageBox::Ok );
	}
	addLinkTool();

	mp_graphics_config_model = new GraphicsConfigModel( this );
	ErrorHandler t_error_1 = mp_graphics_config_model->readFile();
	if ( t_error_1.hasError() )
	{
		mp_graphics_config_model->setDefaultValue();
	}


	QFile t_file( "../configxml/workdir.xml" );
	if ( t_file .open( QIODevice::ReadOnly ) )
	{
		QTextStream t_stream( &t_file );
		global_work_direction = t_stream .readLine();
		t_file .close();
	}
}

// ---- constructor and destructor ---- //

// ---- components initialization functions ---- //
// draw spliter structure
void Integrator::addStructure()
{
	// result tab sub window
	mp_result_tab_widget = new ResultTabWidget;
	mp_result_tab_widget->setTabsClosable( true );
	connect( mp_result_tab_widget, SIGNAL(currentChanged(int)),
		this, SLOT(resultTabCurrentChangedHandler(int)) );
	
	// set central widget
	setRibbonCentralWidget( mp_result_tab_widget );

	// set window size
	QDesktopWidget *d = QApplication::desktop();
	QRect screenRect = d->screenGeometry();
	this->resize(screenRect.width()*2/3, screenRect.height()*2/3);

	mp_result_tab_widget->setFocus();
}

// draw menu bar
void Integrator::addToolBarWidget()
{
	mp_toolbar_widget = new ToolBarWidget( this );
	setRibbonToolbarWidget( mp_toolbar_widget );
	connect( mp_toolbar_widget, SIGNAL( triggered(QAction*) ), this,
			 SLOT( flowEventHandler(QAction *) ) );

	// Menu Bar -> Begin
	ToolBarTab * network_tab = new ToolBarTab( "Begin", TB_BEGIN, this );
	// Menu Bar -> Begin -> Network File
	ToolBarBox * network_box = new ToolBarBox(
		"Network File", TBT_NETWORK, this );
	network_box->addLargeButton( new ToolBarButton(
		QIcon( "images/new.png" ), "New", FT_SOC_NEW,
		ToolBarButton::LargeButton ) );
	network_box->addLargeButton( new ToolBarButton(
		QIcon( "images/open.png" ), "Open", FT_SOC_OPEN,
		ToolBarButton::LargeButton ) );
	network_box->addLargeButton( new ToolBarButton(
		QIcon( "images/save.png" ), "Save", FT_SOC_SAVE,
		ToolBarButton::LargeButton ) );
	network_box->addLargeButton( new ToolBarButton(
		QIcon( "images/saveas.png" ), "Save As", FT_SOC_SAVEAS,
		ToolBarButton::LargeButton ) );
	network_box->addLargeButton( new ToolBarButton(
		QIcon( "images/close.png" ), "Close", FT_SOC_CLOSE,
		ToolBarButton::LargeButton ) );
	network_tab->addToolBarBox( network_box );
	// Menu Bar -> Analysis -> Configuration
	ToolBarBox * option_box = new ToolBarBox(
		"Options", TBT_OPTION, this );
	QList< ToolBarButton * > option_button_list;
	option_button_list.append( new ToolBarButton(
		QIcon( "images/settings.png" ), "Link Tools", FT_OPTION_TOOLS_CFG,
		ToolBarButton::SmallButton ) );
	option_button_list.append( new ToolBarButton(
		QIcon( "images/settings.png" ), "Work Direction", FT_OPTION_ENV_CONFIG,
		ToolBarButton::SmallButton ) );
	option_button_list.append( new ToolBarButton(
		QIcon( "images/settings.png" ), "Graphics", FT_OPTION_GRAPHICS_CONFIG,
		ToolBarButton::SmallButton ) );
	option_box->addSmallButton( option_button_list );
	option_box->addLargeButton( new ToolBarButton(
		QIcon( "images/refresh.png" ), "Renew  Tools", FT_OPTION_RENEW_TOOLS,
		ToolBarButton::LargeButton ) );
	option_box->addLargeButton( new ToolBarButton(
		QIcon( "images/information.png" ), "About", FT_ABOUT,
		ToolBarButton::LargeButton ) );
	network_tab->addToolBarBox( option_box );
	mp_toolbar_widget->addToolBarTab( network_tab );

	// Menu Bar -> Analyse
	ToolBarTab * tool_tab = new ToolBarTab( "Tools", TB_TOOLS, this );
	// Menu Bar -> Analysis -> Configuration
	ToolBarBox * configuration_box = new ToolBarBox(
		"Configuration", TBT_CONFIG, this );
	configuration_box->addLargeButton( new ToolBarButton(
		QIcon( "images/edit.png" ), "Network", FT_TOOLS_SOC_CONFIG,
		ToolBarButton::LargeButton ) );
	tool_tab->addToolBarBox( configuration_box );
	// Menu Bar -> Analysis -> Simulation
	ToolBarBox * simulation_box = new ToolBarBox(
		"Simulation", TBT_SIMULATE, this );
    mp_online_simulation = new ToolBarButton(
        QIcon( "images/sitemap_simulation.png" ), "Online\nSimulation", FT_TOOLS_ONLINE_SIMULATION,
        ToolBarButton::LargeButton );
    mp_online_simulation->setVisible(false);
    mp_offline_simulation = new ToolBarButton(
        QIcon( "images/run.png" ), "Offline\nSimulation", FT_TOOLS_ONLINE_SIMULATION,
        ToolBarButton::LargeButton );
    simulation_box->addLargeButton( mp_online_simulation );
    simulation_box->addLargeButton( mp_offline_simulation );
    tool_tab->addToolBarBox( simulation_box );
	// Menu Bar -> Analysis -> Debug
	ToolBarBox * debug_box = new ToolBarBox(
		"Debug", TBT_DEBUG, this );
	debug_box->addLargeButton( new ToolBarButton(
		QIcon( "images/sitemap_simulation.png" ), "Simulation", FT_TOOLS_REPLAY_SIMULATION,
		ToolBarButton::LargeButton ) );
	tool_tab->addToolBarBox( debug_box );
	// Menu Bar -> Analysis -> Analyse
	ToolBarBox * analyze_box = new ToolBarBox(
		"Analyse", TBT_ANALYSE, this );
	analyze_box->addLargeButton( new ToolBarButton(
		QIcon( "images/statistics_eventtrace.png" ), "Records", FT_TOOLS_ANALYSE_EVENTTRACE,
		ToolBarButton::LargeButton ) );
    analyze_box->addLargeButton( new ToolBarButton(
        QIcon( "images/pie-chart_graph.png" ), "Record\nWindow", FT_TOOLS_ANALYSE_TRAFFIC_WINDOW,
        ToolBarButton::LargeButton ) );
	tool_tab->addToolBarBox( analyze_box );
	mp_toolbar_widget->addToolBarTab( tool_tab );
}

void Integrator::addLinkTool()
{
	QList< ToolBarButton * > configure_list;
	QList< ToolBarButton * > simulation_list;
	QList< ToolBarButton * > debug_list;
	QList< ToolBarButton * > analyse_list;
	for ( int i = 0; i < mp_linktool_model->rowCount(); i ++ )
	{
		QString name = mp_linktool_model->linkToolItem( i )->name();
		int code = FT_LINK_START + i;
		switch( mp_linktool_model->linkToolItem( i )->categoryNum() )
		{
		case 0:
			configure_list.append( new ToolBarButton( QIcon( "images/edit.png" ),
				name, code, ToolBarButton::SmallButton ) );
			break;
		case 1:
			simulation_list.append( new ToolBarButton( QIcon( "images/run.png" ),
				name, code, ToolBarButton::SmallButton ) );
			break;
		case 2:
			debug_list.append( new ToolBarButton( QIcon( "images/pie-chart_graph.png" ),
				name, code, ToolBarButton::SmallButton ) );
			break;
		case 3:
			analyse_list.append( new ToolBarButton( QIcon( "images/pie-chart_graph.png" ),
				name, code, ToolBarButton::SmallButton ) );
			break;
		default: break;
		}
	}

	if ( configure_list.size() > 0 )
	{
		ToolBarBox * t_box =
			mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_CONFIG );
		ToolBarButtonScrollArea * configure_list_table =
			new ToolBarButtonScrollArea( configure_list, t_box );
		t_box->addButtonScrollArea( configure_list_table );
	}
	if ( simulation_list.size() > 0 )
	{
		ToolBarBox * t_box =
			mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_SIMULATE );
		ToolBarButtonScrollArea * simulation_list_table =
			new ToolBarButtonScrollArea( simulation_list, t_box );
		t_box->addButtonScrollArea( simulation_list_table );
	}
	if ( debug_list.size() > 0 )
	{
		ToolBarBox * t_box =
			mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_DEBUG );
		ToolBarButtonScrollArea * debug_list_table =
			new ToolBarButtonScrollArea( debug_list, t_box );
		t_box->addButtonScrollArea( debug_list_table );
	}
	if ( analyse_list.size() > 0 )
	{
		ToolBarBox * t_box =
			mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_ANALYSE );
		ToolBarButtonScrollArea * analyse_list_table =
			new ToolBarButtonScrollArea( analyse_list, t_box );
		t_box->addButtonScrollArea( analyse_list_table );
	}
}

void Integrator::clearLinkTool()
{
	mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_CONFIG )->
		removeWidgets();
	mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_SIMULATE )->
		removeWidgets();
	mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_ANALYSE )->
		removeWidgets();
	mp_toolbar_widget->tabOfCode( TB_TOOLS )->boxOfCode( TBT_DEBUG )->
		removeWidgets();
}

// set operate state
void Integrator::setOperateState(OperateStateEnum state)
{
	// set operate state
	OperateStateEnum prestate = m_operate_state;
	m_operate_state = state;
	if ( m_operate_state == OPERATE_NOFILE )
	{
		setWindowTitle( VISUALARCH_TITLE );
		mp_toolbar_widget->setTabVisible( TB_TOOLS, false );
		for ( int i = 0; i < mp_toolbar_widget->actions().size(); i ++ )
		{
			int code = mp_toolbar_widget->actions()[ i ]->data().toInt();
			if ( code == FT_SOC_NEW || code == FT_SOC_OPEN ||
				 code == FT_OPTION_TOOLS_CFG || code == FT_OPTION_RENEW_TOOLS ||
				 code == FT_OPTION_ENV_CONFIG || code == FT_OPTION_GRAPHICS_CONFIG ||
				 code == FT_ABOUT )
			{
				mp_toolbar_widget->actions()[ i ]->setEnabled( true );
			}
			else
			{
				mp_toolbar_widget->actions()[ i ]->setEnabled( false );
			}
		}
	}
	else if ( m_operate_state == OPERATE_NEWFILE ||
			  m_operate_state == OPERATE_SAVEDFILE )
	{
		if ( m_operate_state == OPERATE_NEWFILE )
		{
			setWindowTitle( QString( VISUALARCH_TITLE ) + " - New SoC" );
		}
		else
		{
			QFileInfo t_file_info( m_soc_file_name );
			setWindowTitle( QString( VISUALARCH_TITLE ) + " - " +
							t_file_info.fileName() );
		}
		mp_toolbar_widget->setTabVisible( TB_TOOLS, true );
		if (prestate == OPERATE_NOFILE)
		{
			for ( int i = 0; i < mp_toolbar_widget->actions().size(); i ++ )
			{
				mp_toolbar_widget->actions()[ i ]->setEnabled( true );
			}
		}
	}
}

void Integrator::flowEventHandler( Integrator::EventCode e )
{
	QAction * t_action = new QAction( this );
	t_action->setData( QVariant( e ) );
	flowEventHandler( t_action );
}

// event handler center
void Integrator::flowEventHandler( QAction * event )
{
	int name = event->data().toInt();
	if ( name == FT_SOC_NEW || name == FT_SOC_OPEN )
	{
		// if there is a open network, close it first
		if ( m_operate_state != OPERATE_NOFILE )
		{
			flowEventHandler( FT_SOC_CLOSE );
		}
		// choose network template file or network file
		if ( name == FT_SOC_NEW )
		{
			m_soc_cfg = EsySoCCfg();
			m_soc_file_name = "";
			setOperateState( OPERATE_NEWFILE );
		}
		else
		{
			QString t_file_path = QFileDialog::getOpenFileName( this,
				"Choose SoC Configuration File", global_work_direction,
				"SoC Configuration File (*." +
					QString( SOCCFG_EXTENSION ) + ")" );
			if ( t_file_path == "" )
			{
				return;
			}

			string filepath_str = t_file_path.left(
				t_file_path.lastIndexOf(".") ).toStdString();
			ErrorHandler t_error = m_soc_cfg.readXml( filepath_str );
			if ( t_error.hasError() )
			{
				QMessageBox::warning(this, t_error.title(), t_error.text(),
					QMessageBox::Ok );
				return;
			}
			m_soc_file_name = t_file_path;
			setOperateState( OPERATE_SAVEDFILE );
		}

        flowEventHandler( FT_TOOLS_SOC_CONFIG );
	}
	else if ( name == FT_TOOLS_SOC_CONFIG )
	{
		// a network config window is open
		int t_index = mp_result_tab_widget->indexOfType(
					ResultBaseDialog::RESULT_DISPLAY_CONFIG );
		if ( t_index >= 0 )
		{
			mp_result_tab_widget ->setCurrentIndex( t_index );
		}
		else
		{
			// new window
			if ( m_soc_file_name == "" )
			{
				SoCDialog * socviewdlg = new SoCDialog(
					ResultBaseDialog::RESULT_DISPLAY_CONFIG, &m_soc_cfg,
							mp_graphics_config_model );
				mp_result_tab_widget->addTab( socviewdlg,
					"SoC Config-New SoC" );
			}
			else
			{
				SoCDialog * socviewdlg = new SoCDialog(
					ResultBaseDialog::RESULT_DISPLAY_CONFIG, &m_soc_cfg,
							mp_graphics_config_model );
				QFileInfo t_file_info( m_soc_file_name );
				mp_result_tab_widget->addTab( socviewdlg,
                    "SoC Config-" + t_file_info.fileName());
			}
		}
	}
	else if ( name == FT_SOC_SAVE || name == FT_SOC_SAVEAS )
	{
		if ( name == FT_SOC_SAVEAS || m_operate_state == OPERATE_NEWFILE )
		{
			QString t_file_path = QFileDialog::getSaveFileName( this,
				"Choose Save File", global_work_direction,
				"SoC Configuration File (*." +
					QString( SOCCFG_EXTENSION ) + ")" );
			if ( t_file_path == "" )
			{
				return;
			}
			m_soc_file_name = t_file_path;
		}

		string filepath_str = m_soc_file_name.left(
			m_soc_file_name.lastIndexOf(".") ).toStdString();
		ErrorHandler t_error = m_soc_cfg.writeXml( filepath_str );
		if ( t_error.hasError() )
		{
			QMessageBox::warning(this, t_error.title(), t_error.text(),
				QMessageBox::Ok );
			return;
		}

		int t_cfg_index = mp_result_tab_widget ->indexOfType(
					ResultBaseDialog::RESULT_DISPLAY_CONFIG );
		if ( t_cfg_index >= 0 )
		{
			QFileInfo t_file_info( m_soc_file_name );
			mp_result_tab_widget->setTabText( t_cfg_index, "SoC Config-" +
											  t_file_info.fileName() );
		}
		setOperateState( OPERATE_SAVEDFILE );
	}
	else if ( name == FT_SOC_CLOSE )
	{
		flowEventHandler( FT_SOC_SAVE );
		while ( mp_result_tab_widget ->count() > 0 )
		{
			QWidget * t_dialog = mp_result_tab_widget ->widget( 0 );
			mp_result_tab_widget ->removeTab( 0 );
			delete t_dialog;
		}
		setOperateState( OPERATE_NOFILE );
	}
	else if ( name == FT_OPTION_TOOLS_CFG )
	{
		clearLinkTool();
		LinkToolDialog * t_dialog = new LinkToolDialog( 0,
			LinkToolDelegate::LINKTOOLVIEW_EDIT );
		if ( t_dialog ->exec() == QDialog::Accepted )
		{
			mp_linktool_model = t_dialog->linkToolModel();
			mp_linktool_model->setParent( this );
		}
		else
		{
			delete t_dialog->linkToolModel();
		}
		delete t_dialog;
		addLinkTool();
	}
	else if ( name == FT_OPTION_RENEW_TOOLS )
	{
		LinkToolDialog * t_dialog = new LinkToolDialog( mp_linktool_model,
			LinkToolDelegate::LINKTOOLVIEW_SELECT );
		if ( t_dialog ->exec() == QDialog::Accepted )
		{
			// write renew shell
#ifdef 	Q_OS_LINUX
			QFile t_file( "renew.sh" );
#else
			QFile t_file( "renew.bat" );
#endif
			t_file .open( QIODevice::WriteOnly );
			QTextStream t_stream( &t_file );
			t_stream << mp_linktool_model->renewScript();
			t_file .close();
			//		renewToolsHandler();
#ifdef 	Q_OS_LINUX
			ProcessDialog * renewdlg = new ProcessDialog( 0, "", "sh renew.sh" );
#else
			ProcessDialog * renewdlg = new ProcessDialog( 0, "", "renew.bat" );
#endif
			mp_result_tab_widget ->addTab( renewdlg, "Renew Tools");
			renewdlg ->start();
			renewdlg ->setExecText( mp_linktool_model->renewScript() );
		}
		delete t_dialog;
	}
    else if ( name == FT_TOOLS_ONLINE_SIMULATION )
    {
        bool online_visible = mp_online_simulation->isVisible();
        mp_offline_simulation->setVisible(online_visible);
        mp_online_simulation->setVisible(!online_visible);
    }
    else if ( name == FT_TOOLS_REPLAY_SIMULATION )
	{
		if (m_soc_file_name == "")
		{
			QMessageBox::warning(this, "Not saved file",
								 "Please save the file at first");
			return;
		}

		// open event trace file
		QString recodrfname = QFileDialog::getOpenFileName( this,
            "Open Record File", global_work_direction,
			"SoC Record File (*." + QString( SOCRECORD_EXTENSION ) + "b *."
				+ QString( SOCRECORD_EXTENSION ) + "t)" );
		if (recodrfname == "")
			return;

		SoCDialog * socviewdlg = new SoCDialog(
			ResultBaseDialog::RESULT_REPLAY_EVENTTRACE, &m_soc_cfg,
					mp_graphics_config_model );

		QFileInfo t_file_info( recodrfname );
		mp_result_tab_widget ->addTab( socviewdlg,
			"Replay Sim-" + t_file_info.fileName());

		if ( !socviewdlg->setRecord( recodrfname ) )
		{
			return;
		}
		else
		{
			socviewdlg->loadRecordStart();
		}
	}
	else if ( name == FT_ABOUT )
	{
		QString aboutinfo = QString( VISUALARCH_TITLE ) + " for " +
				QString( VISUALARCH_PLATFORM ) + "\n" +
				"Version : " + QString( VISUALARCH_PLATFORM_CODE ) +
				QString( VISUALARCH_VERSION ) + "\n" +
				"Developed by GCC from SCIE, UESTC";
		QMessageBox::information ( this, "About", aboutinfo );
	}
	else if ( name == FT_TOOLS_ANALYSE_EVENTTRACE )
	{
        QString fname = QFileDialog::getOpenFileName( this,
            "Open Record File", global_work_direction,
            "SoC Record File (*." + QString( SOCRECORD_EXTENSION ) + "b *."
                + QString( SOCRECORD_EXTENSION ) + "t)" );
        if ( fname == "" )
		{
			return;
		}

		AnalyseDialog * t_dialog = new AnalyseDialog(
			ResultBaseDialog::RESULT_ANALYSE_EVENTTRACE, m_soc_cfg );

		QFileInfo t_file_info( fname );
		mp_result_tab_widget ->addTab( t_dialog,
            "Analyse Record-" + t_file_info.fileName());

		if ( !t_dialog ->setAnalyseFile( fname ) )
		{
			return;
		}
		else
		{
			t_dialog ->analyseStartHandler();
        }
	}
	else if ( name == FT_TOOLS_ANALYSE_TRAFFIC_WINDOW )
	{
        QString fname = QFileDialog::getOpenFileName( this,
			"Open Traffic Window File", global_work_direction,
            "Traffic Window File (*." + QString( RECORDWIN_EXTENSION ) + "t)" );
		if ( fname == "" )
		{
			return;
		}

		AnalyseDialog * t_dialog = new AnalyseDialog(
			ResultBaseDialog::RESULT_ANALYSE_WINDOW, m_soc_cfg );

		QFileInfo t_file_info( fname );
		mp_result_tab_widget ->addTab( t_dialog,
			"Analyse Traffic Window-" + t_file_info.fileName());

		if ( !t_dialog ->setAnalyseFile( fname ) )
		{
			return;
		}
		else
		{
			t_dialog ->analyseStartHandler();
        }
	}
	else if ( name == FT_OPTION_ENV_CONFIG )
	{
		QString dir = QFileDialog::getExistingDirectory(this, "Work Directory",
			global_work_direction,
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if ( dir == "" )
		{
			return;
		}

		global_work_direction = dir;
		QFile t_file( "../configxml/workdir.xml" );
		if ( t_file .open( QIODevice::WriteOnly ) )
		{
			QTextStream t_stream( &t_file );
			t_stream << global_work_direction;
			t_file .close();
		}
	}
	else if ( name == FT_OPTION_GRAPHICS_CONFIG )
	{
		GraphicsConfigDialog * t_dialog = new GraphicsConfigDialog();
		if ( t_dialog ->exec() == QDialog::Accepted )
		{
			mp_graphics_config_model->copy( t_dialog->graphicsConfigModel() );
		}
		delete t_dialog->graphicsConfigModel();
		delete t_dialog;
	}
	// link tools
	else if ( name >= FT_LINK_START )
	{
		int seq = name - FT_LINK_START;
        if ( (mp_linktool_model->linkToolItem( seq )->categoryNum() == 1) &&
                mp_online_simulation->isVisible())
        {
            if (m_soc_file_name == "")
            {
                QMessageBox::warning(this, "Not saved file",
                                     "Please save the file at first");
                return;
            }

            ToolsConfigDialog * tools_dialog = new ToolsConfigDialog(
                mp_linktool_model->linkToolItem( seq )->direction() + "/" +
                mp_linktool_model->linkToolItem( seq )->config(), true );
            if ( tools_dialog->exec() != QDialog::Accepted )
            {
                return;
            }

            SoCDialog * socviewdlg = new SoCDialog(
                ResultBaseDialog::RESULT_ONLINE_EVENTTRACE, &m_soc_cfg,
                        mp_graphics_config_model );

            mp_result_tab_widget->addTab( socviewdlg,
                "Online Sim-" + mp_linktool_model->linkToolItem( seq )->name());

            socviewdlg->setCommand(mp_linktool_model->linkToolItem( seq )->direction(),
                                   tools_dialog->processCommand( 0 ));
        }
        else
        {
            ToolsParaDialog * t_dialog = new ToolsParaDialog(
                mp_linktool_model->linkToolItem( seq )->direction() + "/" +
                mp_linktool_model->linkToolItem( seq )->config() );
            mp_result_tab_widget ->addTab( t_dialog,
                "Link Tools-"  + mp_linktool_model->linkToolItem( seq )->name() );
        }
	}
}

// ---- slot functions ---- //
void Integrator::resultTabCurrentChangedHandler( int index )
{
	for ( int i = 0; i < mp_toolbar_widget->count(); i ++ )
	{
		if ( ((ToolBarTab *)mp_toolbar_widget->widget( i ))->tabCode() > TB_CONTROL_BEGIN )
		{
			mp_toolbar_widget->removeToolBarTab( i );
			i --;
		}
	}

	if ( index < 0 || index >= mp_result_tab_widget->count() )
	{
		return;
	}

	ResultBaseDialog * t_dialog = (ResultBaseDialog *)mp_result_tab_widget->widget( index );
	for ( int i = 0; i < t_dialog->toolBarTabList().size(); i ++ )
	{
		ToolBarTab * t = t_dialog->toolBarTabList()[ i ];
		mp_toolbar_widget->addToolBarTab( t );
		if ( i == 0 )
		{
			mp_toolbar_widget->setCurrentWidget( t );
		}
	}
}
