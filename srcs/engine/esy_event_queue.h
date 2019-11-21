/*
 * File name : esy_event_queue.h
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
 * @defgroup ESY_SIM_ENGINE Simulation Engine
 * @brief Event and event queue.
 */

/**
 * @ingroup ESY_SIM_ENGINE
 * @file esy_event_queue.h
 * @brief Declare event and event queue.
 */

#ifndef ENGINE_ESY_EVENT_QUEUE_H_
#define ENGINE_ESY_EVENT_QUEUE_H_

#include <map>
#include <iostream>
#include <memory>

/**
 * @ingroup ESY_SIM_ENGINE
 * @brief Event structure.
 */
class EsyEvent
{
protected:
    uint64_t m_time;        /*!< @brief Event response time. */
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
     * @param payload pointer to event payload.
     */
    EsyEvent( uint64_t time, uint16_t type, void* payload )
        : m_time( time )
        , m_type( type )
        , mp_payload( payload )
    {}
    /**
     * @}
     */

    /**
     * @name Functions to access variables.
     * @{
     */
    /**
     * @brief Return event reponse time.
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

/**
 * @ingroup ESY_SIM_ENGINE
 * @brief Shared pointer of event
 */
typedef std::shared_ptr< EsyEvent > EsyEventPtr;

/**
 * @ingroup ESY_SIM_ENGINE
 * @brief Event queue.
 */
class EsyEventQueue
{
public:
    /**
     * @brief Type of response function
     */
    typedef void (*EsyEventFun)(EsyEventQueue* queue, void* payload);

    static uint16_t SIM_STOP_EVENT; /*!< @brief Cofe of simulation stop event.
        Constant value is 0. */

private:
    uint64_t m_curr_time;       /*!< @brief Simulation time. */
    uint64_t m_event_counter;   /*!< @brief Number of message injected. */
    std::multimap< uint64_t, EsyEventPtr > m_event_map; /*!< @brief Sorted map 
        between event time and event pointer */
    std::map< uint16_t, EsyEventFun > m_event_fun_map;  /*!< @brief Map between
        event type and event function. */

public:
    /**
     * @brief Construction function.
     * @param sim_length Simulaion length is cycle.
     */
    EsyEventQueue( uint64_t sim_length );

    /**
     * @brief Return current simulation time.
     */
    uint64_t currTime() const { return m_curr_time; }

    /**
     * @brief Insert new event into the event queue and increase event counter.
     * @param e Event to insert.
     */
    void insertEvent( const EsyEventPtr & e );

    /**
     * @brief Register event type and response function.
     * @param type Code of event type.
     * @param fun  Pointer to event response function.
     */
    void registerEvent( uint16_t type, EsyEventFun fun )
    {
        m_event_fun_map.insert(
            std::pair< uint16_t, EsyEventFun >( type, fun ) );
    }

    /**
     * @brief Handle all events before specified simulation time.
     * @param sim_cycle Simulation time to pause.
     */
    bool simulate( uint64_t sim_cycle );

    /**
     * @brief Run entire simulation until simulation stop.
     */
    void simAll();
};

#endif
