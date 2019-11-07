/*
 * File name : esynet_statistics.h
 * Function : Define statistics unit.
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
 * @ingroup ESYNET_STATISTICS
 * @file esynet_statistics.h
 * @brief Define statistics unit.
 */

#ifndef ESYNET_STATISTICS_H
#define ESYNET_STATISTICS_H

#include "esynet_router_power.h"
#include "esynet_global.h"
#include <iostream>

/**
 * @ingroup ESYNET_STATISTICS
 * @brief Structure for statistics variable for a router.
 */
class EsynetRouterStatistic
{
protected:

public:
    /**
     * @name Constructor function.
     * @{
     */
    /**
     * @brief Create an empty structure.
     */
    EsynetRouterStatistic()
    {
    }
    /**
     * @brief Create a new structure by copying from exist structure.
     * @param t  Structure to copy.
     */
    EsynetRouterStatistic( const EsynetRouterStatistic & t )
    {
    }
    /**
     * @}
     */

    /**
     * @name Overloading operators.
     * @{
     */
    /**
     * @brief Overloading operator =.
     * @param a  Structure to copy.
     */
    EsynetRouterStatistic& operator=( const EsynetRouterStatistic& a )
    {
        return *this;
    }
    /**
     * @brief Overloading operator +.
     * @param a  Structure to add.
     */
    EsynetRouterStatistic& operator+( const EsynetRouterStatistic& a )
    {
        return *this;
    }
    /**
     * @}
     */

    /**
     * @brief Print statistics to output stream
     * @param os reference to output stream
     * @param a constant reference to NI statistics unit.
     */
    friend std::ostream& operator<<( std::ostream& os, const EsynetRouterStatistic& a )
    {
        return os;
    }
};

/**
 * @ingroup ESYNET_STATISTICS
 * @brief  Structure for statistics variable for a network interface.
 */
class EsynetNIStatistic
{
protected:
    long m_injected_packet;     /*!< @brief Counter of injected packets. */
    double m_inject_start_time; /*!< @brief Time of first injected packet. */
    double m_inject_stop_time;  /*!< @brief Time of last injected packet. */

    long m_accepted_packet;     /*!< @brief Counter of accepted packets. */
    double m_accept_start_time; /*!< @brief Time of first accepted packet. */
    double m_accept_stop_time;  /*!< @brief Time of last accepted packet. */

    double m_total_delay;   /*!< @brief Delay of accepted packet. */
    double m_max_delay;     /*!< @brief Max delay of accepted packet. */

public:
    /**
     * @name Constructor function.
     * @{
     */
    /**
     * @brief Create an empty structure.
     */
    EsynetNIStatistic()
        : m_injected_packet( 0 )
        , m_inject_start_time( -1 )
        , m_inject_stop_time( -1 )
        , m_accepted_packet( 0 )
        , m_accept_start_time( -1 )
        , m_accept_stop_time( -1 )
        , m_total_delay( 0.0 )
        , m_max_delay( 0.0 )
    {
    }
    /**
     * @brief Create a new structure by copying from exist structure.
     * @param t  structure to copy.
     */
    EsynetNIStatistic( const EsynetNIStatistic & t )
        : m_injected_packet( t.injectedPacket() )
        , m_inject_start_time( t.injectStartTime() )
        , m_inject_stop_time( t.injectStopTime() )
        , m_accepted_packet( t.acceptedPacket() )
        , m_accept_start_time( t.acceptStartTime() )
        , m_accept_stop_time( t.acceptStopTime() )
        , m_total_delay( t.totalDelay() )
        , m_max_delay( t.maxDelay() )
    {
    }
    /**
     * @}
     */

    /**
     * @name Functions to access variables.
     * @{
     */
    /**
     * @brief Return the number of injected packets.
     */
    inline long injectedPacket() const { return m_injected_packet; }
    /**
     * @brief Return time of first injected packet.
     */
    inline long injectStartTime() const { return m_inject_start_time; }
    /**
     * @brief Return time of last injected packet.
     */
    inline long injectStopTime() const { return m_inject_stop_time; }
    /**
     * @brief Return the number of accepted packets.
     */
    inline long acceptedPacket() const { return m_accepted_packet; }
    /**
     * @brief Return time of first accepted packet.
     */
    inline double acceptStartTime() const { return m_accept_start_time; }
    /**
     * @brief Return time of last accepted packets.
     */
    inline double acceptStopTime() const { return m_accept_stop_time; }
    /**
     * @brief Return the sum of the delay of accepted packets.
     */
    inline double totalDelay() const { return m_total_delay; }
    /**
     * @brief Return maximum delay of accepted packets.
     */
    inline double maxDelay() const { return m_max_delay; }
    /**
     * @}
     */

