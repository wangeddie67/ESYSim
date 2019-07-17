#ifndef ANALYSE_EVENTTRACE_THREAD_H
#define ANALYSE_EVENTTRACE_THREAD_H

#include "../define/qt_include.h"
#include "../modelsview/path_trace_table.h"
#include "../modelsview/application_gantt_chart.h"

#include "esy_netcfg.h"
#include "esy_interdata.h"
#include "esy_eventtype.h"

#include "basic_thread.h"

// Reader event trace and analyse specified task
class AnalyseEventtraceThread : public BasicThread
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
		ANALYSE_INJECT_TIME, 	// injection vs time
		ANALYSE_TRAFFIC_TIME, 	// traffic vs time
		ANALYSE_DIST_LAT, 	// distance and latency
		ANALYSE_TRAFFIC_AREA, 	// traffic vs area
        ANALYSE_TRAFFIC_WINDOW,
        ANALYSE_PORT_TRAFFIC,
        ANALYSE_APPLICATION
	};
	
	// constructor and deconstructor
public:
	AnalyseEventtraceThread();	// none
    AnalyseEventtraceThread(	// load
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long long * total_cycle
    );
    AnalyseEventtraceThread(	// generation
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long * tpi, long *tpr, long * tfi, long * tfr,
        long long * tc, long long * tic,
        double * td, long * md, double * tl, long * ml,
        long *appreq, long *appenter, long *appexit
	);
    AnalyseEventtraceThread(  // generation
        EsyDataFileIStream<EsyDataItemEventtrace> *s, EsyNetworkCfg *cfg, long *tpi,
        long *tpr, long *tfi, long *tfr, long long *tc, long long *tic,
        double *td, long *md, double *tl, long *ml, long *appreq, long *appenter,
        long *appexit, QMap<long, QVector<double> > *in_finish_app_list);
	AnalyseEventtraceThread(	// inject vs time
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long src_id, long dst_id, long app_id, long long total_cycle,
        long cycle_point, long cycle_scaling,
		QVector< double > * y_pir, QVector< double > * y_fir
	);
	AnalyseEventtraceThread(	// traffic vs time
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long id, long app_id, long long total_cycle,
        long cycle_point, long cycle_scaling,
        QVector< double > * inject, QVector< double > * accept,
        QVector< double > * send, QVector< double > * receive
	) ;
	AnalyseEventtraceThread(	// distance & latency
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long src_id, long dst_id, long app_id, long md, long ml,
		QVector< double > * dist, QVector< double > * latency
	);
	AnalyseEventtraceThread(	// traffic vs area
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long app_id,
        QVector< double > * inject, QVector< double > * accept,
        QVector< double > * through
	);
    AnalyseEventtraceThread(	// traffic window
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long step, long width, long long total_cycle,
        QVector< QVector< double > > * inject, QVector< QVector< double > > * accept,
        QVector< QVector< double > > * through
    );
    AnalyseEventtraceThread(    // port traffic
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        int src, int dst, long app_id,
        long *totalapp, QVector< EsyDataItemTraffic > * list
    );
    AnalyseEventtraceThread (	// application
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long app_id, long long totalcycle, long totalapp,
        QVector< double > *appreq, QVector< double > *appenter,
        QVector< double > *appexit,
        GanttDate  *appevent// draw gantt
    );
    AnalyseEventtraceThread (	// application
        EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
        long app_id, long long totalcycle, long totalapp,
        QVector< double > *appreq, QVector< double > *appenter,
        QVector< double > *appexit,
        GanttDate  *appevent,// draw gantt
        QMap <long , QVector<double > >  * in_finish_app_list,
        QMap <long , QVector<double > >  * in_latency_list ,
        QMap <long , QVector<double > >  * in_distance_list_AWMD ,
        QMap <long , QVector<double > >  * in_distance_list_AMD
    );

private:
	// thread variables
	AnalyseTask m_task;	// task code
    EsyDataFileIStream< EsyDataItemEventtrace > * mp_reader;	// file reader

    QVector< bool > m_router_app_enable;

    // network variables
    EsyNetworkCfg * m_network_cfg;
    // analyse control fliter
    long m_src;
    long m_dst;
    long m_id;
    long m_app_id;
    // perlimitation
    long long int m_total_cycle;
    long m_max_distance;
    long m_max_length;
    long m_total_app;
    long m_window_step;
    long m_window_width;
    // cycle scaling
    long m_cycle_point;
    long m_cycle_scaling;
    // result variable -- general
    long long * mp_total_cycle;
    long long * mp_total_inject_cycle;
    long * mp_total_packets_inject;
	long * mp_total_packets_received;
	long * mp_total_flits_inject;
	long * mp_total_flits_received;
	double * mp_total_distance;
	long * mp_max_distance;
	double * mp_total_latency;
	long * mp_max_latency;
    long * mp_total_app_req;
    long * mp_total_app_start;
    long * mp_total_app_exit;
    // result variable -- inject vs time
    QVector< double > * mp_packet_time;
	QVector< double > * mp_flit_time;
	// result variable -- flit vs time
    QVector< double > * mp_flit_inject;
    QVector< double > * mp_flit_accept;
    QVector< double > * mp_flit_send;
    QVector< double > * mp_flit_receive;
	// result variable -- distance
    QVector< double > * mp_distance;
	// result variable -- latency
    QVector< double > * mp_latency;
	// result variable -- traffic vs area
    QVector< double > * mp_traffic_inject;
    QVector< double > * mp_traffic_accept;
    QVector< double > * mp_traffic_through;
    // result variable -- traffic window
    QVector< double > m_window_start_cycle;
    QVector< double > m_window_stop_cycle;
    QVector< QVector< double > > * mp_window_inject;
    QVector< QVector< double > > * mp_window_accept;
    QVector< QVector< double > > * mp_window_through;
    // result variable -- port traffic
    QVector< EsyDataItemTraffic > * mp_hop_count_list;
    // result variable -- Application
    QVector< double > * mp_app_req_time_list;
    QVector< double > * mp_app_enter_time_list;
    QVector< double > * mp_app_exit_time_list;
    GanttDate *mp_app_event_list; // draw gantt
    QMap <long , QVector<double > >  * finish_app_list;  // appid, < enter sche time, start time, end time>
    QMap <long , QVector<double > >  * latency_list ; // appid, <packets num, latency sum>
    QMap <long , QVector<double > >  * distance_list_AWMD ; // appid, <packets num, distance sum>
    QMap <long , QVector<double > >  * distance_list_AMD ; // appid, <path num, distance sum>
    QVector< long > petable; // app_id, petable[10] = 1 : pe10 used for app 1;
    QMap <long , QMap<long, QMap< long , bool>  >  >  edge;  //used to compute AMD,  appid, <src>    : edge[0][10][14] = true,  app 0 , the edge from pe10 to pe14 was been computed

protected:
	void run();
};

#endif
