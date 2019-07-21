#include "analyse_benchmark_thread.h"

// ---- class AnalyseBenchmarkThread ---- ///
// ---- constructor and destructor ---- //
AnalyseBenchmarkThread::AnalyseBenchmarkThread() :
    m_task( ANALYSE_NONE )
{
}
AnalyseBenchmarkThread::AnalyseBenchmarkThread(
    EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetCfg * cfg,
    long * tpi, long * tfi, long long * cycle, double * dist, long * mdist
) :
    m_task( ANALYSE_GENERAL ),
    m_reader( s ), m_network_cfg( cfg ),
    mp_total_packets_inject( tpi ), mp_total_flits_inject( tfi ),
    mp_total_cycle( cycle ),
    mp_total_distance( dist ), mp_max_dist( mdist )
{
}
AnalyseBenchmarkThread::AnalyseBenchmarkThread(
    EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetCfg * cfg,
    long src_id, long dst_id, long start, long length,
    QVector< double > * y_pir, QVector< double > * y_fir
) :
    m_task( ANALYSE_INJECT_TIME ),
    m_reader( s ), m_network_cfg( cfg ),
    m_src_addr( src_id ), m_dst_addr( dst_id ),
    m_start_cycle(start), m_length_cycle(length),
    mp_pir_time( y_pir ), mp_fir_time( y_fir )
{
}
AnalyseBenchmarkThread::AnalyseBenchmarkThread(
    EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetCfg * cfg,
    long src_id, long dst_id, long start, long length,
    long * md, QVector< double > * y_hop
) :
    m_task( ANALYSE_DISTANCE ),
    m_reader( s ), m_network_cfg( cfg ),
    m_src_addr( src_id ), m_dst_addr( dst_id ),
    m_start_cycle(start), m_length_cycle(length),
    mp_max_dist( md ), mp_hop_dist(y_hop)
{
}
AnalyseBenchmarkThread::AnalyseBenchmarkThread(
    EsyDataFileIStream< EsyDataItemBenchmark > * s, EsyNetCfg * cfg,
    long id, long start, long length,
    QVector< double > * y_traffic_inject, QVector< double > * y_traffic_accept
) :
    m_task( ANALYSE_INJECT_AREA ),
    m_reader( s ), m_network_cfg( cfg ),
    m_id_addr( id ), m_start_cycle( start ), m_length_cycle( length ),
    mp_traffic_inject( y_traffic_inject ),
    mp_traffic_accept( y_traffic_accept )
{
}

void AnalyseBenchmarkThread::run()
{
    m_reader ->reset();
    updateProgress( 0, m_reader->size() );

    switch( m_task )
    {
    case ANALYSE_NONE: break;
    case ANALYSE_GENERAL:
        *mp_total_packets_inject = 0;
        *mp_total_flits_inject = 0;
        *mp_total_cycle = 0;
        *mp_total_distance = 0.0;
        *mp_max_dist = 0;
        break;
    case ANALYSE_INJECT_TIME:
        mp_pir_time ->resize( m_length_cycle + 1 );
        mp_fir_time ->resize( m_length_cycle + 1 );
        for (int i = 0; i < m_length_cycle + 1; i ++)
        {
            (*mp_pir_time)[i] = 0.0; (*mp_fir_time)[i] = 0.0;
        }
        break;
    case ANALYSE_DISTANCE:
        mp_hop_dist ->resize( *mp_max_dist + 1 );
        for (int i = 0; i < mp_hop_dist->size(); i ++)
        {
            (*mp_hop_dist)[i] = 0.0;
        }
        break;
    case ANALYSE_INJECT_AREA:
        mp_traffic_inject->resize( m_network_cfg->routerCount() );
        mp_traffic_accept->resize( m_network_cfg->routerCount() );
        for (int i = 0; i < m_network_cfg->routerCount(); i ++)
        {
            (*mp_traffic_inject)[ i ] = 0.0;
            (*mp_traffic_accept)[ i ] = 0.0;
        }
        break;
    default: break;
    }

    while ( ! m_reader ->eof() )
    {
        if ( isQuit() )
            break;

        EsyDataItemBenchmark t_item = m_reader ->getNextItem();
        if ( m_reader ->eof() )
            break;
        //----analyse----//
        switch( m_task )
        {
        case ANALYSE_NONE: break;
        case ANALYSE_GENERAL:
            if ( (*mp_total_cycle) < qFloor(t_item.time() ) )
            {
                (*mp_total_cycle) = qFloor(t_item.time() );
            }
            (*mp_total_packets_inject) += 1;
            (*mp_total_flits_inject) += t_item.size();
            {
            int t_dist = m_network_cfg->coordDistance(
                        (long)t_item.src(), (long)t_item.dst() );
            (*mp_total_distance) += t_dist;
            if ( (*mp_max_dist) < t_dist )
                (*mp_max_dist) = t_dist;
            }
            break;
        case ANALYSE_INJECT_TIME:
            if (qFloor(t_item. time()) >= m_start_cycle && qFloor(t_item. time()) <= (m_start_cycle + m_length_cycle))
            {
                int index = qFloor(t_item. time()) - m_start_cycle;
                if ( ( (m_src_addr == -1) || (m_src_addr == t_item .src() ) ) &&
                     ( (m_dst_addr == -1) || (m_dst_addr == t_item .dst() ) ) )
                {
                    (*mp_pir_time)[index] += 1;
                    (*mp_fir_time)[index] += t_item.size();
                }
            }
            break;
        case ANALYSE_DISTANCE:
            if (qFloor(t_item. time()) >= m_start_cycle && qFloor(t_item. time()) <= (m_start_cycle + m_length_cycle))
            {
                if ( ( (m_src_addr == -1) || (m_src_addr == t_item .src() ) ) &&
                     ( (m_dst_addr == -1) || (m_dst_addr == t_item .dst() ) ) )
                {
                    (*mp_hop_dist)[ m_network_cfg->coordDistance(
                        (long)t_item .src(), (long)t_item .dst() ) ] += 1;
                }
            }
            break;
        case ANALYSE_INJECT_AREA:
            if (qFloor(t_item. time()) >= m_start_cycle && qFloor(t_item. time()) <= (m_start_cycle + m_length_cycle))
            {
                if ( m_id_addr == -1 )
                {
                    (*mp_traffic_inject)[ t_item .src() ] += t_item.size();
                    (*mp_traffic_accept)[ t_item .dst() ] += t_item.size();
                }
                else
                {
                    if ( t_item .src() == m_id_addr )
                        (*mp_traffic_inject)[ t_item .dst() ] += t_item.size();
                    if ( t_item .dst() == m_id_addr )
                        (*mp_traffic_accept)[ t_item .src() ] += t_item.size();
                }
            }
            break;
        default: break;
        }

        updateProgress( m_reader->pos(), m_reader->size() );
    }
    updateProgress( m_reader->size(), m_reader->size() );
}
// ---- class AnalyseBenchmarkThread ---- ///

