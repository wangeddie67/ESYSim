
#include <vector>
#include <string>
#include <iostream>

#include "window.h"

using namespace std;

WindowAnalyser::WindowAnalyser(long buffer_size, const string & window_file, 
    long router_num, long window_width, long window_step)
{
    mp_windowout = new EsyDataFileOStream< EsyDataItemTrafficWindow >( 
        1000, window_file, WINDOW_EXTENSION, false, true );
    m_window_count = 1;
    m_router_num = router_num;
    m_window_width = window_width;
    m_window_step = window_step;    
    m_window_list.push_back( EsyDataItemTrafficWindow( 
        0.0, m_window_width, m_router_num ) );
}

void WindowAnalyser::analyse(const EsyDataItemEventtrace & item)
{
    if ((item.type() != ET_FLIT_SWITCH) && 
        (item.type() != ET_PACKET_INJECT) && (item.type() != ET_PACKET_ACCEPT))
    {
        return;
    }
    
    while ((m_window_list[ m_window_list.size() - 1 ].start() + 
            m_window_step ) <= item.time())
    {
        m_window_list.push_back(EsyDataItemTrafficWindow( 
            m_window_list[m_window_list.size() - 1].start() + m_window_step,
            m_window_list[m_window_list.size() - 1].start() + m_window_step + 
            m_window_width, m_router_num));
        m_window_count ++;
    }

    for (int seq = 0; seq < m_window_list.size(); seq ++)
    {
        if ((item.time() >= m_window_list[seq].start()) &&
            (item.time() <  m_window_list[seq].end()))
        {
            if (item.type() == ET_FLIT_SWITCH)
            {
                m_window_list[seq].increaseThroughTraffic( 
                    item.src(), 1);
            }
            if (item.type() == ET_PACKET_INJECT)
            {
                m_window_list[seq].increaseInjectTraffic( 
                    item.src(), item.flitSize());
            }
            if (item.type() == ET_PACKET_ACCEPT)
            {
                m_window_list[seq].increaseAcceptTraffic( 
                    item.dst(), item.flitSize());
            }
        }
        else
        {
            mp_windowout->addNextItem(m_window_list[seq]);
            m_window_list.erase(m_window_list.begin() + seq);
            seq --;
        }
    }
}

void WindowAnalyser::finish()
{
    LINK_PROGRESS_INIT
    
    while (m_window_list.size() > 0)
    {
        LINK_PROGRESS_UPDATE(m_window_list[0].end(), 
            m_window_list[m_window_list.size() - 1].end())

        m_window_list.erase(m_window_list.begin());
    }
    mp_windowout->setTotal(m_window_count);
    mp_windowout->writeTotal();
    mp_windowout->close();

    LINK_PROGRESS_END
}
    
