#include "analyse_eventtrace_thread.h"

// ---- class AnalyseEventtraceThread ---- //
// no task specified
AnalyseEventtraceThread::AnalyseEventtraceThread() :
    m_task( ANALYSE_NONE ), mp_reader( 0 ), m_network_cfg(),
    m_src( -1 ), m_dst( -1 ), m_id( -1 ), m_app_id( -1 )
{
}
// load event trace
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long long * total_cycle
) :
    m_task( ANALYSE_LOAD ), mp_reader( s ), m_network_cfg( cfg ),
    m_src( -1 ), m_dst( -1 ), m_id( -1 ), m_app_id( -1 ),
    mp_total_cycle( total_cycle )
{
}
// general task
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long * tpi, long *tpr, long * tfi, long * tfr, long long * tc, long long * tic,
    double *td, long *md, double *tl, long * ml,
    long *appreq, long *appenter, long *appexit
) :
    m_task( ANALYSE_GENERAL ), mp_reader( s ), m_network_cfg( cfg ),
    m_src( -1 ), m_dst( -1 ), m_id( -1 ), m_app_id( -1 ),
    mp_total_cycle( tc ), mp_total_inject_cycle( tic ),
    mp_total_packets_inject( tpi ), mp_total_packets_received( tpr ),
    mp_total_flits_inject( tfi ), mp_total_flits_received( tfr ),
    mp_total_distance( td ), mp_max_distance( md ), mp_total_latency( tl ), mp_max_latency( ml ),
    mp_total_app_req( appreq ), mp_total_app_start( appenter ), mp_total_app_exit( appexit )
{
}
// general task
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream<EsyDataItemEventtrace> *s, EsyNetworkCfg *cfg, long *tpi,
    long *tpr, long *tfi, long *tfr, long long *tc, long long *tic, double *td,
    long *md, double *tl, long *ml, long *appreq, long *appenter, long *appexit,
    QMap<long, QVector<double> > *in_finish_app_list)
    : m_task(ANALYSE_GENERAL),
      //m_traffic_type(NONE_TRAFFIC),
      mp_reader(s),
      m_network_cfg(cfg),
      m_src(-1),
      m_dst(-1),
      m_id(-1),
      m_app_id(-1),
      mp_total_cycle(tc),
      mp_total_inject_cycle(tic),
      mp_total_packets_inject(tpi),
      mp_total_packets_received(tpr),
      mp_total_flits_inject(tfi),
      mp_total_flits_received(tfr),
      mp_total_distance(td),
      mp_max_distance(md),
      mp_total_latency(tl),
      mp_max_latency(ml),
      mp_total_app_req(appreq),
      mp_total_app_start(appenter),
      mp_total_app_exit(appexit),
      finish_app_list(in_finish_app_list) {}
