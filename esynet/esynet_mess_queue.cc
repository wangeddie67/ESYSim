/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#include "esynet_mess_queue.h"

/* initialization value of global pointer to entity of message queue */
//MessQueue * MessQueue::mp_global_pointer = 0;

/*************************************************
  Function : 
    bool operator<(const MessEvent & a, const MessEvent & b)
  Description: 
    return if message $a$ is before than message $b$
  Input:
    MessEvent& a  compare message
    MessEvent& b  compare message
  Return
    bool
*************************************************/
bool operator<(const EsynetMessEvent & a, const EsynetMessEvent & b)
{
    return a.eventStart() < b.eventStart();
}

/*************************************************
  Function : 
    MessQueue::MessQueue( time_type start_time )
  Description : 
    constructs an message queue start at $start_time$.
  Calls :
    void MessQueue::addMessage(const MessEvent &x)
    MessEvent(time_type time, time_type pipe, MessType mtype)
  Input :
    double start_time  start time of queue
*************************************************/
EsynetMessQueue::EsynetMessQueue(
	double start_time, EsynetSimBaseUnit * sim_platform,
	EsynetConfig * argument_cfg
):
    mp_argument_cfg( argument_cfg ),
    m_current_time( start_time ),
    m_mess_counter( 0 ),
    mp_eventtrace( 0 ),
    mp_outputtrace( 0 ),
    mp_sim_platform( sim_platform )
{
    /* add fisrt ROUTER_message */
	addMessage(EsynetMessEvent::generateRouterMessage(0, PIPE_DELAY_,
		EsynetMessEvent::ROUTER));
}

/*************************************************
  Function : 
    void MessQueue::insertMsg( int src, int dest, long long int sim_cycle,
        long packetSize, long long int msgNo, long long addr, long vc )
  Description: 
    insert EVG message with specfic $src$, $dest$, $sim_cycle$, $size$,
    $msgNo$, $addr$ and $vc$. Packet is not larger than 10 cyclcs.
  Calls :
    void MessQueue::addMessage(const MessEvent &x);
  Input :
              int src         source id
              int dest        destination id
    long long int sim_cycle   event time
             long packetSize  packet size
    long long int msgNo       message num er
        long long addr        not used
             long vc          virtual channel in source
*************************************************/
void EsynetMessQueue::insertMsg(int src, int dest, long long int sim_cycle,
    long packetSize, long int msgNo, long long addr, long vc )
{
    /* insert EVG_message */
	addMessage(EsynetMessEvent::generateEvgMessage(m_current_time,
        src, dest, packetSize, msgNo, (double)sim_cycle ) );
}

/*************************************************
  Function : 
    void MessQueue::simulator(long long int sim_cycle)
  Description : 
    handle all event before $sim_cycle$.
  Calls :
    void sim_foundation::receiveCreditMessage(MessEvent mesg)
    void sim_foundation::receiveEvgMessage(MessEvent mesg)
    void sim_foundation::receiveRouterMessage(MessEvent mesg)
    void sim_foundation::receiveWireMessage(MessEvent mesg)
  Called By :
    int main(int argc, char *argv[])
    int esynetRunSim(long long int sim_cycle)
  Input :
    long long int sim_cycle  cycle limitation to handle
*************************************************/
void EsynetMessQueue::simulator(long long int sim_cycle)
{
    /* loop while there is any packets in queue */
    while( size() > 0 )
    {
        /* get the first message and its time */
		EsynetMessEvent current_message = *begin();
        m_current_time = current_message.eventStart();
        /* current time is later than sim_cycle, end simulator step */
        if( m_current_time > sim_cycle )
        {
            return;
        } /* if( m_current_time > sim_cycle ) */
        
        /* remove first message from queue */
        erase( begin() );
        
        /* if message is not esynet event, pass this loop */
        if ( current_message.eventType() >= EVENT_TRACE_START )
        {
            /* print out event if event trace is enabled */
            if ( mp_argument_cfg->eventTraceEnable() && ( mp_eventtrace != 0 ) )
            {
                /* Construct a new event trace item in event trace equal to the
                 * message */
                EsyDataItemEventtrace * t_item = new EsyDataItemEventtrace( 
                    current_message.eventStart(), 
                    current_message.eventType(), 
                    (short)current_message.srcId(), 
                    (char)current_message.srcPc(), 
                    (char)current_message.srcVc(), 
                    (short)current_message.desId(), 
                    (char)current_message.desPc(), 
                    (char)current_message.desVc(),
                    current_message.flit().flitId(), 
                    (short)current_message.flit().flitSize(), 
                    current_message.flit().flitType(),
                    (short)current_message.flit().sorAddr(), 
                    (short)current_message.flit().desAddr(),
                    (char)current_message.flit().flitFlag(),
                    current_message.flit().startTime()
                );
                /* add the item to event trace */
                mp_eventtrace->addNextItem( *t_item );
                delete t_item;
            } /* if ( ConfigurationPointer().eventTraceEnable()  */
            if ( mp_argument_cfg->eventTraceCoutEnable() )
            {
                /* Construct a new event trace item in event trace equal to the
                 * message */
                EsyDataItemEventtrace * t_item = new EsyDataItemEventtrace( 
                    current_message.eventStart(), 
                    current_message.eventType(), 
                    (short)current_message.srcId(), 
                    (char)current_message.srcPc(), 
                    (char)current_message.srcVc(), 
                    (short)current_message.desId(), 
                    (char)current_message.desPc(), 
                    (char)current_message.desVc(),
                    current_message.flit().flitId(), 
                    (short)current_message.flit().flitSize(), 
                    current_message.flit().flitType(),
                    (short)current_message.flit().sorAddr(), 
                    (short)current_message.flit().desAddr(),
                    (char)current_message.flit().flitFlag(),
                    current_message.flit().startTime()
                );
                string itemstr = t_item->writeItem();
                LINK_TRACE_PRINT( itemstr )
                delete t_item;
            } /* if ( ConfigurationPointer().eventTraceEnable()  */
            if ( mp_argument_cfg->outputTraceEnable() && 
				( mp_outputtrace != 0 ) && 
				current_message.eventType() == ET_PACKET_INJECT )
			{
				EsyDataItemBenchmark * t_item = new EsyDataItemBenchmark(
					current_message.eventStart(),
					(short)current_message.srcId(),
					(short)current_message.desId(),
					(short)current_message.flit().flitSize() );
				mp_outputtrace->addNextItem( *t_item );
				delete t_item;
			}
        } /* if ( current_message.eventType() > EVENT_MAX ) */
        
        /* handle esynet event */
        else
        {
            mp_sim_platform->eventHandler( m_current_time, current_message );
			vector< EsynetMessEvent > result_event = mp_sim_platform->eventQueue();
            mp_sim_platform->clearEventQueue();
            
            for ( size_t i = 0; i < result_event.size(); i ++ )
            {
                insert( result_event[ i ] );
            }
        } /* if ( current_message.eventType() <= EVENT_MAX ) */
    } /* while( size() > 0 ) */
}

