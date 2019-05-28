/*
 * File name : esynet_event_queue.h
 * Function : Define event queue.
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

#ifndef ESYNET_MESS_QUEUE_H
#define ESYNET_MESS_QUEUE_H

#include "esy_interdata.h"
#include "esynet_foundation.h"
#include "esynet_event.h"

#include <set>

/* return if message $a$ is before message $b$ */
bool operator<(const EsynetEvent & a, const EsynetEvent & b);

/**
 * @ingroup ESYNET_SIM_ENGINE
 * @brief Event queue.
 */
class EsynetEventQueue : public multiset< EsynetEvent >
{
private:
    EsynetConfig * mp_argument_cfg;    /*!< @brief Pointer to option list. */
    double m_current_time;  /*!< @brief Simulation time. */
    long m_mess_counter;    /*!< @brief Number of message injected. */

    EsynetSimBaseUnit * mp_sim_platform;    /*!< @brief Pointer to simulation platform. */

    EsyDataFileOStream< EsyDataItemEventtrace > * mp_eventtrace;    /*!< @brief Pointer to output stream interface for event trace. */
    EsyDataFileOStream< EsyDataItemBenchmark > * mp_outputtrace;    /*!< @brief Pointer to output stream interface for benchmark trance. */

public:
    /**
     * @brief Construction function.
     * @param start_time Start simulation time.
     * @param sim_platform Pointer to simlation platform.
     * @param argument_cfg Pointer to option list.
     */
    EsynetEventQueue( double start_time, EsynetSimBaseUnit * sim_platform, EsynetConfig * argument_cfg );
    /**
     * @brief Destruction function.
     */
    ~EsynetEventQueue();

    /**
     * @brief Insert new event into the event queue.
     * @param e Event to insert.
     */
    inline void insertEvent( const EsynetEvent & e ) { m_mess_counter++; insert(e); }

    /**
     * @brief Handle all event before specified simulation time.
     * @param sim_cycle Simulation time.
     */
    void simulator( long long int sim_cycle );

private:
    /**
     * @name Function to control output stream interface for event trace.
     * @{
     */
    /**
     * @brief Create and Open the output stream interface for event trace.
     */
    bool openEventTrace();
    /**
     * @brief Flush the output stream interface for event trace. Then, close and destruct the output stream interface.
     */
    bool closeEventTrace();
    /**
     * @}
     */

    /**
     * @name Function to control output stream interface for benchmark trace.
     * @{
     */
    /**
     * @brief Create and Open the output stream interface for benchmark trace.
     */
    bool openOutputTrace();
    /**
     * @brief Flush the output stream interface for benchmark trace. Then, close and destruct the output stream interface.
     */
    bool closeOutputTrace();
    /**
     * @}
     */
};

#endif
