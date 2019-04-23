#ifndef NOC_DIALOG_H
#define NOC_DIALOG_H

#include "../define/qt_include.h"

#include "result_base_dialog.h"

#include "esy_faultcfg.h"

#include "../modelsview/network_cfg_tree.h"
#include "../modelsview/router_state_table.h"
#include "../modelsview/graphics_config_tree.h"

#include "../threads/analyse_eventtrace_thread.h"
#include "../threads/analyse_pathtrace_thread.h"
#include "../subwidgets/noc_view.h"
#include "../subwidgets/automation_control_panel.h"
#include "../subwidgets/analyse_fliter_panel.h"
#include "../subdialogs/process_dialog.h"

// draw NoC and show the states of each packet and router
class NoCDialog : public ResultBaseDialog
{
	Q_OBJECT
	
	// constructor and deconstructor
public:
    NoCDialog ( void );
    NoCDialog ( ResultTypeEnum type, EsyNetworkCfg* network_cfg,
                GraphicsConfigModel * graphics_cfg );
	~NoCDialog ( void );
	
	// component variable
private:
    NoCScene * mp_noc_scene;	// graphic sense
    NoCView  * mp_noc_view;	// graphic view

    AutomationControlPanel * mp_control_panel;
    AnalyseFilterPanel * mp_fliter_panel;

    QCheckBox 	* mp_checkbox_mapping;
    QHBoxLayout * mp_mapping_layout;

	QTabWidget * mp_view_tab_widget;

    NetworkCfgModel * mp_network_cfg_model;
    NetworkCfgModel * mp_router_cfg_model;

    RouterStateModel * mp_router_input_model;
    RouterStateModel * mp_router_output_model;
	
    PathTraceModel * mp_path_model;

    ToolBarButton * mp_save_picture_button;    // save picture

    QWidget        * mp_state_widget;
    QSplitter      * mp_noc_splitter;	// layout
    QHBoxLayout    * mp_noc_layout;

	// network variable
private:
    EsyNetworkCfg * mp_network_cfg;

    int m_watch_router;	// router watched in right part

    QString m_trace_fname;	// event trace file name;
    EsyDataFileIStream< EsyDataItemEventtrace > * m_event_trace_reader;
    EsyDataFileIStream< EsyDataItemPathtrace > * m_path_trace_reader;

    AnalyseEventtraceThread * mp_load_event_trace_thread;
    AnalysePathtraceThread * mp_load_path_trace_thread;
    long long int m_total_step;
    QVector< EsyDataItemTraffic > m_analyse_path_hop_list;
    long m_total_app;

    bool m_run_error_flag;
    bool m_jump_flag;
    long long int m_current_step;	// run cycle;

    ProcessDialog * m_process_dialog;

public:
	bool setEventtrace( const QString & fname );	// set event trace
	bool setPathtrace( const QString & fname );
    void setCommand(const QString & direction , const QString &command);

private:
    void addComponents();

    void setSelectRouter();

	void pathHandler( EsyDataItemPathtrace & pathitem );

    bool eventCheck( const EsyDataItemEventtrace & eventitem, bool forward );
	bool pathCheck( const EsyDataItemPathtrace & pathitem );

    void updateComponents();

    // slots functions
public slots:
    void applyNetworkConfigHandler();
    void applyRouterConfigHandler();
    void savePictureHandler();

    void prevCycle();	// previous cycle
    void nextCycle();	// next cycle
    void stopRun();		// stop run
    void nextRun();	// continue run
    void prevRun();	// continue previous
    void jumpRun();

    void keyReleaseEventHandler(QKeyEvent * event);
	void selectRouterEvent(QMouseEvent * event);

    void loadEventtraceStart();
    void loadEventtraceFinishedHandler();

    void loadPathtraceStart();
	void loadPathtraceFinishedHandler();

    void disableMappingOnlyChecker() { mp_checkbox_mapping->setEnabled( false ); }
};

#endif
