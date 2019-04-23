#ifndef ANALYSEDIALOG_H
#define ANALYSEDIALOG_H

#include "../define/qt_include.h"

#include "result_base_dialog.h"

#include "../threads/analyse_benchmark_thread.h"
#include "../threads/analyse_eventtrace_thread.h"
#include "../threads/analyse_trafficwindow_thread.h"

#include "../modelsview/analyse_result_table.h"

#include "../subwidgets/analyse_plot.h"
#include "../subwidgets/automation_control_panel.h"
#include "../subwidgets/analyse_fliter_panel.h"

/*!
 * \brief Result dialog for analysis functions
 */
class AnalyseDialog : public ResultBaseDialog
{
    Q_OBJECT

protected:
    /*!
     * \brief analysis tasks
     */
    enum AnalyseTaskEnum
    {
        GENERAL,         /**< general statistics */
        INJECT_TIME,     /**< injection vs. time */
        TRAFFIC_TIME,    /**< traffic vs. time */
        DISTANCE,        /**< distance distribution */
        LATENCY,         /**< latency distribution */
        TRAFFIC_AREA,    /**< traffic vs. area */
        TRAFFIC_WINDOW,  /**< traffic vs. area with window */
        APPLICATION,     /**< application statistics */
        TASK_COUNT  /**< total count of tasks */
	};

protected:
    static QStringList const_task_combobox_list;  /**< const stringlist for
        task combobox */

    /** @name component variables
     * pointers to the components in GUI. Only the components needed access
     * have member variables here.
     */
    ///@{
    AnalysePlot * mp_result_plot;  /**< pointer of result plot */
    /** @name components on toolbar tab "Control"
     */
    ///@{
    QComboBox * mp_task_combobox;  /**< pointer to combobox for task selection */
    QLabel    * mp_traffic_window_label;       /**< pointer to label "Window" */
    QLineEdit * mp_traffic_window_edit;        /**< pointer to edit window
        width */
    QLabel    * mp_traffic_window_unit_label;  /**< pointer to label "Cycle */
    QLabel    * mp_traffic_step_label;       /**< pointer to label "Step" */
    QLineEdit * mp_traffic_step_edit;        /**< pointer to edit step */
    QLabel    * mp_traffic_step_unit_label;  /**< pointer to label "Cycle" */
    AnalyseFilterPanel * mp_filter_panel;  /**< pointer to filter panel */
    AutomationControlPanel * mp_control_panel;  /**< pointer to control panel */
    AutomationControlPanel * mp_window_control_panel; /**< pointer to control
        panel to control the traffic window in Traffic Window task */
    ///@}
    /** @name components on toolbar tab "Figure"
     */
    ///@{
    QLabel    * mp_traffic_type_label;     /**< pointer to label "Traffic Type" */
    QComboBox * mp_traffic_type_combobox;  /**< pointer to combobox for traffic
        type */
    QLabel    * mp_traffic_draw_label;    /**< pointer to label "Figure Type" */
    QComboBox * mp_traffic_draw_combobox; /**< pointer to combobox for figure
        type */
    QLabel    * mp_gantt_app_label;     /**< pointer to label "App Id" */
    QComboBox * mp_gantt_app_combobox;  /**< pointer to combobox for application
        id */
    ToolBarBox * mp_figure_config_box;  /**< pointer to figure configuration box */
    PlotAxisConfigPanel * mp_axis_config_panel;  /**< pointer to axis
        configuration panel */
    PlotSizeConfigPanel * mp_size_config_panel;  /**< pointer to plot size
        configuration panel */
    ///@}
    ///@}

    EsyNetworkCfg m_network_cfg;  /**< network configuration structure */
    QString m_analyse_fname;  /**< file name to analyse */
    ResultTableModel * mp_result_model;  /**< pointer to result model */

    AnalyseBenchmarkThread * mp_analyse_benchmark_thread;   /**< thread to
        parse benchmark file */
    EsyDataFileIStream< EsyDataItemBenchmark > * mp_benchmark_reader; /**< reader to
        parse benchmark file */
    AnalyseEventtraceThread * mp_analyse_eventtrace_thread;    /**< thread to
        parse event trace file */
    EsyDataFileIStream< EsyDataItemEventtrace > * mp_eventtrace_reader;  /**< reader to
        parse event trace file */
    AnalyseTrafficWindowThread * mp_analyse_trafficwindow_thread;   /**< thread to
        parse traffic window file */
    EsyDataFileIStream< EsyDataItemTrafficWindow > * mp_trafficwindow_reader;
        /**< reader to parse traffic window file */

