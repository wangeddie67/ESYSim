#include "analyse_trafficwindow_thread.h"

AnalyseTrafficWindowThread::AnalyseTrafficWindowThread() :
    m_task( ANALYSE_NONE ), mp_reader( 0 ), m_network_cfg()
{
}

AnalyseTrafficWindowThread::AnalyseTrafficWindowThread(
    EsyDataFileIStream< EsyDataItemTrafficWindow > * s, EsyNetworkCfg * cfg,
    long * total_cycle, long * window_width, long * window_step,
    long * max_inject, long * max_accept, long * max_through
) :
    m_task( ANALYSE_LOAD ), mp_reader( s ), m_network_cfg( cfg ),
    mp_total_window( total_cycle ),
    mp_window_width( window_width ), mp_window_step( window_step ),
    mp_max_inject( max_inject ), mp_max_accept( max_accept ),
    mp_max_through( max_through )
{
}

void AnalyseTrafficWindowThread::run()
{
    mp_reader ->reset();
    updateProgress( 0, mp_reader->size() );

    switch( m_task )
    {
    case ANALYSE_NONE:
        break;
    case ANALYSE_LOAD:
        *mp_total_window = 0;
        *mp_max_through = 0;
        *mp_max_accept = 0;
        *mp_max_inject = 0;
        {
            EsyDataItemTrafficWindow first_item = mp_reader->getNextItem();
            EsyDataItemTrafficWindow second_item = mp_reader->getNextItem();
            *mp_window_width = first_item.end() - first_item.start();
            *mp_window_step = second_item.start() - first_item.start();
            mp_reader->reset();
        }
        break;
    default: break;
    }

    EsyDataItemTrafficWindow t_prev_item;

    while ( !mp_reader->eof() )
    {
        if ( isQuit() )
        {
            break;
        }

        EsyDataItemTrafficWindow t_item = mp_reader->getNextItem();

        switch( m_task )
        {
        case ANALYSE_NONE:
            break;
        case ANALYSE_LOAD:
            (*mp_total_window) ++;

            for ( size_t i = 0; i < t_item.injectTraffic().size(); i ++ )
            {
                if ( t_item.injectTraffic()[ i ] > (*mp_max_inject) )
                {
                    (*mp_max_inject) = t_item.injectTraffic()[ i ];
                }
            }
            for ( size_t i = 0; i < t_item.acceptTraffic().size(); i ++ )
            {
                if ( t_item.acceptTraffic()[ i ] > (*mp_max_accept) )
                {
                    (*mp_max_accept) = t_item.acceptTraffic()[ i ];
                }
            }
            for ( size_t i = 0; i < t_item.throughTraffic().size(); i ++ )
            {
                if ( t_item.throughTraffic()[ i ] > (*mp_max_through) )
                {
                    (*mp_max_through) = t_item.throughTraffic()[ i ];
                }
            }

            t_prev_item = t_item;
            break;
        default:
            break;
        }
        updateProgress( mp_reader->pos(), mp_reader->size() );
    }
    updateProgress( mp_reader->size(), mp_reader->size() );
}
