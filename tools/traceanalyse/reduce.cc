
#include <vector>
#include <string>
#include <iostream>

#include "reduce.h"

using namespace std;

ReduceAnalyser::ReduceAnalyser(
    long buffer_size, const string & trace_file, bool text_enable)
{
    mp_eventout = new EsyDataFileOStream< EsyDataItemEventtrace >( 
        buffer_size, trace_file + "s", EVENTTRACE_EXTENSION, 
        !text_enable, text_enable);
    m_total_event = 0;
}

void ReduceAnalyser::analyse(const EsyDataItemEventtrace & item)
{
    if (item.type() == ET_PACKET_INJECT || item.type() == ET_PACKET_ACCEPT ||
        item.type() == ET_FLIT_RECEIVE || item.type() == ET_FLIT_SEND ||
        item.type() == ET_FLIT_SWITCH || 
        item.type() == ET_APP_REQUEST || item.type() == ET_APP_ENTER ||
        item.type() == ET_APP_EXIT || item.type() == ET_PE_CHANGE)
    {
        m_total_event = m_total_event + 1;
        mp_eventout->addNextItem(item);
    }
}

void ReduceAnalyser::finish()
{
    mp_eventout->setTotal(m_total_event);
    mp_eventout->writeTotal();
    mp_eventout->close();
}