    /** @name analysis result
     * variables of analysis result. All variables have been demonstration and
     * used if necessary.
     */
    ///@{
    /** @name general statistics */
    ///@{
    long m_total_packets_inject;    /**< total number of injected packets */
    long m_total_packets_received;  /**< total number of received packets */
    long m_total_flits_inject;      /**< total number of injected flits */
    long m_total_flits_received;    /**< total number of received flits */
    long long m_total_cycle;        /**< total number of simulation cycles */
    long long m_total_inject_cycle; /**< total number of simulation cycles from
        the first cycle to the last cycle with packets injected */
    double m_total_distance;        /**< total number of distance of packets */
    long m_max_distance;            /**< maximum number of distance of packets */
    double m_total_latency;         /**< total number of latency of packets */
    long m_max_latency;             /**< maximum number of latency of packets */
    long m_total_app_req;           /**< total number of application request
        mapping */
    long m_total_app_start;         /**< total number of application mapped */
    long m_total_app_exit;          /**< total number of application finished */
    ///@}
    long m_total_cycle_scaling;           /**< scaling factor of total cycle */
    long m_total_cycle_point;             /**< number of total cycle after scaling */
    long m_inject_cycle_scaling;           /**< scaling factor of inject cycle */
    long m_inject_cycle_point;             /**< number of inject cycles after scaling */
    /** @name injection vs. time */
    ///@{
    QVector< double > m_x_inject_time;    /**< time cycle list, on x axis
        [cycle] */
    QVector< double > m_y_inject_packet;  /**< injected packet, on y axis
        [cycle] */
    QVector< double > m_y_inject_flit;    /**< injected flit, on y axis
        [cycle] */
    ///@}
    /** @name traffic vs. time */
    ///@{
    QVector< double > m_x_traffic_time;    /**< time cycle list, on x axis
        [cycle] */
    QVector< double > m_y_traffic_inject;  /**< injected flit, on y axis
        [cycle] */
    QVector< double > m_y_traffic_accept;  /**< accepted flit, on y axis
        [cycle] */
    QVector< double > m_y_traffic_send;    /**< send flit, on y axis [cycle] */
    QVector< double > m_y_traffic_receive; /**< received flit, on y axis
        [cycle] */
    QVector< double > m_y_traffic_buffer;  /**< buffered flit, on y axis
        [cycle] */
    ///@}
    /** @name distance count */
    ///@{
    QVector< double > m_x_dist_dist;  /**< distance list, on x axis  [dist] */
    QVector< double > m_y_dist_count; /**< distance count, on y axis [dist] */
    ///@}
    /** @name latency count */
    ///@{
    QVector< double > m_x_lat_lat;   /**< latency list, on x axis  [latency] */
    QVector< double > m_y_lat_count; /**< latency count, on y axis [latency] */
    ///@}
    /** @name traffic vs. area */
    ///@{
    QVector< double > m_x_area_router;  /**< router list, on x axis [router]*/
    QVector< double > m_y_area_inject;  /**< injected flit, on y axis [router]*/
    QVector< double > m_y_area_accept;  /**< accepted flit, on y axis [router]*/
    QVector< double > m_y_area_through; /**< through flit, on y axis [router]*/
    ///@}
    /** @name traffic vs. area in window */
    ///@{
    long m_window_width;    /**< width of window */
    long m_window_step;     /**< step of window */
    long m_current_window;  /**< sequence of current window */
    long m_total_window;    /**< total number of window */
    long m_max_inject;  /**< maxium value of injected flits among all windows */
    long m_max_accept;  /**< maxium value of accepted flits among all windows */
    long m_max_through;    /**< maxium value of through flits among all windows */
    QVector< QVector< double > > m_y_window_inject;  /**< injected flits for
        windows [router][window] */
    QVector< QVector< double > > m_y_window_accept;  /**< accepted flits for
        windows [router][window] */
    QVector< QVector< double > > m_y_window_through; /**< through flits for
        windows [router][window] */
    ///@}
    /** @name application statistics */
    ///@{
    QVector< double > m_app_id_list;
    QVector< double > m_app_req_time_list;
    QVector< double > m_app_start_time_list;
    QVector< double > m_app_exit_time_list;
    GanttDate m_app_event_list; // draw gantt

    QMap<long , QVector<double > >  finish_app_list;
    QMap<long , QVector<double > >  latency_list ; // appid, <packets num, latency sum>
    QMap<long , QVector<double > >  distance_list_AWMD ; // appid, <packets num, distance sum>
    QMap<long , QVector<double > >  distance_list_AMD ; // appid, <path num, distance sum>
    ///@}
    ///@}

    AnalyseTaskEnum m_current_task;

    bool m_run_error_flag;
    bool m_jump_flag;

public:
    AnalyseDialog();
    AnalyseDialog( ResultTypeEnum type, const EsyNetworkCfg & cfg );
    ~AnalyseDialog();

public:
    bool setAnalyseFile( QString & fname );
    void updateResultTable();

public slots:
    void taskChangedHandler( int index );
    void drawPlotHandler();

    void prevCycle();	// previous cycle
    void nextCycle();	// next cycle
    void stopRun();		// stop run
    void nextRun();	// continue run
    void prevRun();	// continue previous
    void jumpRun();

    void keyReleaseEventHandler(QKeyEvent * event);

    void analyseFinishedHandler();
    void analyseStartHandler();

    void savePicHandler();
    void saveDataHandler();

    void plotSizeEditingFinishedHandler();
    void plotResizeEventHandler();
};

#endif // ANALYSEDIALOG_H
