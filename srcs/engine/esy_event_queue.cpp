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

#include "esy_event_queue.h"

bool operator<( const EsyEventPtr & a, const EsyEventPtr & b )
{
    return a->eventTime() < b->eventTime();
}

EsyEventQueue::EsyEventQueue( double start_time )
    : m_curr_time( start_time )
    , m_event_counter( 0 )
{
}

void EsyEventQueue::simulator( uint64_t sim_cycle )
{
    // loop while there is any packets in queue
    while( size() > 0 )
    {
        // get the first message and its time
        EsyEventPtr curr_event = *begin();
        m_curr_time = curr_event->eventTime();
        // current time is later than sim_cycle, end simulator step
        if( m_curr_time > sim_cycle )
        {
            return;
        }

        // remove first message from queue
        erase( begin() );

        std::map< uint16_t, EsyEventFun >::iterator iter =
            m_event_fun_map.find( curr_event->eventType() );
 
        if( iter != m_event_fun_map.end() )
        {
            std::cout << "[ERROR] Unknown event type "
                << curr_event->eventType() << "." << std::endl;
        }
        else
        {
            EsyEventFun fun = iter->second;
            fun( this, m_curr_time, curr_event->eventPayload() );
        }
    }
}