// inject vs time
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long src_id, long dst_id, long app_id, long long total_cycle,
    long cycle_point, long cycle_scaling,
    QVector< double > * y_pir, QVector< double > * y_fir
) :
    m_task( ANALYSE_INJECT_TIME ), mp_reader( s ), m_network_cfg( cfg ),
    m_src( src_id ), m_dst( dst_id ), m_id( -1 ), m_app_id( app_id ),
    m_total_cycle( total_cycle ), m_cycle_point( cycle_point ), m_cycle_scaling( cycle_scaling ),
    mp_packet_time( y_pir ), mp_flit_time( y_fir )
{
}
// traffic vs time
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long id, long app_id, long long total_cycle,
    long cycle_point, long cycle_scaling,
    QVector< double > * inject, QVector< double > * accept,
    QVector< double > * send, QVector< double > * receive
) :
    m_task( ANALYSE_TRAFFIC_TIME ), mp_reader( s ), m_network_cfg( cfg ),
    m_src( -1 ), m_dst( -1 ), m_id( id ), m_app_id( app_id ),
    m_total_cycle( total_cycle ), m_cycle_point( cycle_point ), m_cycle_scaling( cycle_scaling ),
    mp_flit_inject( inject ), mp_flit_accept( accept ),
    mp_flit_send( send ), mp_flit_receive( receive )
{
}
// distance & latency
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long src_id, long dst_id, long app_id, long maxdist, long maxlat,
    QVector< double > * dist, QVector< double > * latency
) :
    m_task( ANALYSE_DIST_LAT ), mp_reader( s ), m_network_cfg( cfg ),
    m_src( src_id ), m_dst( dst_id ), m_id( -1 ), m_app_id( app_id ),
    m_max_distance( maxdist ), m_max_length( maxlat ),
    mp_distance( dist ), mp_latency( latency )
{
}
// traffic vs area
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long app_id,
    QVector< double > * inject, QVector< double > * accept,
    QVector< double > * through
) :
    m_task( ANALYSE_TRAFFIC_AREA ), mp_reader( s ), m_network_cfg( cfg ),
    m_src(-1 ), m_dst( -1 ), m_id( -1 ), m_app_id( app_id ),
    mp_traffic_inject( inject ), mp_traffic_accept( accept ),
    mp_traffic_through( through )
{
}
// traffic window
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long step, long width, long long total_cycle,
    QVector< QVector< double > > * inject, QVector< QVector< double > > * accept,
    QVector< QVector< double > > * through
) :
    m_task( ANALYSE_TRAFFIC_WINDOW ), mp_reader( s ), m_network_cfg( cfg ),
    m_src(-1 ), m_dst( -1 ), m_id( -1 ), m_app_id( -1 ),
    m_total_cycle( total_cycle ), m_window_step( step ), m_window_width( width ),
    mp_window_inject( inject ), mp_window_accept( accept ),
    mp_window_through( through )
{
}
// port traffic
AnalyseEventtraceThread::AnalyseEventtraceThread(
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    int src, int dst, long app_id,
    long * total_app, QVector< EsyDataItemTraffic > * list
) :
    m_task( ANALYSE_PORT_TRAFFIC ), mp_reader( s ), m_network_cfg( cfg ),
    m_src( src ), m_dst( dst ), m_id( -1 ), m_app_id( app_id ),
    mp_total_app_start( total_app ), mp_hop_count_list( list )
{
}
AnalyseEventtraceThread::AnalyseEventtraceThread (	// application
    EsyDataFileIStream< EsyDataItemEventtrace > * s, EsyNetworkCfg * cfg,
    long app_id, long long totalcycle, long totalapp,
    QVector< double > *appreq, QVector< double > *appenter, QVector< double > *appexit,
    GanttDate  *appevent
) :
    m_task( ANALYSE_APPLICATION ), mp_reader( s ), m_network_cfg( cfg ),
    m_src( -1 ), m_dst( -1 ), m_id( -1 ), m_app_id( app_id ),
    m_total_cycle( totalcycle ), m_total_app( totalapp ),
    mp_app_req_time_list( appreq ), mp_app_enter_time_list( appenter ),
    mp_app_exit_time_list( appexit ),
    mp_app_event_list( appevent )
{

}
AnalyseEventtraceThread::AnalyseEventtraceThread(  // application
    EsyDataFileIStream<EsyDataItemEventtrace> *s, EsyNetworkCfg *cfg, long app_id,
    long long totalcycle, long totalapp, QVector<double> *appreq,
    QVector<double> *appenter, QVector<double> *appexit, GanttDate *appevent,
    QMap<long, QVector<double> > *in_finish_app_list,
    QMap<long, QVector<double> > *in_latency_list,
    QMap<long, QVector<double> > *in_distance_list_AWMD,
    QMap<long, QVector<double> > *in_distance_list_AMD)
    : m_task(ANALYSE_APPLICATION),
    //  m_traffic_type(NONE_TRAFFIC),
      mp_reader(s),
      m_network_cfg(cfg),
      m_src(-1),
      m_dst(-1),
      m_id(-1),
      m_app_id(app_id),
      m_total_cycle(totalcycle),
      m_total_app(totalapp),
      mp_app_req_time_list(appreq),
      mp_app_enter_time_list(appenter),
      mp_app_exit_time_list(appexit),
      mp_app_event_list(appevent),
      finish_app_list(in_finish_app_list),
      latency_list(in_latency_list),
      distance_list_AWMD(in_distance_list_AWMD),
      distance_list_AMD(in_distance_list_AMD)
{
   // qDebug("in application ..");
}
// ---- class AnalyseEventtraceThread ---- //

