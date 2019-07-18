#ifndef ANALYSE_TRAFFICWINDOW_THREAD_H
#define ANALYSE_TRAFFICWINDOW_THREAD_H

#include "../define/qt_include.h"

#include "esy_netcfg.h"
#include "esy_interdata.h"
#include "esy_eventtype.h"

#include "basic_thread.h"

class AnalyseTrafficWindowThread : public BasicThread
{
    // enumerate definition
public:
    // task
    enum AnalyseTask
    {
        ANALYSE_NONE,		// none
        ANALYSE_LOAD,
    };

    // constructor and deconstructor
public:
    AnalyseTrafficWindowThread();	// none
    AnalyseTrafficWindowThread(	// load
        EsyDataFileIStream< EsyDataItemTrafficWindow > * s, EsyNetworkCfg * cfg,
        long * total_cycle, long * window_width, long * window_step,
        long * max_inject, long * max_accept, long * max_through
    );

private:
    // thread variables
    AnalyseTask m_task;	// task code
    EsyDataFileIStream< EsyDataItemTrafficWindow > * mp_reader;	// file reader

    // network variables
    EsyNetworkCfg * m_network_cfg;
    // perlimitation
    long * mp_total_window;
    long * mp_window_width;
    long * mp_window_step;
    long * mp_max_inject;
    long * mp_max_accept;
    long * mp_max_through;

protected:
    void run();
};

#endif // ANALYSE_TRAFFICWINDOW_THREAD_H
