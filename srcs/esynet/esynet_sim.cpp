/*
 * File name : esynet_foundation.cc
 * Function : Implement network platform.
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
 * @ingroup ESYNET_NETWORK
 * @file esynet_foundation.cc
 * @brief Implement network platform
 */

#include "esynet_sim.h"
#include "esynet_event.h"

Esynet::Esynet( const EsyNetCfg & network_cfg,
                EsyRandGen* random_gen,
                EsyPacketGenerator* packet_gen,
                EsyEventQueue* event_queue
              )
    : mp_random_gen( random_gen )
    , mp_packet_gen( packet_gen )
{
    // register events
    event_queue->registerEvent( EVENT_CYCLE,
                                esynetEventFunClock );
    event_queue->registerEvent( EVENT_PAC_INJECT,
                                esynetEventFunPacketInject );

    // Append first cycle event.
    EsynetEventClock* clock_event = new EsynetEventClock( this );
    EsyEventPtr cycle_event( new EsynetEvent(
        event_queue->currTime(), EVENT_CYCLE, clock_event ) );
    event_queue->insertEvent( cycle_event );
}

void Esynet::simulationResults()
{
}

/*
void Esynet::eventHandler(double time, const EsynetEvent& mess)
{
    EsynetSimBaseUnit::eventHandler(time, mess);
    for ( size_t i = 0; i < m_router_list.size(); i ++ )
    {
        m_router_list[ i ].setTime( time );
    }
    for ( size_t i = 0; i < m_ni_list.size(); i ++ )
    {
        m_ni_list[ i ].setTime( time );
    }

    switch( mess.eventType() )
    {
    case EsynetEvent::EVG:    receiveEvgMessage(mess); break;
    case EsynetEvent::ROUTER: receiveRouterMessage(mess); break;
    case EsynetEvent::WIRE:   receiveWireMessage(mess); break;
    case EsynetEvent::CREDIT: receiveCreditMessage(mess); break;
    case EsynetEvent::NIREAD: receiveNiReadMessage(mess); break;
    default: cout << "This message type " << mess.eventType() << " is not supported.\n"; break;
    }

    for ( size_t i = 0; i < m_router_list.size(); i ++ )
    {
        const std::vector< EsynetEvent > & mess_list = m_router_list[i].eventQueue();
        for ( size_t j = 0; j < mess_list.size(); j ++ )
        {
            addEvent( mess_list[ j ] );
        }
        m_router_list[ i ].clearEventQueue();
    }
    for ( size_t i = 0; i < m_ni_list.size(); i ++ )
    {
        const std::vector< EsynetEvent > & mess_list = m_ni_list[i].eventQueue();
        for ( size_t j = 0; j < mess_list.size(); j ++ )
        {
            addEvent( mess_list[ j ] );
        }
        m_ni_list[ i ].clearEventQueue();
    }
}
*/

void Esynet::updateStatistic()
{
/*    if ( m_argument_cfg.injectedPacket() >= 0 && m_statistic.acceptedPacket() >= m_argument_cfg.injectedPacket() )
    {
        m_injection_state = MEASURE_END;
    }

    switch ( m_latency_measure_state )
    {
    case MEASURE_INIT:
        if ( m_statistic.injectedPacket() >= m_argument_cfg.warmUpPacket() )
        {
            m_latency_measure_state = MEASURE_PRE;
        }
        break;
    case MEASURE_PRE:
        if ( ( m_argument_cfg.latencyMeasurePacket() >= 0 ) &&
             ( m_statistic.injectedPacket() >= m_argument_cfg.warmUpPacket() +
               m_argument_cfg.latencyMeasurePacket() ) )
        {
            m_latency_measure_state = MEASURE_ING;
        }
    case MEASURE_ING:
        if ( ( m_argument_cfg.latencyMeasurePacket() >= 0 ) &&
            ( m_statistic.acceptMarkPacket() >= m_argument_cfg.latencyMeasurePacket() ) )
        {
            m_latency_measure_state = MEASURE_END;
        }
        break;
    case MEASURE_END:
        break;
    }

    switch ( m_throughput_measure_state )
    {
    case MEASURE_INIT:
        if ( m_statistic.acceptedPacket() >= m_argument_cfg.warmUpPacket() )
        {
            m_throughput_measure_state = MEASURE_ING;
            m_statistic.setThroughputMeasureStart( m_current_time, m_statistic.acceptedPacket() );
        }
        break;
    case MEASURE_PRE:
        break;
    case MEASURE_ING:
        m_statistic.setThroughputMeasureStop( m_current_time, m_statistic.acceptedPacket() );
        if ( m_argument_cfg.throughputMeasurePacket() >= 0 &&
            m_statistic.acceptedPacket() >= m_argument_cfg.warmUpPacket() + m_argument_cfg.throughputMeasurePacket() )
        {
            m_throughput_measure_state = MEASURE_END;
        }
        break;
    case MEASURE_END:
        break;
    }*/
}

void Esynet::informationPropagate()
{
    // Add Code
}

void
Esynet::handleClockEvent( EsyEventQueue* queue )
{
    // next clock event
    EsynetEventClock* clock_event = new EsynetEventClock( this );
    queue->insertEvent( EsyEventPtr(
        new EsynetEvent( queue->currTime() + 1, EVENT_CYCLE, clock_event ) ) );

    std::vector< EsyNetworkPacketPtr > pac_list =
        mp_packet_gen->generatePacket( queue->currTime() );

    for ( std::size_t i = 0; i < pac_list.size(); i ++ )
    {
        EsyNetworkPacketPtr pac = pac_list[ i ];

        EsynetEventPacketInject* pac_event = new EsynetEventPacketInject(
            this, pac->srcNi(), pac->dstNi(), pac->length() );

        queue->insertEvent( EsyEventPtr(
            new EsynetEvent( pac->time(), EVENT_PAC_INJECT, pac_event )  ) );
    }
}

void
Esynet::handlePacketInjectEvent( EsyEventQueue* queue,
                                 uint32_t src_ni,
                                 uint32_t dst_ni,
                                 uint32_t length
)
{
    std::cout << queue->currTime() << " " << src_ni << " " << dst_ni << " " << length << std::endl;
}

