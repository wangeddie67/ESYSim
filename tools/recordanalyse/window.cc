
#include <vector>
#include <string>
#include <iostream>

#include "window.h"

using namespace std;

WindowAnalyser::WindowAnalyser(long buffer_size, const string & window_file, 
    long tile_num, long window_width, long window_step)
{
    mp_windowout = new EsyDataFileOStream< EsyDataItemRecordWindow >( 
        1000, window_file, RECORDWIN_EXTENSION, false, true );
    m_window_count = 1;
    m_tile_num = tile_num;
    m_window_width = window_width;
    m_window_step = window_step;    
    m_window_list.push_back( EsyDataItemRecordWindow( 
        0.0, m_window_width, m_tile_num ) );
}

void WindowAnalyser::analyse(const EsyDataItemSoCRecord & item)
{
    while ((m_window_list[ m_window_list.size() - 1 ].start() + 
            m_window_step ) <= item.hop())
    {
        m_window_list.push_back(EsyDataItemRecordWindow( 
            m_window_list[m_window_list.size() - 1].start() + m_window_step,
            m_window_list[m_window_list.size() - 1].start() + m_window_step + 
            m_window_width, m_tile_num));
        m_window_count ++;
    }

    for (int seq = 0; seq < m_window_list.size(); seq ++)
    {
        if ((item.hop() >= m_window_list[seq].start()) &&
            (item.hop() <  m_window_list[seq].end()))
        {
            long id = item.actualId();
            m_window_list[seq].increaseIl1Access(id, item.il1Access());
            m_window_list[seq].increaseIl1Hit(id, item.il1Hit());
            m_window_list[seq].increaseIl1Miss(id, item.il1Miss()); 
            m_window_list[seq].increaseDl1Access(id, item.dl1Access());
            m_window_list[seq].increaseDl1Hit(id, item.dl1Hit());
            m_window_list[seq].increaseDl1Miss(id, item.dl1Miss());
            m_window_list[seq].increaseIl2Access(id, item.il2Access());
            m_window_list[seq].increaseIl2Hit(id, item.il2Hit());
            m_window_list[seq].increaseIl2Miss(id, item.il2Miss());
            m_window_list[seq].increaseDl2Access(id, item.dl2Access());
            m_window_list[seq].increaseDl2Hit(id, item.dl2Hit());
            m_window_list[seq].increaseDl2Miss(id, item.dl2Miss());
            m_window_list[seq].increaseMemAccess(id, item.memAccess());
            m_window_list[seq].increaseMsgSend(id, item.msgSend());
            m_window_list[seq].increaseMsgRecv(id, item.msgRecv());
            m_window_list[seq].increaseMsgProbe(id, item.msgProbe());
            m_window_list[seq].increaseAddInteger(id, item.addInteger());
            m_window_list[seq].increaseMultiIntegar(id, item.multiIntegar());
            m_window_list[seq].increaseAddFloat(id, item.addFloat());
            m_window_list[seq].increaseMultiFloat(id, item.multiFloat());
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
    
