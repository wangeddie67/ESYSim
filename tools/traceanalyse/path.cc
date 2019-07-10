
#include <vector>
#include <string>
#include <iostream>

#include "path.h"

using namespace std;

PathAnalyser::PathAnalyser(long buffer_size, const string & path_file)
{
    mp_pathout = new EsyDataFileOStream< EsyDataItemPathtrace >(
        buffer_size, path_file, PATHTRACE_EXTENSION, false, true);
    m_path_list_offset = 0;
}

void PathAnalyser::analyse(const EsyDataItemEventtrace & item)
{
    long seq = item.flitId() - m_path_list_offset;
    if (seq < 0)
    {
        return;
    }
    
    if ( item.type() == ET_PACKET_INJECT )
    {
        while (seq >= (long long int)m_path_list.size())
        {
            m_path_list.push_back(PathtraceItemHop());
            m_path_state.push_back(-1);
        }
        if (m_path_state[ seq ] == -1)    // new packet
        {
            m_path_list[ seq ].m_src = item.flitSrc();
            m_path_list[ seq ].m_dst = item.flitDst();
            m_path_state[ seq ] = 0;
        }
    }
    else if (item.type() == ET_FLIT_SWITCH && item.flitType() == 0) 
    {
        if (m_path_state[ seq ] == 0) // exist packet
        {
            m_path_list[ seq ].m_hop.push_back( 
                EsyDataItemHop(item.src(), 
                item.srcPc(), item.srcVc(), item.dstPc(), item.dstVc()));
        }
    }
    else if (item.type() == ET_PACKET_ACCEPT)
    {
        if (m_path_state[ seq ] == 0)
        {
            m_path_state[ seq ] = 1;
        }
        // write to file
        while (m_path_state.size() > 0)
        {
            if (m_path_state[ 0 ] == 1)
            {
                mp_pathout->addNextItem(EsyDataItemPathtrace( 
                    m_path_list[ 0 ].m_src, m_path_list[ 0 ].m_dst, 
                    m_path_list[ 0 ].m_hop ));
                m_path_list.erase(m_path_list.begin());
                m_path_state.erase(m_path_state.begin());
                m_path_list_offset ++;
            }
            else
            {
                break;
            }
        }
    }
}
    
void PathAnalyser::finish()
{
    LINK_PROGRESS_INIT
    
    long t_size = m_path_list_offset + m_path_state.size();

    while (m_path_state.size() > 0)
    {
        long t_pos = t_size - m_path_state.size();
        LINK_PROGRESS_UPDATE(t_pos, t_size)

        if (m_path_list[ 0 ].m_hop.size() > 0)
        {
            mp_pathout->addNextItem(EsyDataItemPathtrace( 
                m_path_list[ 0 ].m_src, m_path_list[ 0 ].m_dst, 
                m_path_list[ 0 ].m_hop));
        }
        m_path_list.erase(m_path_list.begin());
        m_path_state.erase(m_path_state.begin());
        m_path_list_offset ++;
    }
    mp_pathout->setTotal(m_path_list_offset);
    mp_pathout->writeTotal();
    mp_pathout->close();

    LINK_PROGRESS_END

    LINK_RESULT_APPEND(1, (double)m_path_list_offset)
}