/*************************************************
  Function : 
    bool MessQueue::open_event_trace()
  Description : 
    open interface to event trace
  Called By :
    int main(int argc, char *argv[]);
  Return : 
    bool  true
*************************************************/
bool EsynetMessQueue::openEventTrace()
{
    /* construct new entity of interface to event trace */
    mp_eventtrace = new EsyDataFileOStream< EsyDataItemEventtrace >(
        mp_argument_cfg->eventTraceBufferSize(), 
        mp_argument_cfg->eventTraceFileName(), 
		EVENTTRACE_EXTENSION, true, mp_argument_cfg->eventTraceTextEnable() );
    /* return true */
    return true;
}

/*************************************************
  Function : 
    bool MessQueue::close_event_trace()
  Description : 
    close interface to event trace
  Called By :
    int main(int argc, char *argv[]);
  Return : 
    bool  true
*************************************************/
bool EsynetMessQueue::closeEventTrace()
{
    /* flush buffer */
    mp_eventtrace->flushBuffer();
	mp_eventtrace->setTotal( m_current_time );
	mp_eventtrace->writeTotal();
    /* delete entity of ODataFileStream */
    delete mp_eventtrace;
    /* return true */
    return true;
}

bool EsynetMessQueue::openOutputTrace()
{
    /* construct new entity of interface to event trace */
    mp_outputtrace = new EsyDataFileOStream< EsyDataItemBenchmark >(
        mp_argument_cfg->outputTraceBufferSize(), 
        mp_argument_cfg->outputTraceFileName(), 
        BENCHMARK_EXTENSION, true, mp_argument_cfg->outputTraceTextEnable() );
    /* return true */
    return true;
}

bool EsynetMessQueue::closeOutputTrace()
{
    /* flush buffer */
    mp_outputtrace->flushBuffer();
	mp_outputtrace->setTotal( m_current_time );
	mp_outputtrace->writeTotal();
    /* delete entity of ODataFileStream */
    delete mp_outputtrace;
    /* return true */
    return true;
}

/*************************************************
  Function : 
    bool linkEmptyCheck( long id, long port = -1 ) const
  Description : 
    check if there is any packets to router $id$. 
    if there is no packet, return True
  Called By :
    void ProcessingElement::bistRouterFSM()
  Return : 
    bool  if there is no packet, return True
*************************************************/
bool EsynetMessQueue::linkEmptyCheck(long id, long port) const
{
    /* loop of packet */
    for ( const_iterator l_event = begin(); l_event != end(); l_event ++ )
    {
        /* event type is WIRE */
		if ((*l_event).eventType() == EsynetMessEvent::WIRE)
        {
            /* if there is WIRE event to router $id$, return false */
            if ( port == -1 )
            {
                if ( ( *l_event ).desId() == id)
                {
                    return false;
                }
            }
            else
            {
                if ( ( *l_event ).desId() == id && ( *l_event ).desPc() == port )
                {
                    return false;
                }
            }
        } /* if ( at( l_event ).eventType() == WIRE ) */
    } /* for ( int l_event = 0; l_event < size(); l_event ++ ) */
    return true;
}
