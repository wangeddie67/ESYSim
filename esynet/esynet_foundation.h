/*
 * File name : esynet_foundation.h
 * Function : Define network platform.
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
 * @file esynet_foundation.h
 * @brief Declare network platform
 */

#ifndef ESYNET_FOUNDATION_H
#define ESYNET_FOUNDATION_H

#include "esy_linktool.h"
#include "esynet_ni_unit.h"
#include "esynet_router_unit.h"
#include "esynet_packet_generator.h"

/**
 * @brief Simulation platform.
 *
 * The duty of simulation platform includes:
 * - generate the routers and PEs for simulation.
 * - control simulation progress.
 * - response to simulation events.
 * - synchronize the signal between routers and PEs.
 * - statistic simulation results.
 */
class EsynetFoundation : public EsynetSimBaseUnit
{
public:
    /**
     * @brief State of measurement.
     */
    enum MeasureState
    {
        MEASURE_INIT,  /*!< Measurement has not start. */
        MEASURE_PRE,   /*!< Prepare phase for measurement, e.g. latency */
        MEASURE_ING,   /*!< Measurement is running. */
        MEASURE_END    /*!< Measurement has finished. */
    };

protected:
    EsyNetworkCfg * m_network_cfg;  /*!< @brief Pointer to network configuration structure. */
    EsynetConfig * m_argument_cfg;  /*!< @brief Pointer to argument configuration structure. */

    std::vector< EsynetRouter > m_router_list;  /*!< @brief Vector of router component. */
    std::vector< EsynetNI > m_ni_list;  /*!< @brief Vector of process element componenet. */

    EsynetPacketGenerator m_packet_generator; /*!< @brief packet generator */
    EsynetFoundationStatistic m_statistic;  /*!< @brief Global statistic unit. */
    MeasureState m_latency_measure_state;  /*!< @brief State of measurement of average latency. */
    MeasureState m_throughput_measure_state;  /*!< @brief State of measurement of throughput. */
    MeasureState m_injection_state; /*!< @brief State of injection and acception if the number of packet is limited. */

public:
    /**
     * @brief Create a simulation based on network configuration and argument
     * configuration.
     *
     * - Create and configure simulation components, including routers and PEs.
     * - Connect routers and PEs according to network topology.
     * - Initializate the period events for the frequency scaled routers.
     * - Configure fault injection units in each router.
     *
     * @param network_cfg  pointer to network configuration structure.
     * @param argument_cfg pointer to argument configuration structure.
     */
    EsynetFoundation(EsyNetworkCfg * network_cfg, EsynetConfig * argument_cfg);
    
    /** @name Functions to access variables */
    ///@{
    /**
     * @brief Access state of latency measurement #m_latency_measure_state.
     * @return stat of latency measurement #m_latency_measure_state.
     */
    MeasureState latencyMeasureState() const { return m_latency_measure_state; }
    /**
     * @brief Access state of throughput measurement #m_throughput_measure_state.
     * @return stat of throughput measurement #m_throughput_measure_state.
     */
    MeasureState throughputMeasureState() const { return m_throughput_measure_state; }
    /**
     * @brief Access state of throughput measurement # m_injection_state*.
     * @return stat of throughput measurement #m_injection_state.
     */
    MeasureState limitedInjectionState() const { return m_injection_state; }
    /**
     * @brief Access the specified router.
     * @param id  id of specified router.
     * @return  reference to this router entity.
     */
    EsynetRouter & router(long id) { return m_router_list[id]; }
    
    vector< EsynetEvent > acceptList();
    ///@}

    /**
     * @brief Collect the statistic variables and control the simulation.
     *
     * This function implements three functions:
     * - If the number of injected packets is larger than user-specified number
     * of injected router (by option -injected_packet), stop PE generating
     * packets.
     * - If the number of injected packets is larger than user-specified number
     * of warm up packets, start measurement by setting the status of latency
     * measurement #m_latency_measure_state to #MEASURE_ING; If user specifies
     * the number of packets for latency measurement, stop marking packets by
     * setting the status of latency measurement #m_latency_measure_state to
     * #MEASURE_END when enough packets have marked for measurement.
     * - If the number of accepted packets is larger than user-specified number
     * of warm up packets, start measurement by setting the status of throughput
     * measurement #m_throughput_measure_state to #MEASURE_ING; If user
     * specifies the number of packets for throughput measurement, stop
     * measurement by setting the status of throughput measurement
     * #m_throughput_measure_state to #MEASURE_END when enough packets have
     * recorded for measurement. Record the time of start and end of this
     * measurement.
     */
    void updateStatistic();
    
    /**
     * @brief Synchronize the signals between routers and PEs.
     *
     * This function is used to share informations between routers and PEs.
     * Until now only one function has implemented:
     * - Propagate the fault information of one router to a 3x3 area with the
     * faulty router in the center. This is function is used by TARRA.
     */
    void informationPropagate();
    
    /** Functions to handle simulation event */
    ///@{
    /**
     * @brief Message injection event handler.
     *
     * - Fill in the field of packet id and flit flag.
     * - Generate a new packet and inject the packet into PE.
     *
     * @param mesg  including the packet to inject.
     * \sa #eventHandler()
     */
    void receiveEvgMessage(const EsynetEvent & mesg);
    /**
     * @brief Router period event handler.
     *
     * - insert next router period event into message queue.
     * - run router pipeline.
     * - collect the statistic variable.
     * - propagate information between routers and PEs.
     *
     * @param mesg  including the router and the period.
     * \sa #eventHandler()
     */
    void receiveRouterMessage(const EsynetEvent & mesg);
    /**
     * @brief Receive flit event handler.
     *
     * - injected the flit into the input buffer of router.
     * - clear the flag of flit on link.
     *
     * @param mesg  including the flit to receive.
     * \sa #eventHandler()
     */
    void receiveWireMessage(const EsynetEvent & mesg);
    /**
     * @brief Receive credit event handler.
     * @param mesg  including the credit to receive.
     * \sa #eventHandler()
     */
    void receiveCreditMessage(const EsynetEvent & mesg);
    /**
     * @brief Receive NI read event handler.
     * @param mesg  including the ni read to receive.
     * \sa #eventHandler()
     */
    void receiveNiReadMessage(const EsynetEvent & mesg);
    ///@}

    /**
     * @brief Collect simulation results and print them out through standard
     * error and link tool interface.
     */
    void simulationResults();

    /**
     * @brief Overwrite the handler to simulation event.
     *
     * - set the simulation period of routers and PEs.
     * - handle the simulation event by calling corresponding functions.
     * - collect new generated events.
     *
     * @param time  current simulation time.
     * @param mess  message to handle.
     * \sa #receiveEvgMessage(), #receiveRouterMessage(),
     * #receiveWireMessage(), #receiveCreditMessage()
     */
    void eventHandler(double time, const EsynetEvent & mess);

public:
    /**
     * @brief print configuration of routers to ostream os
     * @param os  output stream.
     * @param sf  the simulation to print.
     * @return  output stream.
     */
    friend ostream& operator<<(ostream& os, const EsynetFoundation& sf);
};

#endif
