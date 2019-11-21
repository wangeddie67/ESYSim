/*
 * File name : esy_event_queue.h
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
 * @ingroup ESY_SIM_ENGINE
 * @file esy_event_queue.cpp
 * @brief Implement event queue.
 */

#include "esy_event_queue.h"

uint16_t EsyEventQueue::SIM_STOP_EVENT = 0;

EsyEventQueue::EsyEventQueue( uint64_t sim_length )
    : m_curr_time( 0 )
    , m_event_counter( 0 )
{
    // Append stop event.
    EsyEventPtr stop_event( new EsyEvent( sim_length, SIM_STOP_EVENT, NULL ) );
    insertEvent( stop_event );
}

void
EsyEventQueue::insertEvent(const EsyEventPtr& e)
{
    m_event_counter++;
    m_event_map.insert(
        std::pair< uint64_t, EsyEventPtr >( e->eventTime(), e ) );
}


bool
EsyEventQueue::simulate( uint64_t sim_cycle )
{
    // Loop all packets in the queue
    while( m_event_map.size() > 0 )
    {
        // Get the first message and its time
        EsyEventPtr curr_event = m_event_map.begin()->second;
        m_curr_time = curr_event->eventTime();

        // If current time is later than sim_cycle, end simulation step
        if( m_curr_time > sim_cycle )
        {
            return true;
        }

        // Remove first message from queue.
        m_event_map.erase( m_event_map.begin() );

        // If meeting simulation stop event, quit simulation.
        if ( curr_event->eventType() == SIM_STOP_EVENT )
        {
            return false;
        }

        // Get response function to this event.
        std::map< uint16_t, EsyEventFun >::iterator iter =
            m_event_fun_map.find( curr_event->eventType() );

        // If there is unkown event type, print error information.
        if( iter == m_event_fun_map.end() )
        {
            std::cout << "[ERROR] Unknown event type "
                << curr_event->eventType() << "." << std::endl;
        }
        // Call response function
        else
        {
            EsyEventFun fun = iter->second;
            fun( this, curr_event->eventPayload() );
        }
    }

    // Return true so that simulation can continue
    return true;
}

void
EsyEventQueue::simAll()
{
    uint64_t sim_cycle = m_curr_time;
    bool end_flag = true;

    // Loop simulation step until the simulation stop event
    while ( end_flag )
    {
        // Simulation step
        sim_cycle += 1;
        end_flag = simulate( sim_cycle );
    }
}

