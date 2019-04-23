#include "analyse_recordwindow_thread.h"

AnalyseRecordWindowThread::AnalyseRecordWindowThread() :
    m_task( ANALYSE_NONE ), mp_reader( 0 ), m_soc_cfg()
{
}

AnalyseRecordWindowThread::AnalyseRecordWindowThread(
    EsyDataFileIStream< EsyDataItemRecordWindow > * s, EsySoCCfg * cfg,
    long * total_cycle, long * window_width, long * window_step,
    QVector< double > *max_value
) :
    m_task( ANALYSE_LOAD ), mp_reader( s ), m_soc_cfg( cfg ),
    mp_total_window( total_cycle ),
    mp_window_width( window_width ), mp_window_step( window_step ),
    mp_max_value( max_value )
{
}

long AnalyseRecordWindowThread::maxValue(const vector< long > & values)
{
    long max = 0;
    for (size_t i = 0; i < values.size(); i ++)
    {
        if (values[ i ] > max)
        {
            max = values[ i ];
        }
    }
    return max;
}

void AnalyseRecordWindowThread::run()
{
    mp_reader ->reset();
    updateProgress( 0, mp_reader->size() );

    switch( m_task )
    {
    case ANALYSE_NONE:
        break;
    case ANALYSE_LOAD:
        (*mp_max_value).resize(VARIABLE_COUNT);
        for (int i = 0; i < VARIABLE_COUNT; i ++)
        {
            (*mp_max_value)[ i ] = 0;
        }
        *mp_total_window = 0;
        {
            EsyDataItemRecordWindow first_item = mp_reader->getNextItem();
            EsyDataItemRecordWindow second_item = mp_reader->getNextItem();
            *mp_window_width = first_item.end() - first_item.start();
            *mp_window_step = second_item.start() - first_item.start();
            mp_reader->reset();
        }
        break;
    default: break;
    }

    EsyDataItemRecordWindow t_prev_item;

    while ( !mp_reader->eof() )
    {
        if ( isQuit() )
        {
            break;
        }

        EsyDataItemRecordWindow t_item = mp_reader->getNextItem();

        switch( m_task )
        {
        case ANALYSE_NONE:
            break;
        case ANALYSE_LOAD:
            (*mp_total_window) ++;

#define MAX_VALUE_FUN(itemfun, index) \
if ( maxValue( itemfun() ) > (*mp_max_value)[index] ) \
{ \
    (*mp_max_value)[index] = maxValue( itemfun() ); \
}
            MAX_VALUE_FUN( t_item.il1Access, L1_INST_CACHE_ACCESS )
            MAX_VALUE_FUN( t_item.il1Hit, L1_INST_CACHE_HIT )
            MAX_VALUE_FUN( t_item.il1Miss, L1_INST_CACHE_MISS )

            MAX_VALUE_FUN( t_item.dl1Access, L1_DATA_CACHE_ACCESS )
            MAX_VALUE_FUN( t_item.dl1Hit, L1_DATA_CACHE_HIT )
            MAX_VALUE_FUN( t_item.dl1Miss, L1_DATA_CACHE_MISS )

            MAX_VALUE_FUN( t_item.il2Access, L2_INST_CACHE_ACCESS )
            MAX_VALUE_FUN( t_item.il2Hit, L2_INST_CACHE_HIT )
            MAX_VALUE_FUN( t_item.il2Miss, L2_INST_CACHE_MISS )

            MAX_VALUE_FUN( t_item.dl2Access, L2_DATA_CACHE_ACCESS )
            MAX_VALUE_FUN( t_item.dl2Hit, L2_DATA_CACHE_HIT )
            MAX_VALUE_FUN( t_item.dl2Miss, L2_DATA_CACHE_MISS )

            MAX_VALUE_FUN( t_item.memAccess, MEMORY_ACCESS )
            MAX_VALUE_FUN( t_item.msgSend, MESSAGE_SEND )
            MAX_VALUE_FUN( t_item.msgRecv, MESSAGE_RECEIVE )
            MAX_VALUE_FUN( t_item.msgProbe, MESSAGE_PROBE )

            MAX_VALUE_FUN( t_item.addInteger, INTEGER_ADDER_OPERATION )
            MAX_VALUE_FUN( t_item.multiIntegar, INTEGAR_MULTIPLER_OPERATION )
            MAX_VALUE_FUN( t_item.addFloat, FLOAT_ADDER_OPERATION )
            MAX_VALUE_FUN( t_item.multiFloat, FLOAT_MULTIPLER_OPERATION )

            t_prev_item = t_item;
            break;
        default:
            break;
        }
        updateProgress( mp_reader->pos(), mp_reader->size() );
    }
    updateProgress( mp_reader->size(), mp_reader->size() );
}

double AnalyseRecordWindowThread::variable(
    const EsyDataItemRecordWindow & item, long var, long i)
{
    switch( var )
    {
    case L1_INST_CACHE_ACCESS : return item.il1Access()[ i ]; break;
    case L1_INST_CACHE_HIT : return item.il1Hit()[ i ]; break;
    case L1_INST_CACHE_MISS : return item.il1Miss()[ i ]; break;
    case L1_DATA_CACHE_ACCESS : return item.dl1Access()[ i ]; break;
    case L1_DATA_CACHE_HIT : return item.dl1Hit()[ i ]; break;
    case L1_DATA_CACHE_MISS : return item.dl1Miss()[ i ]; break;
    case L2_INST_CACHE_ACCESS : return item.il2Access()[ i ]; break;
    case L2_INST_CACHE_HIT : return item.il2Hit()[ i ]; break;
    case L2_INST_CACHE_MISS : return item.il2Miss()[ i ]; break;
    case L2_DATA_CACHE_ACCESS : return item.dl2Access()[ i ]; break;
    case L2_DATA_CACHE_HIT : return item.dl2Hit()[ i ]; break;
    case L2_DATA_CACHE_MISS : return item.dl2Miss()[ i ]; break;
    case MEMORY_ACCESS : return item.memAccess()[ i ]; break;
    case MESSAGE_SEND : return item.msgSend()[ i ]; break;
    case MESSAGE_RECEIVE : return item.msgRecv()[ i ]; break;
    case MESSAGE_PROBE : return item.msgProbe()[ i ]; break;
    case INTEGER_ADDER_OPERATION : return item.addInteger()[ i ]; break;
    case INTEGAR_MULTIPLER_OPERATION : return item.multiIntegar()[ i ]; break;
    case FLOAT_ADDER_OPERATION : return item.addFloat()[ i ]; break;
    case FLOAT_MULTIPLER_OPERATION : return item.multiFloat()[ i ]; break;
    default: return 0; break;
    }
}
