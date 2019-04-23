#ifndef ANALYSE_RECORDWINDOW_THREAD_H
#define ANALYSE_RECORDWINDOW_THREAD_H

#include "../define/qt_include.h"

#include "esy_soccfg.h"
#include "esy_interdata.h"

#include "basic_thread.h"

class AnalyseRecordWindowThread : public BasicThread
{
    // enumerate definition
public:
    // task
    enum AnalyseTask
    {
        ANALYSE_NONE,		// none
        ANALYSE_LOAD,
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
    AnalyseRecordWindowThread();	// none
    AnalyseRecordWindowThread(	// load
        EsyDataFileIStream< EsyDataItemRecordWindow > * s, EsySoCCfg * cfg,
        long * total_cycle, long * window_width, long * window_step,
        QVector< double > * max_value
    );
    double variable(const EsyDataItemRecordWindow & item, long var, long i);

private:
    // thread variables
    AnalyseTask m_task;	// task code
    EsyDataFileIStream< EsyDataItemRecordWindow > * mp_reader;	// file reader

    // network variables
    EsySoCCfg * m_soc_cfg;
    // perlimitation
    long * mp_total_window;
    long * mp_window_width;
    long * mp_window_step;
    QVector< double > * mp_max_value;

protected:
    void run();
    long maxValue(const vector< long > & values);
};

#endif // ANALYSE_TRAFFICWINDOW_THREAD_H
