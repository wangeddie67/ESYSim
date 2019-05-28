/*
 * File name : esynet_event_queue.h
 * Function : Implement event queue.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>
 */

/**
 * @ingroup ESYNET_SIM_ENGINE
 * @file esynet_event_queue.h
 * @brief Implement ESYNet event queue.
 */

#include "esynet_event_queue.h"

bool operator<( const EsynetEvent & a, const EsynetEvent & b )
{
    return a.eventStart() < b.eventStart();
}

EsynetEventQueue::EsynetEventQueue( double start_time, EsynetSimBaseUnit * sim_platform, EsynetConfig * argument_cfg )
    : mp_argument_cfg( argument_cfg )
    , m_current_time( start_time )
    , m_mess_counter( 0 )
    , mp_sim_platform( sim_platform )
    , mp_eventtrace( 0 )
    , mp_outputtrace( 0 )
{
    // add fisrt ROUTER_message
    m_mess_counter ++;
    insert( EsynetEvent::generateRouterEvent( 0, PIPE_DELAY_, EsynetEvent::ROUTER ) );

    // open event trace
    if ( mp_argument_cfg->eventTraceEnable() )
    {
        openEventTrace();
    }
    // open output trace
    if ( mp_argument_cfg->outputTraceEnable() )
    {
        openOutputTrace();
    }
}

EsynetEventQueue::~EsynetEventQueue()
{
    // close event trace
    if ( mp_argument_cfg->eventTraceEnable() )
    {
        closeEventTrace();
    }
    // close output trace
    if ( mp_argument_cfg->outputTraceEnable() )
    {
        closeOutputTrace();
    }
}

void EsynetEventQueue::simulator( long long int sim_cycle )
{
    // loop while there is any packets in queue
    while( size() > 0 )
    {
        // get the first message and its time
        EsynetEvent current_message = *begin();
        m_current_time = current_message.eventStart();
        // current time is later than sim_cycle, end simulator step
        if( m_current_time > sim_cycle )
        {
            return;
        }

        // remove first message from queue
        erase( begin() );

        // if message is not esynet event, pass this loop
        if ( current_message.type() >= EVENT_TRACE_START )
        {
            // print out event if event trace is enabled
            if ( mp_argument_cfg->eventTraceEnable() && ( mp_eventtrace != 0 ) )
            {
                // Construct a new event trace item in event trace equal to the message
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
                    (short)current_message.flit().srcRouter(), 
                    (short)current_message.flit().desRouter(),
                    (char)current_message.flit().flitFlag(),
                    current_message.flit().startTime()
                );
                // add the item to event trace
                mp_eventtrace->addNextItem( *t_item );
                delete t_item;
            }
            if ( mp_argument_cfg->eventTraceCoutEnable() )
            {
                // Construct a new event trace item in event trace equal to the message
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
                    (short)current_message.flit().srcRouter(), 
                    (short)current_message.flit().desRouter(),
                    (char)current_message.flit().flitFlag(),
                    current_message.flit().startTime()
                );
                string itemstr = t_item->writeItem();
                LINK_TRACE_PRINT( itemstr )
                delete t_item;
            }
            if ( mp_argument_cfg->outputTraceEnable() && ( mp_outputtrace != 0 ) && ( current_message.type() == ET_PACKET_INJECT ) )
            {
                EsyDataItemBenchmark * t_item = new EsyDataItemBenchmark(
                    current_message.eventStart(),
                    (short)current_message.srcId(),
                    (short)current_message.desId(),
                    (short)current_message.flit().flitSize() );
                mp_outputtrace->addNextItem( *t_item );
                delete t_item;
            }
        }
        else
        {
            mp_sim_platform->eventHandler( m_current_time, current_message );
            for ( size_t i = 0; i < mp_sim_platform->eventQueue().size(); i ++ )
            {
                insert( mp_sim_platform->eventQueue()[ i ] );
            }
            mp_sim_platform->clearEventQueue();
        }
    }
}

bool EsynetEventQueue::openEventTrace()
{
    // construct new entity of interface to event trace.
    mp_eventtrace = new EsyDataFileOStream< EsyDataItemEventtrace >(
        mp_argument_cfg->eventTraceBufferSize(), mp_argument_cfg->eventTraceFileName(),
        EVENTTRACE_EXTENSION, true, mp_argument_cfg->eventTraceTextEnable() );
    return true;
}

bool EsynetEventQueue::closeEventTrace()
{
    // flush buffer.
    mp_eventtrace->flushBuffer();
    mp_eventtrace->setTotal( m_current_time );
    mp_eventtrace->writeTotal();

    delete mp_eventtrace;
    return true;
}

bool EsynetEventQueue::openOutputTrace()
{
    // construct new entity of interface to event trace.
    mp_outputtrace = new EsyDataFileOStream< EsyDataItemBenchmark >(
        mp_argument_cfg->outputTraceBufferSize(), mp_argument_cfg->outputTraceFileName(),
        BENCHMARK_EXTENSION, true, mp_argument_cfg->outputTraceTextEnable() );
    return true;
}

bool EsynetEventQueue::closeOutputTrace()
{
    // flush buffer.
    mp_outputtrace->flushBuffer();
    mp_outputtrace->setTotal( m_current_time );
    mp_outputtrace->writeTotal();

    delete mp_outputtrace;
    return true;
}