    /**
     * @name Functions to update variables
     * @{
     */
    /**
     * @brief Increase one injected packet.
     * @param time Time when the packet is injected.
     */
    inline void incInjectPacket( double time ) 
    {
        // Increase injected packet.
        m_injected_packet ++; 

        // Update time of the first and the last injected packet.
        if (m_inject_start_time < 0.0)
        {
            m_inject_start_time = time;
        }
        m_inject_stop_time = time;
    }
    /**
     * @brief Increase one injected packet.
     * @param time Time when the packet is accepted.
     * @param delay Packet delay from source NI to destination NI.
     */
    inline void incAcceptPacket( double time, double delay )
    {
        // Increase accepted packet.
        m_accepted_packet ++;

        // Update the time of the first and the last accepted packet.
        if ( m_accept_start_time < 0.0 )
        {
            m_accept_start_time = time;
        }
        m_accept_stop_time = time;

        // Increase total packet delay.
        m_total_delay += delay;
        // Update maximum packet delay.
        if (delay > m_max_delay)
        {
            m_max_delay = delay;
        }
    }
    /**
     * @}
     */

    /**
     * @name Overloading operators.
     * @{
     */
    /**
     * @brief Overloading operator =.
     * @param a  Structure to copy.
     */
    EsynetNIStatistic& operator=( const EsynetNIStatistic& a )
    {
        m_injected_packet = a.injectedPacket();
        m_inject_start_time = a.injectStartTime();
        m_inject_stop_time = a.injectStopTime();

        m_accepted_packet = a.acceptedPacket();
        m_accept_start_time = a.acceptStartTime();
        m_accept_stop_time = a.acceptStopTime();

        m_total_delay = a.totalDelay();
        m_max_delay = a.maxDelay();

        return *this;
    }
    /**
     * @brief Overloading operator +.
     * 
     * If variables mean the maximum/minimum/first/last, choose the larger/smaller/earlier/later among two structures.
     *
     * @param a  Structure to add.
     */
    EsynetNIStatistic& operator+( const EsynetNIStatistic& a )
    {
        m_injected_packet += a.injectedPacket();
        if ( a.injectStartTime() < m_inject_start_time && a.injectStartTime() > 0 )
        {
            m_inject_start_time = a.injectStartTime();
        }
        if ( a.injectStopTime() > m_inject_stop_time )
        {
            m_inject_stop_time = a.injectStopTime();
        }

        m_accepted_packet += a.acceptedPacket();
        if ( a.acceptStartTime() < m_accept_start_time && a.acceptStartTime() > 0 )
        {
            m_accept_start_time = a.acceptStartTime();
        }
        if ( a.acceptStopTime() > m_accept_stop_time )
        {
            m_accept_stop_time = a.acceptStopTime();
        }

        m_total_delay += a.totalDelay();
        if ( a.maxDelay() > m_max_delay )
        {
            m_max_delay = a.maxDelay();
        }

        return (*this);
    }
    /**
     * @}
     */

    /**
     * @name Calculate variables
     * @{
     */
    /**
     * @brief Return average latency.
     */
    double latency() const
    {
        if ( m_accepted_packet > 0 )
        {
            return m_total_delay / m_accepted_packet;
        }
        else
        {
            return 0.0;
        }
    }
    /**
     * @brief Return packet injected rate.
     */
    double packetInjectRate() const
    {
        if ( m_inject_stop_time - m_inject_start_time > 0 )
        {
            return (double)m_injected_packet / (double)( m_inject_stop_time - m_inject_start_time );
        }
        else
        {
            return 0.0;
        }
    }
    /**
     * @brief Return average throughput.
     */
    double throughput() const
    {
        if ( m_accept_stop_time - m_accept_start_time > 0 )
        {
            return (double)m_accepted_packet / (double)( m_accept_stop_time - m_accept_start_time );
        }
        else
        {
            return 0.0;
        }
    }
    /**
     * @}
     */
    /**
     * @brief Print statistics to output stream
     * @param os reference to output stream
     * @param a constant reference to NI statistics unit.
     */
    friend std::ostream& operator<<( std::ostream& os, const EsynetNIStatistic& a )
    {
        os.precision(6);
        os << "packet injected:       " << a.injectedPacket() << std::endl;
        os << "packet injection rate: " << a.packetInjectRate() << std::endl;
        os << "total finished:        " << a.acceptedPacket() << std::endl;
        os << "average delay:         " << a.latency() << std::endl;
        os << "throughput:            " << a.throughput() << std::endl;

        return os;
    }
};

/**
 * @ingroup ESYNET_STATISTICS
 * @brief  Structure for statistics variable for a foundation platform.
 */
