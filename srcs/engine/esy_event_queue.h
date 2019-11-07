/*
 * File name : esynet_event_queue.h
 * Function : Declare event queue.
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
 * @brief Declare ESYNet event queue.
 */

#ifndef ENGINE_ESY_EVENT_QUEUE_H_
#define ENGINE_ESY_EVENT_QUEUE_H_

#include <set>
#include <map>
#include <iostream>
#include <memory>

/**
 * @ingroup ESYNET_SIM_ENGINE
 * @brief Event structure.
 */
class EsyEvent
{
public:
    uint64_t m_time;        /*!< @brief Event time. */
    uint16_t m_type;        /*!< @brief Event type. */
    void*    mp_payload;    /*!< @brief Event payload. */

public:
    /**
     * @name Construction function.
     * @{
     */
    /**
     * @brief Construct an empty instance.
     */
    EsyEvent()
        : m_time( 0.0 )
        , m_type( 0 )
        , mp_payload( NULL )
    {}
    /**
     * @brief Construct an instance with all field specified.
     * @param time event time.
     * @param type event type.
     * @param payload event payload.
     */
    EsyEvent( uint64_t time, uint16_t type, void* payload )
        : m_time( time )
        , m_type( type )
        , mp_payload( payload )
    {}
    /**
     * @brief Construct an instance by copying from exist instance.
     * @param t Instance to copy.
     */
    EsyEvent( const EsyEvent& t )
        : m_time( t.m_time )
        , m_type( t.m_type )
        , mp_payload( t.mp_payload )
    {}
    /**
     * @}
     */

    /**
     * @name Functions to access variables.
     * @{
     */
    /**
     * @brief Return event time.
     */
    uint64_t eventTime() const { return m_time; }
    /**
     * @brief Return event type.
     */
    uint16_t eventType() const { return m_type; }
    /**
     * @brief Return pointer to payload.
     */
    void* eventPayload() const { return mp_payload; }
    /**
     * @}
     */
};

typedef std::shared_ptr< EsyEvent > EsyEventPtr;

/* return if message $a$ is before message $b$ */
bool operator<(const EsyEventPtr & a, const EsyEventPtr & b);

/**
 * @ingroup ESYNET_SIM_ENGINE
 * @brief Event queue.
 */
class EsyEventQueue : public std::multiset< EsyEventPtr >
{
public:
    typedef void (*EsyEventFun)(EsyEventQueue* queue,
                                uint64_t time,
                                void* payload);

private:
    uint64_t m_curr_time;       /*!< @brief Simulation time. */
    uint64_t m_event_counter;   /*!< @brief Number of message injected. */
    std::map< uint16_t, EsyEventFun > m_event_fun_map;  /*!< @brief Map between
        event type and event function. */

public:
    /**
     * @brief Construction function.
     * @param start_time Start simulation time.
     * @param sim_platform Pointer to simlation platform.
     * @param argument_cfg Pointer to option list.
     */
    EsyEventQueue( double start_time );
    /**
     * @brief Destruction function.
     */
    ~EsyEventQueue();

    /**
     * @brief Insert new event into the event queue.
     * @param e Event to insert.
     */
    void insertEvent( const EsyEventPtr & e ) { m_event_counter++; insert(e); }

    void registerEvent( uint16_t type, EsyEventFun fun )
    {
        m_event_fun_map.insert(
            std::pair< uint16_t, EsyEventFun >( type, fun ) );
    }

    /**
     * @brief Handle all event before specified simulation time.
     * @param sim_cycle Simulation time.
     */
    void simulator( uint64_t sim_cycle );

};

#endif
