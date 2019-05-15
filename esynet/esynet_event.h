/*
 * File name : esynet_event.h
 * Function : Define event structure.
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
 * @file esynet_event.h
 * @brief Declare ESYNet event structure.
 */

#ifndef ESYNET_MESS_EVENT_H
#define ESYNET_MESS_EVENT_H

#include "esy_eventtype.h"
#include "esynet_flit.h"

#include <vector>
#include <iostream>

/**
 * @ingroup ESYNET_SIM_ENGINE
 * @brief Event structure.
 */
class EsynetEvent
{
public:
    /**
     * @brief Type of event. 
     */
    enum EventType {
        EVG = 0,    /*!< @brief Packet generation event. */
        ROUTER = 1, /*!< @brief Router pipeline event. */
        WIRE = 2,   /*!< @brief Link transmission event. */
        CREDIT = 3, /*!< @brief Credit transmission event. */
        NIREAD = 4, /*!< @brief NI read event. */
    };

private:
    double m_event_time;    /*!< @brief Event time. */
    double m_pipe_time;     /*!< @brief Pipeline time for different clock-domain. */
    long m_event_type;       /*!< @brief Message type. */
    long m_src_id;      /*!< @brief Source router id. */
    long m_src_pc;      /*!< @brief Source physical channel. */
    long m_src_vc;      /*!< @brief Source virtual channel. */
    long m_des_id;      /*!< @brief Destination router id. */
    long m_des_pc;      /*!< @brief Destination physical channel. */
    long m_des_vc;      /*!< @brief Destination virtual channel. */
    EsynetFlit m_flit;      /*!< @brief Flit struture. */

public:
    /**
     * @name Construction function.
     * @{
     */
    /**
     * @brief Construct an empty instance.
     */
    EsynetEvent()
        : m_event_time( 0.0 )
        , m_pipe_time( 0.0 )
        , m_event_type ( EVG )
        , m_src_id( -1 )
        , m_src_pc( -1 )
        , m_src_vc( -1 )
        , m_des_id( -1 )
        , m_des_pc( -1 )
        , m_des_vc( -1 )
        , m_flit()
    {}
    /**
     * @brief Construct an instance with all field specified.
     * @param time event time.
     * @param type event type.
     * @param srcid Source router id.
     * @param srcpc Source physical channel.
     * @param srcvc Source virtual channel.
     * @param desid Destination router id.
     * @param despc Destination physical channel.
     * @param desvc Destination virtual channel.
     * @param flit Constant reference to flit structure.
     */
    EsynetEvent( double time, long type, long srcid, long srcpc, long srcvc, long desid, long despc, long desvc, const EsynetFlit & flit )
        : m_event_time( time )
        , m_pipe_time( 0.0 )
        , m_event_type ( type )
        , m_src_id( srcid )
        , m_src_pc( srcpc )
        , m_src_vc( srcvc )
        , m_des_id( desid )
        , m_des_pc( despc )
        , m_des_vc( desvc )
        , m_flit( flit )
    {}
    /**
     * @brief Construct an instance by copying from exist instance.
     * @param t Instance to copy.
     */
    EsynetEvent( const EsynetEvent& t )
        : m_event_time( t.eventStart() )
        , m_pipe_time( t.pipeTime() )
        , m_event_type ( t.type() )
        , m_src_id( t.srcId() )
        , m_src_pc( t.srcPc() )
        , m_src_vc( t.srcVc() )
        , m_des_id( t.desId() )
        , m_des_pc( t.desPc() )
        , m_des_vc( t.desVc() )
        , m_flit( t.flit() )
    {}
    /**
     * @}
     */

    /**
     * @brief Copy event structure.
     * @param t Instance to copy.
     */
    EsynetEvent& operator=( const EsynetEvent& t )
    {
        m_event_time = t.eventStart();
        m_pipe_time = t.pipeTime();
        m_event_type = t.eventType();
        m_src_id = t.srcId();
        m_src_pc = t.srcPc();
        m_src_vc = t.srcVc();
        m_des_id = t.desId();
        m_des_pc = t.desPc();
        m_des_vc = t.desVc();
        m_flit = t.flit();

        return (*this);
    }

    /**
     * @name Functions to access variables.
     * @{
     */
    /**
     * @brief Return event time.
     */
    inline double eventStart() const { return m_event_time; }
    /**
     * @brief Return pipeline time.
     */
    inline double pipeTime() const { return m_pipe_time; }
    /**
     * @brief Return event type.
     */
    inline EventType eventType() const { return (EventType)m_event_type; }
    /**
     * @brief Return event type in integar number.
     */
    inline long type() const { return m_event_type; }
    /** 
     * @brief Return source router id.
     */
    inline long srcId() const { return m_src_id; }
    /** 
     * @brief Return source physical channel.
     */
    inline long srcPc() const { return m_src_pc; }
    /** 
     * @brief Return source virtual channel.
     */
    inline long srcVc() const { return m_src_vc; }
    /** 
     * @brief Return destination router id.
     */
    inline long desId() const { return m_des_id; }
    /** 
     * @brief Return destination physical channel.
     */
    inline long desPc() const { return m_des_pc; }
    /** 
     * @brief Return destination virtual channel.
     */
    inline long desVc() const { return m_des_vc; }
    /** 
     * @brief Return constant reference to flit structure.
     */
    inline const EsynetFlit & flit() const { return m_flit; }
    /**
     * @}
     */

