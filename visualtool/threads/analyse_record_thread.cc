#include "analyse_record_thread.h"

// ---- class AnalyseEventtraceThread ---- //
// no task specified
AnalyseRecordThread::AnalyseRecordThread() :
	m_task( ANALYSE_NONE ), mp_reader( 0 ), m_soc_cfg()
{
}
// load event trace
AnalyseRecordThread::AnalyseRecordThread(
	EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
	long long * total_cycle, QVector< long long > * total_cycle_list
) :
	m_task( ANALYSE_LOAD ), mp_reader( s ), m_soc_cfg( cfg ),
	mp_total_cycle( total_cycle ), mp_total_cycle_list( total_cycle_list )
{
}
AnalyseRecordThread::AnalyseRecordThread(
    EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
    long long * total_time, QVector< long > * total_variable
) :
    m_task( ANALYSE_GENERAL ), mp_reader( s ), m_soc_cfg( cfg ),
    mp_total_cycle( total_time ), mp_total_variables( total_variable )
{
}
AnalyseRecordThread::AnalyseRecordThread(
    EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
    long tile_id, long long total_time,
    long cycle_point, long cycle_scaling,
    QVector< QVector< double > > * y_variables
) :
    m_task( ANALYSE_TIME ), mp_reader( s ), m_soc_cfg( cfg ),
    m_total_time( total_time ), m_tile_id( tile_id ),
    m_cycle_point( cycle_point ), m_cycle_scaling( cycle_scaling ),
    mp_y_variable( y_variables )
{
}
AnalyseRecordThread::AnalyseRecordThread(
    EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
    long long total_time,
    QVector< QVector< double > > * y_area_variables,
    QVector< QVector< double > > * y_arae_tile_variables
) :
    m_task( ANALYSE_AREA ), mp_reader( s ), m_soc_cfg( cfg ),
    m_total_time( total_time ),
    mp_y_area_variable( y_area_variables ),
    mp_y_area_tile_variable( y_arae_tile_variables )
{
}
AnalyseRecordThread::AnalyseRecordThread(
    EsyDataFileIStream< EsyDataItemSoCRecord > * s, EsySoCCfg * cfg,
    long window_step, long window_width, long long total_time,
    QVector< QVector< QVector< double > > > * y_window_variables
) :
    m_task( ANALYSE_WINDOW ), mp_reader( s ), m_soc_cfg( cfg ),
    m_total_time( total_time ), m_window_step( window_step ), m_window_width( window_width ),
    mp_y_window_variable( y_window_variables )
{
}

// ---- class AnalyseEventtraceThread ---- //

