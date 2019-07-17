#ifndef ANALYSE_BENCHMARK_THREAD_H
#define ANALYSE_BENCHMARK_THREAD_H

#include "../define/qt_include.h"

#include "esy_netcfg.h"
#include "esy_interdata.h"
#include "esy_eventtype.h"

#include "basic_thread.h"

// Reader benchmark and analyse specified task
class AnalyseBenchmarkThread : public BasicThread
{
	Q_OBJECT
	
	// enumerate definition
public:
	// task
    enum AnalyseTask
    {
		ANALYSE_NONE,		// none
		ANALYSE_GENERAL, 	// general 
		ANALYSE_INJECT_TIME, 	// injection vs time
		ANALYSE_DISTANCE, 	// distance
		ANALYSE_INJECT_AREA 	// traffic vs area
	};
	
	// constructor and deconstructor
public:
    AnalyseBenchmarkThread();
    AnalyseBenchmarkThread(
        EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetworkCfg * cfg,
        long * tpi, long * tfi, long long * cycle, double * dist, long *  mdist
	);
    AnalyseBenchmarkThread(
        EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetworkCfg * cfg,
		long src_id, long dst_id, long start, long length, 
		QVector< double > * y_pir, QVector< double > * y_fir
	);
    AnalyseBenchmarkThread(
        EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetworkCfg * cfg,
		long src_id, long dst_id, long start, long length, 
		long * md, QVector< double > * y_hop
	);
    AnalyseBenchmarkThread(
        EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetworkCfg * cfg,
        long id,  long start, long length,
        QVector< double > * y_traffic_inject,
        QVector< double > * y_traffic_accept
	);
	
private:
	// thread variables
    AnalyseTask m_task;	// task code
    EsyDataFileIStream< EsyDataItemBenchmark > * m_reader;	// file reader

	// network variables
    EsyNetworkCfg * m_network_cfg;
	// analyse control
    long m_src_addr;
    long m_dst_addr;
    long m_id_addr;
    long m_start_cycle;
    long m_length_cycle;
    // result variable -- general
    long * mp_total_packets_inject;
    long * mp_total_flits_inject;
    long long * mp_total_cycle;
    double * mp_total_distance;
    long * mp_max_dist;
	// result variable -- traffic vs time
    QVector< double > * mp_pir_time;
    QVector< double > * mp_fir_time;
	// result variable -- hop and dist
    QVector< double > * mp_hop_dist;
	// result variable -- traffic vs area
    QVector< double > * mp_traffic_inject;
    QVector< double > * mp_traffic_accept;
	
protected:
	void run();
};

#endif
