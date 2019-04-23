#ifndef NOC_DIALOG_H
#define NOC_DIALOG_H

#include "../define/qt_include.h"

#include "result_base_dialog.h"

#include "esy_faultcfg.h"

#include "../modelsview/soc_cfg_tree.h"
#include "../modelsview/tile_state_table.h"
#include "../modelsview/graphics_config_tree.h"

#include "../threads/analyse_record_thread.h"
#include "../subwidgets/automation_control_panel.h"
#include "../subwidgets/soc_view.h"
#include "../subdialogs/process_dialog.h"

// draw NoC and show the states of each packet and router
class SoCDialog : public ResultBaseDialog
{
	Q_OBJECT
	
	// constructor and deconstructor
public:
	SoCDialog ( void );
	SoCDialog ( ResultTypeEnum type, EsySoCCfg* soc_cfg,
                GraphicsConfigModel * graphics_cfg );
	~SoCDialog ( void );
	
	// component variable
private:
	SoCScene * mp_soc_scene;	// graphic sense
	SoCView  * mp_soc_view;	// graphic view

	AutomationControlPanel * mp_control_panel;

	SoCCfgModel * mp_global_cfg_model;
	SoCCfgModel * mp_tile_cfg_model;
	SoCCfgModel * mp_router_cfg_model;

	TileStateModel * mp_tile_state_model;

	QLabel * mp_watch_id_label;
	QTabWidget * mp_config_tab_widget;

	ToolBarButton * mp_save_picture_button;    // save picture

	QWidget        * mp_state_widget;
	QSplitter      * mp_soc_splitter;	// layout
	QHBoxLayout    * mp_soc_layout;

	// network variable
private:
	EsySoCCfg * mp_soc_cfg;

	int m_watch_router;	// router watched in right part
	int m_watch_tile; // tile watched in right part

	QString m_trace_fname;
	EsyDataFileIStream< EsyDataItemSoCRecord > * m_record_reader;
	AnalyseRecordThread * mp_load_record_thread;
	long long int m_total_step;
	QVector< long long int > m_total_step_list;

	bool m_run_error_flag;
	bool m_jump_flag;
	long m_jump_watch_tile;
	long long int m_current_step;

    ProcessDialog * m_process_dialog;

public:
	bool setRecord( const QString & fname );
    void setCommand(const QString & direction , const QString &command);

private:
	void addComponents();

	void setSelectComponent();

	bool recordCheck( const EsyDataItemSoCRecord & recorditem );

	void updateComponents();

    // slots functions
public slots:
	void applyGlobalConfigHandler();
    void applyTileConfigHandler();
    void applyRouterConfigHandler();
    void savePictureHandler();

	void prevCycle();
	void nextCycle();
	void stopRun();
	void nextRun();
	void prevRun();
	void jumpRun();

    void keyReleaseEventHandler(QKeyEvent * event);
	void selectRouterEvent(QMouseEvent * event);

	void loadRecordStart();
	void loadRecordFinishedHandler();
};

#endif