void AnalyseRecordThread::run()
{
    mp_reader ->reset();
    updateProgress( 0, mp_reader->size() );

    switch( m_task )
    {
    case ANALYSE_NONE:
        break;
    case ANALYSE_LOAD:
        *mp_total_cycle = mp_reader->total();
		(*mp_total_cycle_list).fill( -1, m_soc_cfg->tileCount() );
        break;
    case ANALYSE_GENERAL:
        *mp_total_cycle = 0;
        (*mp_total_variables).resize( VARIABLE_COUNT );
        for (int i = 0 ; i < VARIABLE_COUNT; i ++ )
        {
            (*mp_total_variables)[ i ] = 0;
        }
        break;
    case ANALYSE_TIME:
        (*mp_y_variable).resize( VARIABLE_COUNT );
        for (int i = 0 ; i < VARIABLE_COUNT; i ++ )
        {
            (*mp_y_variable)[ i ].resize( m_cycle_point + 1 );
            for (int j = 0; j < m_cycle_point + 1; j ++ )
            {
                (*mp_y_variable)[ i ][ j ] = 0;
            }
        }
        break;
    case ANALYSE_AREA:
        (*mp_y_area_variable).resize( VARIABLE_COUNT );
        (*mp_y_area_tile_variable).resize( VARIABLE_COUNT );
        for (int i = 0 ; i < VARIABLE_COUNT; i ++ )
        {
            (*mp_y_area_tile_variable)[ i ].resize( m_soc_cfg->tileCount() );
            (*mp_y_area_variable)[ i ].resize( m_soc_cfg->network().routerCount() );
            for (size_t j = 0; j < m_soc_cfg->tileCount(); j ++ )
            {
                (*mp_y_area_tile_variable)[ i ][ j ] = 0;
            }
            for (int j = 0; j < m_soc_cfg->network().routerCount(); j ++ )
            {
                (*mp_y_area_variable)[ i ][ j ] = 0;
            }
        }
        break;
    case ANALYSE_WINDOW:
        (*mp_y_window_variable).resize( VARIABLE_COUNT );
        for (int var = 0 ; var < VARIABLE_COUNT; var ++ )
        {
            (*mp_y_window_variable)[ var ].resize( m_soc_cfg->tileCount() );
            int stepcount = (m_total_time - m_window_width) / m_window_step + 1;
            m_window_start_cycle.resize( stepcount );
            m_window_stop_cycle.resize( stepcount );
            for (size_t i = 0; i < m_soc_cfg->tileCount(); i ++)
            {
                int stepcount = (m_total_time - m_window_width) / m_window_step + 1;
                m_window_start_cycle.resize( stepcount );
                m_window_stop_cycle.resize( stepcount );
                (*mp_y_window_variable)[ var ][i].resize( stepcount );

                for (int j = 0; j < stepcount; j ++ )
                {
                    m_window_start_cycle[ j ] = j * m_window_step;
                    m_window_stop_cycle[ j ] = j * m_window_step + m_window_width;
                    (*mp_y_window_variable)[ var ][i][j] = 0.0;
                }
            }
        }
        break;
    default: break;
    }

    while ( !mp_reader ->eof() )
    {
        if ( isQuit() )
        {
            break;
        }

		EsyDataItemSoCRecord t_item = mp_reader ->getNextItem();
        //----analyse----//
//		long item_hop = qFloor(t_item.hop());
		int item_tile = t_item.actualId();

        switch( m_task )
        {
        case ANALYSE_NONE:
            break;
        case ANALYSE_LOAD:
            if ( t_item.hop() > *mp_total_cycle )
            {
				*mp_total_cycle = qCeil( t_item.hop() );
            }
			if ( t_item.simCycle() >
				 (*mp_total_cycle_list)[ item_tile ] )
			{
				(*mp_total_cycle_list)[ item_tile ] =
					qCeil( t_item.simCycle() );
			}
            break;
        case ANALYSE_GENERAL:
            if ( t_item.hop() > *mp_total_cycle )
            {
                *mp_total_cycle = qCeil( t_item.hop() );
            }
            (*mp_total_variables)[L1_INST_CACHE_ACCESS] += t_item.il1Access();
            (*mp_total_variables)[L1_INST_CACHE_HIT] += t_item.il1Hit();
            (*mp_total_variables)[L1_INST_CACHE_MISS] += t_item.il1Miss();
            (*mp_total_variables)[L1_DATA_CACHE_ACCESS] += t_item.dl1Access();
            (*mp_total_variables)[L1_DATA_CACHE_HIT] += t_item.dl1Hit();
            (*mp_total_variables)[L1_DATA_CACHE_MISS] += t_item.dl1Miss();
            (*mp_total_variables)[L2_INST_CACHE_ACCESS] += t_item.il2Access();
            (*mp_total_variables)[L2_INST_CACHE_HIT] += t_item.il2Hit();
            (*mp_total_variables)[L2_INST_CACHE_MISS] += t_item.il2Miss();
            (*mp_total_variables)[L2_DATA_CACHE_ACCESS] += t_item.dl2Access();
            (*mp_total_variables)[L2_DATA_CACHE_HIT] += t_item.dl2Hit();
            (*mp_total_variables)[L2_DATA_CACHE_MISS] += t_item.dl2Miss();
            (*mp_total_variables)[MEMORY_ACCESS] += t_item.memAccess();
            (*mp_total_variables)[MESSAGE_SEND] += t_item.msgSend();
            (*mp_total_variables)[MESSAGE_RECEIVE] += t_item.msgRecv();
            (*mp_total_variables)[MESSAGE_PROBE] += t_item.msgProbe();
            (*mp_total_variables)[INTEGER_ADDER_OPERATION] += t_item.addInteger();
            (*mp_total_variables)[INTEGAR_MULTIPLER_OPERATION] += t_item.multiIntegar();
            (*mp_total_variables)[FLOAT_ADDER_OPERATION] += t_item.addInteger();
            (*mp_total_variables)[FLOAT_MULTIPLER_OPERATION] += t_item.multiIntegar();
            break;
        case ANALYSE_TIME:
            if ( ( m_tile_id == -1 ) || ( m_tile_id >= 0 && t_item.actualId() == m_tile_id ) )
            {
                long long cp = qCeil( t_item.hop() ) / m_cycle_scaling;
                (*mp_y_variable)[L1_INST_CACHE_ACCESS][cp] += t_item.il1Access();
                (*mp_y_variable)[L1_INST_CACHE_HIT][cp] += t_item.il1Hit();
                (*mp_y_variable)[L1_INST_CACHE_MISS][cp] += t_item.il1Miss();
                (*mp_y_variable)[L1_DATA_CACHE_ACCESS][cp] += t_item.dl1Access();
                (*mp_y_variable)[L1_DATA_CACHE_HIT][cp] += t_item.dl1Hit();
                (*mp_y_variable)[L1_DATA_CACHE_MISS][cp] += t_item.dl1Miss();
                (*mp_y_variable)[L2_INST_CACHE_ACCESS][cp] += t_item.il2Access();
                (*mp_y_variable)[L2_INST_CACHE_HIT][cp] += t_item.il2Hit();
                (*mp_y_variable)[L2_INST_CACHE_MISS][cp] += t_item.il2Miss();
                (*mp_y_variable)[L2_DATA_CACHE_ACCESS][cp] += t_item.dl2Access();
                (*mp_y_variable)[L2_DATA_CACHE_HIT][cp] += t_item.dl2Hit();
                (*mp_y_variable)[L2_DATA_CACHE_MISS][cp] += t_item.dl2Miss();
                (*mp_y_variable)[MEMORY_ACCESS][cp] += t_item.memAccess();
                (*mp_y_variable)[MESSAGE_SEND][cp] += t_item.msgSend();
                (*mp_y_variable)[MESSAGE_RECEIVE][cp] += t_item.msgRecv();
                (*mp_y_variable)[MESSAGE_PROBE][cp] += t_item.msgProbe();
                (*mp_y_variable)[INTEGER_ADDER_OPERATION][cp] += t_item.addInteger();
                (*mp_y_variable)[INTEGAR_MULTIPLER_OPERATION][cp] += t_item.multiIntegar();
                (*mp_y_variable)[FLOAT_ADDER_OPERATION][cp] += t_item.addInteger();
                (*mp_y_variable)[FLOAT_MULTIPLER_OPERATION][cp] += t_item.multiIntegar();
            }
            break;
        case ANALYSE_AREA:
            {
                long tid = t_item.actualId();
                (*mp_y_area_tile_variable)[L1_INST_CACHE_ACCESS][tid] += t_item.il1Access();
                (*mp_y_area_tile_variable)[L1_INST_CACHE_HIT][tid] += t_item.il1Hit();
                (*mp_y_area_tile_variable)[L1_INST_CACHE_MISS][tid] += t_item.il1Miss();
                (*mp_y_area_tile_variable)[L1_DATA_CACHE_ACCESS][tid] += t_item.dl1Access();
                (*mp_y_area_tile_variable)[L1_DATA_CACHE_HIT][tid] += t_item.dl1Hit();
                (*mp_y_area_tile_variable)[L1_DATA_CACHE_MISS][tid] += t_item.dl1Miss();
                (*mp_y_area_tile_variable)[L2_INST_CACHE_ACCESS][tid] += t_item.il2Access();
                (*mp_y_area_tile_variable)[L2_INST_CACHE_HIT][tid] += t_item.il2Hit();
                (*mp_y_area_tile_variable)[L2_INST_CACHE_MISS][tid] += t_item.il2Miss();
                (*mp_y_area_tile_variable)[L2_DATA_CACHE_ACCESS][tid] += t_item.dl2Access();
                (*mp_y_area_tile_variable)[L2_DATA_CACHE_HIT][tid] += t_item.dl2Hit();
                (*mp_y_area_tile_variable)[L2_DATA_CACHE_MISS][tid] += t_item.dl2Miss();
                (*mp_y_area_tile_variable)[MEMORY_ACCESS][tid] += t_item.memAccess();
                (*mp_y_area_tile_variable)[MESSAGE_SEND][tid] += t_item.msgSend();
                (*mp_y_area_tile_variable)[MESSAGE_RECEIVE][tid] += t_item.msgRecv();
                (*mp_y_area_tile_variable)[MESSAGE_PROBE][tid] += t_item.msgProbe();
                (*mp_y_area_tile_variable)[INTEGER_ADDER_OPERATION][tid] += t_item.addInteger();
                (*mp_y_area_tile_variable)[INTEGAR_MULTIPLER_OPERATION][tid] += t_item.multiIntegar();
                (*mp_y_area_tile_variable)[FLOAT_ADDER_OPERATION][tid] += t_item.addInteger();
                (*mp_y_area_tile_variable)[FLOAT_MULTIPLER_OPERATION][tid] += t_item.multiIntegar();
            }
            break;
        case ANALYSE_WINDOW:
            for ( int j = 0; j < m_window_start_cycle.size(); j ++ )
            {
                if ( t_item.hop() >= m_window_start_cycle[ j ] && t_item.hop() < m_window_stop_cycle[ j ] )
                {
                    long tid = t_item.actualId();
                    (*mp_y_window_variable)[L1_INST_CACHE_ACCESS][tid][j] += t_item.il1Access();
                    (*mp_y_window_variable)[L1_INST_CACHE_HIT][tid][j] += t_item.il1Hit();
                    (*mp_y_window_variable)[L1_INST_CACHE_MISS][tid][j] += t_item.il1Miss();
                    (*mp_y_window_variable)[L1_DATA_CACHE_ACCESS][tid][j] += t_item.dl1Access();
                    (*mp_y_window_variable)[L1_DATA_CACHE_HIT][tid][j] += t_item.dl1Hit();
                    (*mp_y_window_variable)[L1_DATA_CACHE_MISS][tid][j] += t_item.dl1Miss();
                    (*mp_y_window_variable)[L2_INST_CACHE_ACCESS][tid][j] += t_item.il2Access();
                    (*mp_y_window_variable)[L2_INST_CACHE_HIT][tid][j] += t_item.il2Hit();
                    (*mp_y_window_variable)[L2_INST_CACHE_MISS][tid][j] += t_item.il2Miss();
                    (*mp_y_window_variable)[L2_DATA_CACHE_ACCESS][tid][j] += t_item.dl2Access();
                    (*mp_y_window_variable)[L2_DATA_CACHE_HIT][tid][j] += t_item.dl2Hit();
                    (*mp_y_window_variable)[L2_DATA_CACHE_MISS][tid][j] += t_item.dl2Miss();
                    (*mp_y_window_variable)[MEMORY_ACCESS][tid][j] += t_item.memAccess();
                    (*mp_y_window_variable)[MESSAGE_SEND][tid][j] += t_item.msgSend();
                    (*mp_y_window_variable)[MESSAGE_RECEIVE][tid][j] += t_item.msgRecv();
                    (*mp_y_window_variable)[MESSAGE_PROBE][tid][j] += t_item.msgProbe();
                    (*mp_y_window_variable)[INTEGER_ADDER_OPERATION][tid][j] += t_item.addInteger();
                    (*mp_y_window_variable)[INTEGAR_MULTIPLER_OPERATION][tid][j] += t_item.multiIntegar();
                    (*mp_y_window_variable)[FLOAT_ADDER_OPERATION][tid][j] += t_item.addInteger();
                    (*mp_y_window_variable)[FLOAT_MULTIPLER_OPERATION][tid][j] += t_item.multiIntegar();
                }
            }
            break;
        default:
			break;
		}
        updateProgress( mp_reader->pos(), mp_reader->size() );
    }

    if (m_task == ANALYSE_AREA)
    {
        for (int i = 0 ; i < VARIABLE_COUNT; i ++ )
        {
            for (size_t j = 0; j < m_soc_cfg->tileCount(); j ++ )
            {
                (*mp_y_area_variable)[ i ][ m_soc_cfg->tile( j ).niId() ] =
                    (*mp_y_area_tile_variable)[ i ][ j ];
            }
        }
    }
    updateProgress( mp_reader->size(), mp_reader->size() );
}

