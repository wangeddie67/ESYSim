#ifndef ANALYSEDIALOG_H
#define ANALYSEDIALOG_H

#include "../define/qt_include.h"

#include "result_base_dialog.h"

#include "../threads/analyse_benchmark_thread.h"
#include "../threads/analyse_record_thread.h"
#include "../threads/analyse_recordwindow_thread.h"

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
        GENERAL,  /**< general statistics */
        VAR_TIME,     /**< variable vs. time */
        VAR_AREA,     /**< variable vs. area */
        VAR_WINDOW,   /**< variable window */
        TASK_COUNT
	};

protected:
    static QStringList const_task_combobox_list;  /**< const stringlist for
        task combobox */
    static QStringList const_variable_list; /**< const stringlist for variables */

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
    AutomationControlPanel * mp_control_panel;  /**< pointer to control panel */
    AutomationControlPanel * mp_window_control_panel; /**< pointer to control
        panel to control the traffic window in Traffic Window task */
    ///@}
    /** @name components on toolbar tab "Figure"
     */
    ///@{
    QLabel    * mp_tile_label;     /**< pointer to label "Tile ID" */
    QComboBox * mp_tile_combobox;  /**< pointer to combobox for tile */
    QLabel    * mp_variable_label;     /**< pointer to label "Variable" */
    QComboBox * mp_variable_combobox;  /**< pointer to combobox for variable */
    QLabel    * mp_traffic_draw_label;    /**< pointer to label "Figure Type" */
    QComboBox * mp_traffic_draw_combobox; /**< pointer to combobox for figure
        type */
//    QLabel    * mp_gantt_app_label;     /**< pointer to label "App Id" */
//    QComboBox * mp_gantt_app_combobox;  /**< pointer to combobox for application
//        id */
    ToolBarBox * mp_figure_config_box;  /**< pointer to figure configuration box */
    PlotAxisConfigPanel * mp_axis_config_panel;  /**< pointer to axis
        configuration panel */
    PlotSizeConfigPanel * mp_size_config_panel;  /**< pointer to plot size
        configuration panel */
    ///@}
    ///@}

    EsySoCCfg m_soc_cfg;  /**< network configuration structure */
    QString m_analyse_fname;  /**< file name to analyse */
    ResultTableModel * mp_result_model;  /**< pointer to result model */

    AnalyseRecordThread * mp_analyse_record_thread;    /**< thread to
        parse record file */
    EsyDataFileIStream< EsyDataItemSoCRecord > * mp_record_reader;  /**< reader to
        parse record file */
    AnalyseRecordWindowThread * mp_analyse_recordwindow_thread;   /**< thread to
        parse traffic window file */
    EsyDataFileIStream< EsyDataItemRecordWindow > * mp_recordwindow_reader;
        /**< reader to parse record window file */

    /** @name analysis result
     * variables of analysis result. All variables have been demonstration and
     * used if necessary.
     */
    ///@{
    /** @name general statistics */
    ///@{
    long long m_total_time;          /**< total execution time */
    long m_time_scaling;
    long m_time_point;
    QVector< long > m_total_variable;  /**< total value of variables */
    ///@}
    /** @name variable vs. time */
    ///@{
    QVector< double > m_x_time;    /**< time cycle list, on x axis
        [cycle] */
    QVector< QVector< double > > m_y_variables;
    ///@}
    /** @name traffic vs. area */
    ///@{
    QVector< double > m_x_area_tile;  /**< router list, on x axis [router]*/
    QVector< QVector< double > > m_y_area_tile_variables;
    QVector< QVector< double > > m_y_area_variables;
    ///@}
    /** @name traffic vs. area in window */
    ///@{
    long m_window_width;    /**< width of window */
    long m_window_step;     /**< step of window */
    long m_current_window;  /**< sequence of current window */
    long m_total_window;    /**< total number of window */
    QVector< QVector< QVector< double > > > m_y_window_variables;
    QVector< double > m_y_window_var_max;
    ///@}
    ///@}

    AnalyseTaskEnum m_current_task;

    bool m_run_error_flag;
    bool m_jump_flag;

public:
    AnalyseDialog();
    AnalyseDialog( ResultTypeEnum type, const EsySoCCfg & cfg );
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
