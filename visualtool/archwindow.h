/*
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA  02110-1301, USA.

 ---
 Copyright (C) 2015, Junshi Wang <>
 */

#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "define/qt_include.h"
#include "define/version_include.h"

#include "subwidgets/result_tab_widget.h"

#include "modelsview/link_tool_table.h"
#include "modelsview/soc_cfg_tree.h"
#include "modelsview/graphics_config_tree.h"

/*!
 * Class for main window
 */
class Integrator : public RibbonMainWindow
{
	Q_OBJECT
public:
	/*!
	 * The state of file operation.
	 */
	enum OperateStateEnum {
		OPERATE_NOFILE,     /*!< no file is opened */
		OPERATE_NEWFILE,    /*!< new file, unsaved */
		OPERATE_SAVEDFILE   /*!< opened file is saved */
	};

	/*!
	 * Operation Event Code
	 */
	enum EventCode {
		FT_SOC_NEW    = 0,    /*!< create new soc config file */
		FT_SOC_OPEN   = 1,    /*!< open soc config file */
		FT_SOC_SAVE   = 2,    /*!< save soc config file */
		FT_SOC_SAVEAS = 3,    /*!< save soc config file as new file */
		FT_SOC_CLOSE  = 4,    /*!< close soc config file */
		FT_TOOLS_SOC_CONFIG           = 10,    /*!< soc configuration */
		FT_TOOLS_FAULT_CONFIG         = 11,    /*!< fault configuration */
        FT_TOOLS_ONLINE_SIMULATION    = 19,    /*!< online simulation */
        FT_TOOLS_REPLAY_SIMULATION    = 12,    /*!< replay simulation */
		FT_TOOLS_REPLAY_PATH          = 13,    /*!< replay paths */
		FT_TOOLS_ANALYSE_BENCHMARK    = 14,    /*!< analyse benchmark file */
		FT_TOOLS_ANALYSE_EVENTTRACE   = 15,    /*!< analyse event trace file */
		FT_TOOLS_ANALYSE_PATH_OVERLAP = 16,    /*!< overlap paths on one figure */
		FT_TOOLS_ANALYSE_PORT_TRAFFIC = 17,    /*!< port traffic distribution */
		FT_TOOLS_ANALYSE_TRAFFIC_WINDOW = 18,   /*!< traffic window */
		FT_OPTION_TOOLS_CFG       = 20,    /*!< configure link tools */
		FT_OPTION_RENEW_TOOLS     = 21,    /*!< recompile link tools */
		FT_OPTION_ENV_CONFIG      = 22,    /*!< working direction */
		FT_OPTION_GRAPHICS_CONFIG = 23,    /*!< graphics configuration */
		FT_ABOUT = 31,    /*!< about */
		FT_LINK_START = 40,    /*!< link tools:
			* FT_LINK_START + index of link tools */
	};

	/*!
	 * Toolbar Tab and Box Code
	 */
	enum ToolBarBoxTab {
		TB_BEGIN    = 00,    /*!< begin tab */
		TBT_NETWORK = 01,    /*!< begin tab, network box */
		TBT_OPTION  = 02,    /*!< begin tab, option box */
		TB_TOOLS     = 10,    /*!< tools tab */
		TBT_CONFIG   = 11,    /*!< tools tab, configuration box */
		TBT_SIMULATE = 12,    /*!< tools tab, simulation box */
		TBT_DEBUG    = 13,    /*!< tools tab, debug box */
		TBT_ANALYSE  = 14,    /*!< tools tab, analyse box */
		TB_CONTROL_BEGIN = 20,    /*!< control tab */
	};

    ToolBarButton * mp_online_simulation;
    ToolBarButton * mp_offline_simulation;

public:
	/*!
	 * \brief Constructor function
	 * \param parent pointer to parent widget
	 * \param flags windows flag
	 */
	Integrator( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

private:
	ToolBarWidget * mp_toolbar_widget;  /*!< pointer to toolbar tabwidget */
	ResultTabWidget * mp_result_tab_widget;	 /*!< pointer to result tabwidget */

	OperateStateEnum m_operate_state;  /*!< file operation state */
	QString m_soc_file_name;  /*!< network file name */

	LinkToolModel * mp_linktool_model;  /*!< pointer to link tools */
	GraphicsConfigModel * mp_graphics_config_model;  /*!< pointer to graphics
		* configurations */
	EsySoCCfg m_soc_cfg;  /*!< pointer to soc configuration */

private:
	/*!
	 * Draw structure of main window.
	 */
	void addStructure();
	/*!
	 * Draw tool bar.
	 */
	void addToolBarWidget();
	/*!
	 * Add link tools in tool bar
	 */
	void addLinkTool();
	/*!
	 * Clear link tools in tool bar
	 */
	void clearLinkTool();
	/*!
	 * \brief Set file operation state and change the enable states of components
	 * \param state file operation state
	 */
	void setOperateState( OperateStateEnum state );
	
private slots:
	/*!
	 * \brief Flow event handler, overloaded function.
	 * \param action pointer to the event action
	 */
	void flowEventHandler( QAction * action );
	/*!
	 * \brief Flow event handler, overloaded function.
	 * \param e event code
	 */
	void flowEventHandler( EventCode e );
	/*!
	 * \brief Change the tab in the toolbar when the result tab changed.
	 * Result tab will provide a few toolbar tabs and only the tabs of current
	 * result tab is visable.
	 * \param index index of current result tab
	 */
	void resultTabCurrentChangedHandler( int index );
};

#endif
