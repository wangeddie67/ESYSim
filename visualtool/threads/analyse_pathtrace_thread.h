#ifndef ANALYSE_PATHTRACE_THREAD_H
#define ANALYSE_PATHTRACE_THREAD_H

#include "../define/qt_include.h"
#include "../modelsview/path_trace_table.h"

#include "esy_interdata.h"
#include "basic_thread.h"

// load event trace thread
class AnalysePathtraceThread : public BasicThread
{
    Q_OBJECT
public:
    enum LoadTypeEnum
    {
        LOAD_PATH,
        ANALYSE_PATH,
        LOAD_NONE
    };

private:
    LoadTypeEnum m_type;
    EsyDataFileIStream< EsyDataItemPathtrace > * m_reader;
    long long int * m_total_path;
    QVector< EsyDataItemTraffic > * m_hop_count_list;
	int m_src;
	int m_dst;
    int m_inter;
    int m_inport;
    int m_invc;
    int m_outport;
    int m_outvc;

public:
    AnalysePathtraceThread();
    AnalysePathtraceThread( EsyDataFileIStream< EsyDataItemPathtrace > * s,
        long long int * total_cycle );
    AnalysePathtraceThread( EsyDataFileIStream< EsyDataItemPathtrace > * s,
        QVector< EsyDataItemTraffic > * m_hop_pair_list, int src, int dst,
        int inter, int inport, int invc, int outport, int outvc );

protected:
    void run();
};

#endif
