/*
 * File name : esynet_foundation.h
 * Function : Declare network platform.
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

#ifndef ESYNET_ESYNET_SIM_H_
#define ESYNET_ESYNET_SIM_H_

#include "../engine/esy_event_queue.h"
#include "../randgen/esy_random.h"
#include "../syscfg/esy_netcfg.h"
#include "../trafficgen/esy_packetgenerator.h"

/**
 * @ingroup ESYNET_NETWORK
 * @brief Simulation platform.
 */
class Esynet
{
public:
    /**
     * @brief State- of measurement.
     */
    enum MeasureState
    {
        MEASURE_INIT,  /*!< Measurement has not start. */
        MEASURE_PRE,   /*!< Prepare phase for measurement, e.g. latency */
        MEASURE_ING,   /*!< Measurement is running. */
        MEASURE_END    /*!< Measurement has finished. */
    };

    enum EventType
    {
        EVENT_CYCLE = 1,        /*!< Clock event. */
        EVENT_PAC_INJECT = 2,   /*!< Packet Injection. */
    };

protected:
    EsyRandGen* mp_random_gen;
    EsyPacketGenerator* mp_packet_gen;

    //std::vector< EsynetRouter > m_router_list;  /*!< @brief Vector of router component. */
    //std::vector< EsynetNI > m_ni_list;  /*!< @brief Vector of process element componenet. */

    //EsynetPacketGenerator m_packet_generator; /*!< @brief packet generator */
    //EsynetFoundationStatistic m_statistic;  /*!< @brief Global statistic unit. */
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
     * @param network_cfg  reference to network configuration structure.
     * @param argument_cfg reference to argument configuration structure.
     */
    Esynet( const EsyNetCfg & network_cfg,
            EsyRandGen* random_gen,
            EsyPacketGenerator* packet_gen,
            EsyEventQueue* event_queue
          );


    /**
     * @name Functions to access variables
     * @{
     */
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
     * @}
     */

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

    /**
     * @brief Collect simulation results and print them out through standard
     * error and link tool interface.
     */
    void simulationResults();

    void handleClockEvent( EsyEventQueue* queue );

    void handlePacketInjectEvent( EsyEventQueue* queue,
                                  uint32_t src_ni,
                                  uint32_t dst_ni,
                                  uint32_t length );
};

#endif