void AnalyseEventtraceThread::run()
{
    mp_reader ->reset();
    updateProgress( 0, mp_reader->size() );

    if ( m_app_id == -1 )
    {
        m_router_app_enable.fill( true, m_network_cfg->routerCount() );
    }
    else
    {
        m_router_app_enable.fill( false, m_network_cfg->routerCount() );
    }
        petable.fill(-1, m_network_cfg->routerCount());

    switch( m_task )
    {
    case ANALYSE_NONE:
        break;
    case ANALYSE_LOAD:
        *mp_total_cycle = mp_reader->total();
        return;
        break;
    case ANALYSE_GENERAL:
        *mp_total_packets_inject = 0;
        *mp_total_packets_received = 0;
        *mp_total_flits_inject = 0;
        *mp_total_flits_received = 0;
        *mp_total_cycle = 0;
        *mp_total_inject_cycle = 0;
        *mp_total_distance = 0.0;
        *mp_max_distance = 0;
        *mp_total_latency = 0.0;
        *mp_max_latency = 0;
        *mp_total_app_req = 0;
        *mp_total_app_start = 0;
        *mp_total_app_exit = 0;
        break;
    case ANALYSE_INJECT_TIME:
        mp_packet_time ->resize( m_cycle_point + 1 );
        mp_flit_time ->resize( m_cycle_point + 1 );
        for ( int i = 0; i < ( m_cycle_point + 1 ); i ++ )
        {
            (*mp_packet_time)[i] = 0.0;
            (*mp_flit_time)  [i] = 0.0;
        }
        break;
    case ANALYSE_TRAFFIC_TIME:
        mp_flit_inject ->resize( m_cycle_point + 1 );
        mp_flit_accept ->resize( m_cycle_point + 1 );
        mp_flit_send ->resize( m_cycle_point + 1 );
        mp_flit_receive ->resize( m_cycle_point + 1 );
        for ( int i = 0; i < ( m_cycle_point + 1 ); i ++ )
        {
            (*mp_flit_inject)[i] = 0.0;
            (*mp_flit_accept)[i] = 0.0;
            (*mp_flit_send)[i] = 0.0;
            (*mp_flit_receive)[i] = 0.0;
        }
        break;
    case ANALYSE_DIST_LAT:
        mp_distance ->resize( m_max_distance + 1 );
        mp_latency  ->resize( m_max_length   + 1 );
        for ( int i = 0; i < mp_distance->size(); i ++ )
            (*mp_distance)[i] = 0.0;
        for ( int i = 0; i < mp_latency ->size(); i ++ )
            (*mp_latency) [i] = 0.0;
        break;
    case ANALYSE_TRAFFIC_AREA:
        mp_traffic_inject ->resize( m_network_cfg->routerCount() );
        mp_traffic_accept ->resize( m_network_cfg->routerCount() );
        mp_traffic_through->resize( m_network_cfg->routerCount() );
        for (int i = 0; i < m_network_cfg->routerCount(); i ++)
        {
            (*mp_traffic_inject)[i] = 0.0;
            (*mp_traffic_accept)[i] = 0.0;
            (*mp_traffic_through)[i] = 0.0;
        }
        break;
    case ANALYSE_TRAFFIC_WINDOW:
        mp_window_inject ->resize( m_network_cfg->routerCount() );
        mp_window_accept ->resize( m_network_cfg->routerCount() );
        mp_window_through->resize( m_network_cfg->routerCount() );
        for (int i = 0; i < m_network_cfg->routerCount(); i ++)
        {
            int stepcount = (m_total_cycle - m_window_width) / m_window_step + 1;
            m_window_start_cycle.resize( stepcount );
            m_window_stop_cycle.resize( stepcount );
            (*mp_window_inject)[i].resize( stepcount );
            (*mp_window_accept)[i].resize( stepcount );
            (*mp_window_through)[i].resize( stepcount );

            for (int j = 0; j < stepcount; j ++ )
            {
                m_window_start_cycle[ j ] = j * m_window_step;
                m_window_stop_cycle[ j ] = j * m_window_step + m_window_width;
                (*mp_window_inject)[i][j] = 0.0;
                (*mp_window_accept)[i][j] = 0.0;
                (*mp_window_through)[i][j] = 0.0;
            }
        }
        break;
    case ANALYSE_PORT_TRAFFIC:
        mp_hop_count_list ->clear();
        *mp_total_app_start = 0;
        break;
    case ANALYSE_APPLICATION:
        for ( int i = 0; i < mp_app_event_list->size(); i ++ )
        {
            (*mp_app_event_list)[i].clear();
        }
        mp_app_event_list->resize( m_network_cfg->routerCount() );

        mp_app_req_time_list ->resize( m_total_app );
        mp_app_enter_time_list ->resize( m_total_app );
        mp_app_exit_time_list ->resize( m_total_app );
        for (int i = 0; i < m_total_app; i ++)
        {
            (*mp_app_req_time_list)[i] = 0.0;
            (*mp_app_enter_time_list)[i] = 0.0;
            (*mp_app_exit_time_list)[i] = 0.0;
        }
    default: break;
    }

    while ( !mp_reader ->eof() )
    {
        if ( isQuit() )
        {
            break;
        }

        EsyDataItemEventtrace t_item = mp_reader ->getNextItem();
        //----analyse----//
        long item_time = qFloor(t_item. time());
        long item_point = item_time / m_cycle_scaling;
        int item_src = t_item .src();
        int item_dst = t_item .dst();

        // ---- application ---- //
        if ( m_app_id >= 0 )
        {
            if ( m_task == ANALYSE_TRAFFIC_AREA )
            {
                if ( t_item.type() == ET_APP_ENTER && t_item.src() == m_app_id )
                {
                    m_router_app_enable.fill( true );
                }
                else if ( t_item.type() == ET_APP_EXIT && t_item.src() == m_app_id )
                {
                    m_router_app_enable.fill( false );
                }
            }
            else
            {
                if ( t_item.type() == ET_PE_CHANGE )
                {
                    long pe_id = t_item.src();
                    long pe_old_state = (ProcessorItem::ProcessorStatus)t_item.srcPc();
                    long pe_new_state = (ProcessorItem::ProcessorStatus)t_item.srcVc();
                    long app_id = t_item.dst();

                    if ( app_id == m_app_id )
                    {
                        if ( pe_old_state == ProcessorItem::PE_RELEASE )
                        {
                            m_router_app_enable[ pe_id ] = true;
                        }
                        else if ( pe_new_state == ProcessorItem::PE_RELEASE )
                        {
                            m_router_app_enable[ pe_id ] = false;
                        }
                    }
                }
            }
        }

        switch( m_task )
        {
        case ANALYSE_NONE:
            break;
        case ANALYSE_LOAD:
            if ( t_item.time() > *mp_total_cycle )
            {
                *mp_total_cycle = qCeil( t_item .time() );
            }
            break;
        case ANALYSE_GENERAL:
            if ( t_item.type() == ET_PACKET_INJECT )
            {
                (*mp_total_packets_inject) += 1;
                (*mp_total_flits_inject)   += t_item.flitSize();
                if (*mp_total_inject_cycle < item_time)
                {
                    (*mp_total_inject_cycle) = item_time;
                }
                int t_dist = m_network_cfg->coordDistance(
                            (long)t_item.flitSrc(), (long)t_item.flitDst());
                (*mp_total_distance) += t_dist;
                if ( *mp_max_distance < t_dist )
                {
                    *mp_max_distance = t_dist;
                }
            }
            if ( t_item.type() == ET_PACKET_ACCEPT )
            {
                (*mp_total_packets_received) += 1;
                (*mp_total_flits_received) += t_item.flitSize();
                double t_lat = t_item .time()  - t_item .flitStartTime() + 1;
                (*mp_total_latency) += t_lat;
                if ( *mp_max_latency < t_lat )
                {
                    *mp_max_latency = qFloor( t_lat );
                }
            }

            if (t_item.type() == ET_APP_REQUEST)
             {
                 long app_id = t_item.src();
                 double requestTime = t_item.time();
                 (*finish_app_list)[app_id].resize(3);
                 (*finish_app_list)[app_id][0] = requestTime;
                 (*finish_app_list)[app_id][1] = -1;
                 (*finish_app_list)[app_id][2] = -1;
             }
             if (t_item.type() == ET_APP_ENTER)
             {
                 long app_id = t_item.src();
                 double enterTime = t_item.time();
                 (*finish_app_list)[app_id][1] = enterTime;
             }
             if (t_item.type() == ET_APP_EXIT)
             {
                 long app_id = t_item.src();
                 double exitTime = t_item.time();
                 (*finish_app_list)[app_id][2] = exitTime;
             }

            if ( t_item .type() == ET_APP_REQUEST )
            {
                (*mp_total_app_req) ++;
            }
            if ( t_item .type() == ET_APP_ENTER )
            {
                (*mp_total_app_start) ++;
            }
            if ( t_item .type() == ET_APP_EXIT )
            {
                (*mp_total_app_exit) ++;
            }
            if ( (*mp_total_cycle) < item_time )
            {
                (*mp_total_cycle) = item_time;
            }
            break;
        case ANALYSE_INJECT_TIME:
            if ( t_item .type() == ET_PACKET_INJECT )
            {
                if ( ( ( m_src == -1 ) || ( m_src == item_src ) ) &&
                     ( ( m_dst == -1 ) || ( m_dst == item_dst ) ) &&
                     m_router_app_enable[ item_src ] && m_router_app_enable[ item_dst ] )
                {
                    (*mp_packet_time)[item_point] += 1;
                    (*mp_flit_time)  [item_point] += t_item.flitSize();
                }
            }
            break;
        case ANALYSE_TRAFFIC_TIME:
            if ( t_item .type() == ET_FLIT_RECEIVE )
            {
                if ( ( ( m_id == -1 ) || ( m_id == item_dst ) ) &&
                     m_router_app_enable[ t_item.flitSrc() ] &&
                     m_router_app_enable[ t_item.flitDst() ] )
                {
                    if ( m_network_cfg->router( item_dst ).
                         port( t_item.dstPc() ).networkInterface() )
                    {
                        (*mp_flit_inject)[item_point] += 1;
                    }
                    (*mp_flit_send)[item_point] += 1;
                }
            }
            if ( t_item .type() == ET_FLIT_SEND )
            {
                if ( ( ( m_id == -1 ) || ( m_id == item_src ) ) &&
                     m_router_app_enable[ t_item.flitSrc() ] &&
                     m_router_app_enable[ t_item.flitDst() ] )
                {
                    if ( m_network_cfg->router( item_src ).
                         port( t_item.srcPc() ).networkInterface() )
                    {
                        (*mp_flit_accept)[item_point] += 1;
                    }
                    (*mp_flit_receive)[item_point] += 1;
                }
            }
            break;
        case ANALYSE_DIST_LAT:
            if ( t_item.type() == ET_PACKET_ACCEPT )
            {
                if ( ( ( m_src == -1 ) || ( m_src == item_src ) ) &&
                     ( ( m_dst == -1 ) || ( m_dst == item_dst ) ) &&
                     m_router_app_enable[ item_src ] && m_router_app_enable[ item_dst ] )
                {
                    (*mp_distance)[m_network_cfg->coordDistance(
                                (long)t_item.flitSrc(), (long)t_item.flitDst())] += 1;
                    int dist = qFloor(t_item .time() - t_item .flitStartTime() + 1);
                    (*mp_latency)[dist] += 1;
                }
            }
            break;
        case ANALYSE_TRAFFIC_AREA:
            if ( m_router_app_enable[ 0 ] )
            {
                if ( t_item .type() == ET_PACKET_INJECT )
                {
                    (*mp_traffic_inject)[ item_src ] += t_item .flitSize();
                }
                if ( t_item .type() == ET_PACKET_ACCEPT )
                {
                    (*mp_traffic_accept)[ item_dst ] += t_item .flitSize();
                }
                if ( t_item .type() == ET_FLIT_SWITCH )
                {
                    (*mp_traffic_through)[ item_dst ] += 1;
                }
            }
            break;
        case ANALYSE_TRAFFIC_WINDOW:
            if ( t_item .type() == ET_PACKET_INJECT || t_item .type() == ET_PACKET_ACCEPT ||
                 t_item .type() == ET_FLIT_SWITCH )
            {
                for ( int j = 0; j < m_window_start_cycle.size(); j ++ )
                {
                    if ( item_time >= m_window_start_cycle[ j ] && item_time < m_window_stop_cycle[ j ] )
                    {
                        if ( t_item .type() == ET_PACKET_INJECT )
                        {
                            (*mp_window_inject)[ item_src ][ j ] += t_item .flitSize();
                        }
                        if ( t_item .type() == ET_PACKET_ACCEPT )
                        {
                            (*mp_window_accept)[ item_dst ][ j ] += t_item .flitSize();
                        }
                        if ( t_item .type() == ET_FLIT_SWITCH )
                        {
                            (*mp_window_through)[ item_dst ][ j ] += 1;
                        }
                    }
                }
            }
            break;
        case ANALYSE_PORT_TRAFFIC:
            if ( t_item .type() == ET_FLIT_SWITCH )
            {
                if ( ( ( m_src == -1 ) || ( m_src == item_src ) ) &&
                     ( ( m_dst == -1 ) || ( m_dst == item_dst ) ) &&
                     m_router_app_enable[ t_item.flitSrc() ] &&
                     m_router_app_enable[ t_item.flitDst() ] )
                {
                    EsyDataItemTraffic t_path_item( t_item.src(),
                         t_item.srcPc(), t_item.srcVc(),
                         t_item.dstPc(), t_item.dstVc(),
                         0 );
                    int i;
                    for ( i = 0; i < mp_hop_count_list->size(); i ++ )
                    {
                        if ( (*mp_hop_count_list)[ i ] == t_path_item )
                        {
                            (*mp_hop_count_list)[ i ].incTraffic();
                            break;
                        }
                    }
                    if ( i >= mp_hop_count_list->size() )
                    {
                        t_path_item.incTraffic();
                        mp_hop_count_list->append( t_path_item );
                    }
                }
            }
            if ( t_item .type() == ET_APP_ENTER )
            {
                (*mp_total_app_start) ++;
            }
            break;
        case ANALYSE_APPLICATION:
            //qDebug("in postion 1 ..");
                    // To generate the finish_app_list
                    if (t_item.type() == ET_APP_REQUEST)
                    {
                        long app_id = t_item.src();
                        double requestTime = t_item.time();
                        (*finish_app_list)[app_id].resize(3);
                        (*finish_app_list)[app_id][0] = requestTime;
                        (*finish_app_list)[app_id][1] = -1;
                        (*finish_app_list)[app_id][2] = -1;
                    }
                    if (t_item.type() == ET_APP_ENTER)
                    {
                        long app_id = t_item.src();
                        double enterTime = t_item.time();
                        (*finish_app_list)[app_id][1] = enterTime;
                    }
                    if (t_item.type() == ET_APP_EXIT)
                    {
                        long app_id = t_item.src();
                        double exitTime = t_item.time();
                        (*finish_app_list)[app_id][2] = exitTime;
                    }

                    //qDebug("in postion after To generate the finish_app_list ..");

                    // to get the AWMD\AMD\latency
                    // 1. get a app mapping map
                    if (t_item.type() == ET_PE_CHANGE)
                    {
                        long pe_id = t_item.src();
                        long pe_old_state = (ProcessorItem::ProcessorStatus)t_item.srcPc();
                        long pe_new_state = (ProcessorItem::ProcessorStatus)t_item.srcVc();
                        long app_id = t_item.dst();
                        if (pe_old_state == ProcessorItem::PE_RELEASE)
                        {
                            petable[pe_id] = app_id;
                        }
                        else if (pe_new_state == ProcessorItem::PE_RELEASE)
                        {
                            petable[pe_id] = -1;
                        }
                    }

                  //  qDebug("in postion after 1. get a app mapping map");
                    // 2. compute ..
                    if (t_item.type() == ET_PACKET_ACCEPT)
                    {
                        int src = t_item.src();
                        int des = t_item.dst();
                        int app_id_of = petable[src];
                        if ((*latency_list)[app_id_of].size() == 0)
                        {
                            (*latency_list)[app_id_of].push_back(1);
                            (*latency_list)[app_id_of].push_back(
                                qFloor(t_item.time() - t_item.flitStartTime() + 1));
                        }
                        else
                        {
                            (*latency_list)[app_id_of][0] = (*latency_list)[app_id_of][0] + 1;
                            (*latency_list)[app_id_of][1] =
                                (*latency_list)[app_id_of][1] +
                                qFloor(t_item.time() - t_item.flitStartTime() + 1);
                            // if (app_id_of == 0)
                            //{
                            //    qDebug()<< app_id_of<< (*latency_list)[app_id_of][0] <<
                            //    (*latency_list)[app_id_of][1]<<qFloor(t_item .time() - t_item
                            //    .flitStartTime() + 1);
                            //}
                        }

                        if ((*distance_list_AWMD)[app_id_of].size() == 0)
                        {
                            (*distance_list_AWMD)[app_id_of].push_back(1);
                            (*distance_list_AWMD)[app_id_of].push_back(
                                m_network_cfg->coordDistance((long)t_item.flitSrc(),
                                                             (long)t_item.flitDst()));
                        }
                        else
                        {
                            (*distance_list_AWMD)[app_id_of][0] =
                                (*distance_list_AWMD)[app_id_of][0] + 1;
                            (*distance_list_AWMD)[app_id_of][1] =
                                (*distance_list_AWMD)[app_id_of][1] +
                                m_network_cfg->coordDistance((long)t_item.flitSrc(),
                                                             (long)t_item.flitDst());
                        }

                        if ((*distance_list_AMD)[app_id_of].size() == 0)
                        {
                            (*distance_list_AMD)[app_id_of].push_back(1);
                            (*distance_list_AMD)[app_id_of].push_back(
                                m_network_cfg->coordDistance((long)t_item.flitSrc(),
                                                             (long)t_item.flitDst()));
                            edge[app_id_of][src][des] = true;
                        }
                        else
                        {
                            if (edge[app_id_of].contains(src))
                            {
                                if (edge[app_id_of][src].contains(des))
                                {
                                }
                                else
                                {
                                    (*distance_list_AMD)[app_id_of][0] =
                                        (*distance_list_AMD)[app_id_of][0] + 1;
                                    (*distance_list_AMD)[app_id_of][1] =
                                        (*distance_list_AMD)[app_id_of][1] +
                                        m_network_cfg->coordDistance((long)t_item.flitSrc(),
                                                                     (long)t_item.flitDst());
                                    edge[app_id_of][src][des] = true;
                                }
                            }
                            else
                            {
                                (*distance_list_AMD)[app_id_of][0] =
                                    (*distance_list_AMD)[app_id_of][0] + 1;
                                (*distance_list_AMD)[app_id_of][1] =
                                    (*distance_list_AMD)[app_id_of][1] +
                                    m_network_cfg->coordDistance((long)t_item.flitSrc(),
                                                                 (long)t_item.flitDst());
                                edge[app_id_of][src][des] = true;
                            }
                        }
                    }

                    // qDebug("in postion 2 ..");

                    if (t_item.type() == ET_APP_REQUEST)
                    {
                        long app_id = t_item.src();
                        (*mp_app_req_time_list)[app_id] = t_item.time();
                    }
                    if (t_item.type() == ET_APP_ENTER)
                    {
                        long app_id = t_item.src();
                        (*mp_app_enter_time_list)[app_id] = t_item.time();
                    }
                    if (t_item.type() == ET_APP_EXIT)
                    {
                        long app_id = t_item.src();
                        (*mp_app_exit_time_list)[app_id] = t_item.time();
                    }
                    if (t_item.type() == ET_PE_CHANGE) // for gantt graph
                    {
                        long pe_id = t_item.src();
                        double pe_time = t_item.time();
                        long app_id = t_item.dst();
                        if ((*mp_app_event_list)[pe_id].size() > 0)
                        {
                            if (t_item.srcPc() != ProcessorItem::PE_RELEASE)
                            {
                                if ((app_id == m_app_id) || (m_app_id == -1))
                                {
                                    (*mp_app_event_list)[pe_id][(*mp_app_event_list)[pe_id].size() -
                                                                1].setExitTime(pe_time);
                                }
                            }
                        }

                        if ((app_id == m_app_id) || (m_app_id == -1))
                        {
                            ProcessorItem::ProcessorStatus pe_new_state =
                                (ProcessorItem::ProcessorStatus)t_item.srcVc();
                            if (pe_new_state != ProcessorItem::PE_RELEASE)
                            {
                                long task_id = t_item.dstPc();
                                ProcessorItem p_item(app_id, task_id, pe_new_state, pe_time,
                                                     m_total_cycle);
                                (*mp_app_event_list)[pe_id].append(p_item);
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
        updateProgress( mp_reader->pos(), mp_reader->size() );
    }
    updateProgress( mp_reader->size(), mp_reader->size() );
}

