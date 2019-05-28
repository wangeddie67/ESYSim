/*
 * File name : esynet_sim_base.h
 * Function : Define base class for ESYNet simulation components.
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
 * @file esynet_sim_base.h 
 * @brief Declare base class for ESYNet simulation components.
 */

#ifndef ESYNET_SIM_BASE_H
#define ESYNET_SIM_BASE_H

#include "esynet_event.h"

#include <vector>

/**
 * @brief Base class for simulation components.
 * @ingroup ESYNET_SIM_ENGINE
 */
class EsynetSimBaseUnit
{
protected:
    double m_current_time;  /*!< @brief Simulation time. */
    vector< EsynetEvent > m_event_queue;    /*!< @brief New event generated during event handler. */

public:
    /**
     * @brief Construction function.
     */
    EsynetSimBaseUnit()
        : m_current_time( 0.0 )
        , m_event_queue()
    {}

    /**
     * @name Functions to operate event queue.
     * @{
     */
    /**
     * @brief Return constant reference of the event queue.
     */
    inline const vector< EsynetEvent > & eventQueue() const { return m_event_queue; }
    /**
     * @brief Clear the event queue.
     */
    inline void clearEventQueue() { m_event_queue.clear(); }
    /**
     * @brief Add events into the event queue.
     * @param event constant reference to the event.
     */
    inline void addEvent(const EsynetEvent & event) { m_event_queue.push_back( event ); }
    /**
     * @}
     */

    /**
     * @name Function to set simulationt time.
     * @{
     */
    /**
     * @brief Set simulation time.
     * @param time simulation time.
     */
    inline void setTime( double time ) { m_current_time = time; }
    /**
     * @}
     */

    /**
     * @brief Function to handle events. It should be override in each simulation components.
     * @param time Simulation time
     * @param e event to handle.
     */
    virtual void eventHandler(double time, const EsynetEvent & e)
    {
        m_current_time = time;
        return;
    }
};

#endif // BASICSIMCOMPONENT_H