class EsynetFoundationStatistic
    : public EsynetRouterStatistic
    , public EsynetNIStatistic
{
protected:
    long m_accept_mark_packet; /*!< @brief Counter of accepted packets with mark for latency measurement. */
    double m_total_mark_delay; /*!< @brief Sum of delay of accepted packets with mark for latency measurement. */

    long m_throughput_measure_start_packet; /*!< @brief Counter of injected packet before throughput measurement start. */
    long m_throughput_measure_stop_packet;  /*!< @brief Counter of injected packet before throughput measurement finished. */
    double m_throughput_measure_start_time; /*!< @brief Time of throughput measurement start. */
    double m_throughput_measure_stop_time;  /*!< @brief Time of throughput measurement finished. */

    double m_total_mem_power;       /*!< @brief Memory energy. */
    double m_total_crossbar_power;  /*!< @brief Crossbar energy. */
    double m_total_arbiter_power;   /*!< @brief Arbiter energy. */
    double m_total_link_power;      /*!< @brief Link energy. */

public:
    /**
     * @name Constructor function.
     * @{
     */
    /**
     * @brief Create an empty structure.
     */
    EsynetFoundationStatistic()
        : EsynetRouterStatistic()
        , EsynetNIStatistic()
        , m_accept_mark_packet( 0 )
        , m_total_mark_delay( 0.0 )
        , m_throughput_measure_start_packet( 0 )
        , m_throughput_measure_stop_packet( 0 )
        , m_throughput_measure_start_time( 0.0 )
        , m_throughput_measure_stop_time( 0.0 )
        , m_total_mem_power( 0.0 )
        , m_total_crossbar_power( 0.0 )
        , m_total_arbiter_power( 0.0 )
        , m_total_link_power( 0.0 )
    {
    }
    /**
     * @brief Create a new structure by copying from exist structure.
     * @param t  Structure to copy.
     */
    EsynetFoundationStatistic( const EsynetFoundationStatistic & t )
        : EsynetRouterStatistic( t )
        , EsynetNIStatistic( t )
        , m_accept_mark_packet( t.acceptMarkPacket() )
        , m_total_mark_delay( t.totalMarkDelay() )
        , m_throughput_measure_start_packet( t.throughputMeasureStartPacket() )
        , m_throughput_measure_stop_packet( t.throughputMeasureStopPacket() )
        , m_throughput_measure_start_time( t.throughputMeasureStartTime() )
        , m_throughput_measure_stop_time( t.throughputMeasureStopTime() )
        , m_total_mem_power( t.m_total_mem_power )
        , m_total_crossbar_power( t.m_total_crossbar_power )
        , m_total_arbiter_power( t.m_total_arbiter_power )
        , m_total_link_power( t.m_total_link_power )
    {
    }
    /**
     * @}
     */

    /**
     * @name Functions to access variables.
     * @{
     */
    /**
     * @brief Return the number of accepted packet with mark for latency measurement.
     */
    long acceptMarkPacket() const { return m_accept_mark_packet; }
    /**
     * @brief Return the sum of delay of accped packet with mark for latency measurement.
     */
    double totalMarkDelay() const { return m_total_mark_delay; }
    /**
     * @brief Return the number of packet before throughput measurement starts.
     */
    long throughputMeasureStartPacket() const { return m_throughput_measure_start_packet; }
    /**
     * @brief Return the number of packet when throughput measurement stops.
     */
    long throughputMeasureStopPacket() const { return m_throughput_measure_stop_packet; }
    /**
     * @brief Return the time when throughput measurement starts.
     */
    double throughputMeasureStartTime() const { return m_throughput_measure_start_time; }
    /**
     * @brief Return the time when throughput measurement stops.
     */
    double throughputMeasureStopTime() const { return m_throughput_measure_stop_time; }
    /**
     * @brief Return total power.
     */
    double totalPower( double time ) const
    {
        return ( m_total_mem_power + m_total_crossbar_power + m_total_arbiter_power + m_total_link_power ) / time * POWER_NOM_;
    }
    /**
     * @brief Return buffer power.
     */
    double memPower( double time ) const { return m_total_mem_power / time * POWER_NOM_; }
    /**
     * @brief Return crossbar power.
     */
    double crossbarPower( double time ) const { return m_total_crossbar_power / time * POWER_NOM_; }
    /**
     * @brief Return arbiter power.
     */
    double arbiterPower( double time ) const { return m_total_arbiter_power / time * POWER_NOM_; }
    /**
     * @brief Return link power.
     */
    double linkPower( double time ) const { return m_total_link_power / time * POWER_NOM_; }
    /**
     * @}
     */

    /**
     * @name Functions to update variables
     * @{
     */
    /**
     * @brief Increase accepted packet for latency measurement.
     * @param delay Packet delay.
     */
    void incAcceptMarkPacket( double delay )
    {
        m_accept_mark_packet ++;
        m_total_mark_delay += delay;
    }
    /**
     * @brief Set the packet counter and time when throughput measurement start.
     * @param time Simulation time.
     * @param pac Packet counter.
     */
    void setThroughputMeasureStart( double time, long pac )
    {
        m_throughput_measure_start_time = time;
        m_throughput_measure_start_packet = pac;
    }
    /**
     * @brief Set the packet counter and time when throughput measurement start.
     * @param time Simulation time.
     * @param pac Packet counter.
     */
    void setThroughputMeasureStop( double time, long pac )
    {
        m_throughput_measure_stop_time = time;
        m_throughput_measure_stop_packet = pac;
    }
    /**
     * @brief Set energy consumption of one router.
     */
    void addRouterPower(  unsigned long long curr_time, int router_num, EsynetRouterPower& power )
    {
        m_total_mem_power += power.powerBufferReport( curr_time, router_num );
        m_total_crossbar_power += power.powerCrossbarReport( curr_time, router_num );
        m_total_arbiter_power += power.powerArbiterReport( curr_time, router_num );
        m_total_link_power += power.powerLinkReport( curr_time, router_num );
    }
    /**
     * @}
     */

    /**
     * @name Overloading operators.
     * @{
     */
    /**
     * @brief Overloading operator =.
     * @param a  Structure to copy.
     */
    EsynetFoundationStatistic& operator=( const EsynetFoundationStatistic& a )
    {
        EsynetRouterStatistic::operator=( static_cast< EsynetRouterStatistic >( a ) );
        EsynetNIStatistic::operator=( static_cast< EsynetNIStatistic >( a ) );
        m_accept_mark_packet = a.m_accept_mark_packet;
        m_total_mark_delay = a.m_total_mark_delay;
        m_throughput_measure_start_packet = a.m_throughput_measure_start_packet;
        m_throughput_measure_stop_packet = a.m_throughput_measure_stop_packet;
        m_throughput_measure_start_time = a.m_throughput_measure_start_time;
        m_throughput_measure_stop_time = a.m_throughput_measure_stop_time;

        return *this;
    }
    /**
     * @brief Overloading operator +.
     * @param a  Structure to add.
     */
    EsynetFoundationStatistic& operator+( const EsynetRouterStatistic& a )
    {
        EsynetRouterStatistic::operator+( a );

        return (*this);
    }
    /**
     * @brief Overloading operator +.
     * @param a  Structure to add.
     */
    EsynetFoundationStatistic& operator+( const EsynetNIStatistic& a )
    {
        EsynetNIStatistic::operator+( a );

        return (*this);
    }
    /**
     * @}
     */

    /**
     * @name Calculate variables
     * @{
     */
    /**
     * @brief Return latency measurement.
     */
    double latencyMeasure() const
    {
        if ( m_accept_mark_packet > 0 )
        {
            return m_total_mark_delay / m_accept_mark_packet;
        }
        else
        {
            return 0.0;
        }
    }
    /**
     * @brief Return throughput measurement.
     */
    double throughputMeasure() const
    {
        double throughput_measure_cycle = m_throughput_measure_stop_time - m_throughput_measure_start_time;
        long throughput_measure_packet = m_throughput_measure_stop_packet - m_throughput_measure_start_packet;
        if ( throughput_measure_cycle > 0 )
        {
            return throughput_measure_packet / throughput_measure_cycle;
        }
        else
        {
            return 0.0;
        }
    }
    /**
     * @}
     */
    /**
     * @brief Print statistics to output stream
     * @param os reference to output stream
     * @param time current simulation time
     * @param latency True if print latency measurement
     * @param throughput True if print throughput measurement
     */
    void printStatistics( std::ostream& os, double time, bool latency = false, bool throughput = false )
    {
        os.precision( 6 );
        os << "==== General information ====" << std::endl;
        os << static_cast< EsynetNIStatistic >( *this );

        os << "==== Power Consumption ====" << std::endl;
        os << "total mem power:      " << memPower( time ) << std::endl;
        os << "total crossbar power: " << crossbarPower( time ) << std::endl;
        os << "total arbiter power:  " << arbiterPower( time ) << std::endl;
        os << "total link power:     " << linkPower( time ) << std::endl;
        os << "total power:          " << totalPower( time ) << std::endl;

        if ( latency )
        {
            os << "==== Latency Measurement ====" << std::endl;
            os << "delay:   " << m_total_mark_delay << std::endl;
            os << "packet:  " << m_accept_mark_packet << std::endl;
            os << "latency: " << latencyMeasure() << std::endl;
        }
        if ( throughput )
        {
            os << "==== Throughput Measurement ====" << std::endl;
            os << "packet:     " << m_throughput_measure_stop_packet - m_throughput_measure_start_packet << std::endl;
            os << "cycle:      " << m_throughput_measure_stop_time - m_throughput_measure_start_time  << std::endl;
            os << "throughput: " << throughputMeasure() << std::endl;
        }
    }
};

#endif
