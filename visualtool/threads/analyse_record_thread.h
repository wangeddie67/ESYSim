#ifndef ANALYSE_RECORD_THREAD_H
#define ANALYSE_RECORD_THREAD_H

#include "../define/qt_include.h"
#include "../modelsview/path_trace_table.h"
#include "../modelsview/application_gantt_chart.h"

#include "esy_soccfg.h"
#include "esy_interdata.h"
#include "esy_eventtype.h"

#include "basic_thread.h"

// Reader event trace and analyse specified task
class AnalyseRecordThread : public BasicThread
{
	Q_OBJECT
	
	// enumerate definition
public:
	// task
    enum AnalyseTask
    {
		ANALYSE_NONE,		// none
        ANALYSE_LOAD,
		ANALYSE_GENERAL, 	// general 
        ANALYSE_TIME,
        ANALYSE_AREA,
        ANALYSE_WINDOW
	};
	
    enum AnalyseVariableEnum
    {
        L1_INST_CACHE_ACCESS = 0,
        L1_INST_CACHE_HIT,
        L1_INST_CACHE_MISS,
        L1_DATA_CACHE_ACCESS,
        L1_DATA_CACHE_HIT,
        L1_DATA_CACHE_MISS,
        L2_INST_CACHE_ACCESS,
        L2_INST_CACHE_HIT,
        L2_INST_CACHE_MISS,
        L2_DATA_CACHE_ACCESS,
        L2_DATA_CACHE_HIT,
        L2_DATA_CACHE_MISS,
        MEMORY_ACCESS,
        MESSAGE_SEND,
        MESSAGE_RECEIVE,
        MESSAGE_PROBE,
        INTEGER_ADDER_OPERATION,
        INTEGAR_MULTIPLER_OPERATION,
        FLOAT_ADDER_OPERATION,
        FLOAT_MULTIPLER_OPERATION,
        VARIABLE_COUNT
    };
	// constructor and deconstructor
public:
	AnalyseRecordThread();	// none
	AnalyseRecordThread(	// load
		EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
		long long * total_cycle, QVector< long long > * total_cycle_list
    );
    AnalyseRecordThread(
        EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
        long long * total_time, QVector< long > * total_variable
    );
    AnalyseRecordThread(
        EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
        long tile_id, long long total_time,
        long cycle_point, long cycle_scaling,
        QVector< QVector< double > > * y_variables
    );
    AnalyseRecordThread(
        EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
        long long total_time,
        QVector< QVector< double > > * y_arae_variables,
        QVector< QVector< double > > * y_arae_tile_variables
    );
    AnalyseRecordThread(
        EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
        long window_step, long window_width, long long total_time,
        QVector< QVector< QVector< double > > > * m_y_window_variables
    );

private:
	// thread variables
	AnalyseTask m_task;	// task code
	EsyDataFileIStream< EsyDataItemSoCRecord > * mp_reader;	// file reader

	// soc variables
	EsySoCCfg * m_soc_cfg;

    // perlimitation
    long long m_total_time;
    long m_tile_id;
    long m_window_step;
    long m_window_width;
    // cycle scaling
    long m_cycle_point;
    long m_cycle_scaling;

	// result variable -- general
	long long * mp_total_cycle;
	QVector< long long > * mp_total_cycle_list;

    QVector< long > * mp_total_variables;
    QVector< QVector< double > > * mp_y_variable;
    QVector< QVector< double > > * mp_y_area_variable;
    QVector< QVector< double > > * mp_y_area_tile_variable;
    QVector< QVector< QVector< double > > > * mp_y_window_variable;

    QVector< double > m_window_start_cycle;
    QVector< double > m_window_stop_cycle;

protected:
    void run();
};

#endif