    /**
     * @name Functions to create event structure.
     * @{
     */
    /**
     * @brief Return an packet generation event with specified value.
     * @param time Event time.
     * @param src Source address of new packet.
     * @param des Destination address of new packet.
     * @param pac_size Size of new packet.
     * @param msgno Message number of new packet.
     * @param flag Flags of new packet. Default is 0.
     */
    static EsynetEvent generateEvgEvent( double time, long src, long des, long pac_size, long msgno, long flag = 0 )
    {
        EsynetEvent e;
        e.m_event_time = time;
        e.m_event_type = EVG;
        e.m_src_id = src;
        e.m_des_id = des;
        e.m_flit = EsynetFlit(msgno, pac_size, EsynetFlit::FLIT_HEAD, src, des, time, esynet::EsynetPayload(), flag );
        return e;
    }
    /**
     * @brief Return a router pipeline event with specified value.
     * @param time Event time.
     * @param pipe Pipeline cycle.
     * @param id The target of this router pipeline event. -1 means all routers.
     */
    static EsynetEvent generateRouterEvent( double time, double pipe, long id = -1 )
    {
        EsynetEvent e;
        e.m_event_time = time;
        e.m_pipe_time = pipe;
        e.m_event_type = ROUTER;
        e.m_src_id = id;
        return e;
    }
    /**
     * @brief Return a credit transmission event with specified value.
     * @param time Event time.
     * @param src Source router id.
     * @param src_pc Source physical channel.
     * @param src_vc Source virtual channel.
     * @param des Destination router id.
     * @param des_pc Destination physical channel.
     * @param des_vc Destination virtual channel.
     * @param credit Credit value.
     */
    static EsynetEvent generateCreditEvent( double time, long src, long src_pc, long src_vc, long des, long des_pc, long des_vc, long credit )
    {
        EsynetEvent e;
        e.m_event_time = time;
        e.m_event_type = CREDIT;
        e.m_src_id = src;
        e.m_src_pc = src_pc;
        e.m_src_vc = src_vc;
        e.m_des_id = des;
        e.m_des_pc = des_pc;
        e.m_des_vc = des_vc;
        e.m_flit = EsynetFlit(credit, 0, EsynetFlit::FLIT_HEAD, 0, 0, 0, esynet::EsynetPayload(), 0 );
        return e;
    }
    /**
     * @brief Return a link transmission event with specified value.
     * @param time Event time.
     * @param src Source router id.
     * @param src_pc Source physical channel.
     * @param src_vc Source virtual channel.
     * @param des Destination router id.
     * @param des_pc Destination physical channel.
     * @param des_vc Destination virtual channel.
     * @param flit Constant reference to flit structure.
     */
    static EsynetEvent generateWireEvent( double time, long src, long src_pc, long src_vc, long des, long des_pc, long des_vc, const EsynetFlit & flit )
    {
        EsynetEvent e;
        e.m_event_time = time;
        e.m_event_type = WIRE;
        e.m_src_id = src;
        e.m_src_pc = src_pc;
        e.m_src_vc = src_vc;
        e.m_des_id = des;
        e.m_des_pc = des_pc;
        e.m_des_vc = des_vc;
        e.m_flit = flit;
        return e;
    }
    /**
     * @brief Return a link transmission event with specified value.
     * @param time Event time.
     * @param ni The target NI id.
     */
    static EsynetEvent generateNIReadEvent( double time, long ni )
    {
        EsynetEvent e;
        e.m_event_time = time;
        e.m_event_type = NIREAD;
        e.m_src_id = ni;
        return e;
    }

    /**
     * @}
     */

    /**
     * @brief Print event structure to output stream.
     * @param os Reference to output stream.
     * @param e Event structure to print.
     */
    friend std::ostream & operator<<( std::ostream& os, const EsynetEvent & e )
    {
        os << "time=" << e.eventStart() << " ";
        os << "type=" << (int)e.eventType() << " ";
        os << "src={Router=" << e.srcId() << ",Port=" << e.srcPc() << ",VC=" << e.srcVc() << "} ";
        os << "des={Router=" << e.desId() << ",Port=" << e.desPc() << ",VC=" << e.desVc() << "} ";
        os << " " << e.flit();
        return os;
    }
};
#endif
