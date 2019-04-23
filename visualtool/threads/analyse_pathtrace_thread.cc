#include "analyse_pathtrace_thread.h"

// ---- path trace thread ---- //
AnalysePathtraceThread::AnalysePathtraceThread() :
    m_type( LOAD_NONE ), m_reader( 0 ),
    m_total_path( 0 ), m_hop_count_list( 0 )
{
}

AnalysePathtraceThread::AnalysePathtraceThread( EsyDataFileIStream< EsyDataItemPathtrace > * s,
	long long int * total_path ) :
    m_type( LOAD_PATH ), m_reader( s ),
    m_total_path( total_path ), m_hop_count_list( 0 )
{
}

AnalysePathtraceThread::AnalysePathtraceThread( EsyDataFileIStream< EsyDataItemPathtrace > * s,
    QVector< EsyDataItemTraffic > * list, int src, int dst, int inter,
    int inport, int invc, int outport, int outvc ) :
    m_type( ANALYSE_PATH ), m_reader( s ),
    m_total_path( 0 ), m_hop_count_list( list ),
    m_src( src ), m_dst( dst ), m_inter( inter ),
    m_inport( inport ), m_invc( invc ), m_outport( outport ), m_outvc( outvc )
{
}

void AnalysePathtraceThread::run()
{
	m_reader ->reset();
	if ( m_type == LOAD_PATH )
	{
		*m_total_path = 0;
	}
	else if ( m_type == ANALYSE_PATH )
	{
		while ( m_hop_count_list ->size() > 0 )
		{
			m_hop_count_list ->remove( 0 );
		}
	}

    updateProgress( 0, m_reader->size() );

    // read the maxline
    while ( !m_reader ->eof() )
    {
        if ( isQuit() )
        {
            return;
        }

        EsyDataItemPathtrace t_item = m_reader ->getNextItem();
        if ( t_item .src() == -1 )
			break;
		
		if ( m_type == LOAD_PATH )
		{
            *m_total_path = m_reader->total();
            return;
		}
        else if ( m_type == ANALYSE_PATH ) {
            bool is_fliter = true;
            if ( m_src >= 0 && t_item.src() != m_src ) {
                is_fliter = false;
            }
            if ( m_dst >= 0 && t_item.dst() != m_dst ) {
                is_fliter = false;
            }
            if ( m_inter >= 0 && is_fliter ) {
                is_fliter = false;
                vector< EsyDataItemHop > pathhop = t_item .pathhoplist();
                for ( size_t hop = 0; hop < pathhop .size(); hop ++ ) {
                    if ( ( pathhop[ hop ].addr() == m_inter ) &&
                         ( m_inport == -1 || pathhop[ hop ].inputPc() == m_inport ) &&
                         ( m_invc == -1 || pathhop[ hop ].inputVc() == m_invc ) &&
                         ( m_outport == -1 || pathhop[ hop ].outputPc() == m_outport ) &&
                         ( m_outvc == -1 || pathhop[ hop ].outputVc() == m_outvc ) ) {
                        is_fliter = true;
                    }
                }
            }

            if ( is_fliter )
			{
				vector< EsyDataItemHop > pathhop = t_item .pathhoplist();
				// count path
				for ( size_t hop = 0; hop < pathhop .size(); hop ++ )
				{
                    EsyDataItemTraffic t_item( pathhop[ hop ].addr(),
                         pathhop[ hop ].inputPc(), pathhop[ hop ].inputVc(),
                         pathhop[ hop ].outputPc(), pathhop[ hop ].outputVc(),
                         0 );
                    int i;
                    for ( i = 0; i < m_hop_count_list->size(); i ++ )
                    {
                        if ( (*m_hop_count_list)[ i ] == t_item )
                        {
                            (*m_hop_count_list)[ i ].incTraffic();
                            break;
                        }
                    }
                    if ( i >= m_hop_count_list->size() )
                    {
                        t_item.incTraffic();
                        m_hop_count_list->append( t_item );
                    }
                }
			}
		}
        updateProgress( m_reader->pos(), m_reader->size() );
    }
    updateProgress( m_reader->size(), m_reader->size() );
}
// ---- path trace thread ---- //
