/*
 * File name : esynet_flit.h
 * Function : Define flit structure.
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
 * @file esynet_flit.h
 * @brief Declare flit structure.
 */

#ifndef ESYNET_FLIT_H
#define ESYNET_FLIT_H

#include "esynet_global.h"

#include <vector>
#include <iostream>
#include <stdint.h>

/**
 * @ingroup ESYNET_NETWORK
 * @brief Flit structure.
 */
class EsynetFlit
{
public:
    /**
     * @brief Flit type.
     */
    enum FlitType {
        FLIT_HEAD,  /*!< @brief Head flit */
        FLIT_BODY,  /*!< @brief Body flit */
        FLIT_TAIL   /*!< @brief Tail flit */
    };

    /**
     * @brief Flit flags.
     */
    enum FlitFlag {
        FLIT_MARK = 0x01,   /*!< @brief The flit is marked for latency measurement. */
    };

private:
    long m_id;          /*!< @brief Flit unique id. */
    long m_pac_size;    /*!< @brief Size of packet. */
    FlitType m_type;    /*!< @brief Flit type. */
    long m_src_ni;      /*!< @brief Source NI. */
    long m_src_router;  /*!< @brief Source router. */
    long m_src_port;    /*!< @brief Source port. */
    long m_des_ni;      /*!< @brief Destination NI. */
    long m_des_router;  /*!< @brief Destination router. */
    long m_des_port;    /*!< @brief Destination port. */
    uint16_t m_flag;    /*!< @brief Flit flags. */
    double m_flit_start_time;   /*!< @brief Start time of flit since NI. */

    esynet::EsynetPayload m_payload;   /*!< @brief Payload data. */

public:
    /**
     * @name Construct Function
     * @{
     */
    /**
     * @brief Construct an empty flit.
     */
    EsynetFlit()
        : m_id( 0 )
        , m_pac_size( 1 )
        , m_type( FLIT_HEAD )
        , m_src_ni( 0 )
        , m_src_router( 0 )
        , m_src_port( 0 )
        , m_des_ni( 0 )
        , m_des_router( 0 )
        , m_des_port( 0 )
        , m_flag( 0 )
        , m_flit_start_time( 0 )
        , m_payload()
    {}
    /**
     * @brief Construct a flit with specified field. Specify source and destination NI.
     * @param id Unique id.
     * @param size Packet size.
     * @param type Flit type.
     * @param src Source NI.
     * @param des Destination NI.
     * @param st Start time.
     * @param data Payload data.
     * @param flag Flit flags.
     */
    EsynetFlit( long id, long size, FlitType type, long src, long des
        , double st, const esynet::EsynetPayload & data, long flag = 0
    )
        : m_id( id )
        , m_pac_size( size )
        , m_type( type )
        , m_src_ni( src )
        , m_src_router( 0 )
        , m_src_port( 0 )
        , m_des_ni( des )
        , m_des_router( 0 )
        , m_des_port( 0 )
        , m_flag( flag )
        , m_flit_start_time( st )
        , m_payload ( data )
    {}
    /**
     * @brief Construct a flit with specified field.
     * @param id Unique id.
     * @param size Packet size.
     * @param type Flit type.
     * @param src_ni Source NI.
     * @param src_router Source router.
     * @param src_port Source port.
     * @param des_ni Destination NI.
     * @param des_router Destination router.
     * @param des_port Destination port.
     * @param st Start time.
     * @param data Payload data.
     * @param flag Flit flags.
     */
    EsynetFlit( long id, long size, FlitType type, long src_ni, long src_router, long src_port, long des_ni, long des_router, long des_port
        , double st, const esynet::EsynetPayload & data, long flag = 0
    )
        : m_id( id )
        , m_pac_size( size )
        , m_type( type )
        , m_src_ni( src_ni )
        , m_src_router( src_router )
        , m_src_port( src_port )
        , m_des_ni( des_ni )
        , m_des_router( des_router )
        , m_des_port( des_port )
        , m_flag( flag )
        , m_flit_start_time( st )
        , m_payload ( data )
    {}
    /**
     * @brief Construct a flit by copying
     * @param a Flit to copy.
     */
    EsynetFlit( const EsynetFlit & a )
        : m_id( a.m_id )
        , m_pac_size( a.m_pac_size )
        , m_type( a.m_type )
        , m_src_ni( a.m_src_ni )
        , m_src_router( a.m_src_router )
        , m_src_port( a.m_src_port )
        , m_des_ni( a.m_des_ni )
        , m_des_router( a.m_des_router )
        , m_des_port( a.m_des_port )
        , m_flag( a.m_flag )
        , m_flit_start_time( a.m_flit_start_time )
        , m_payload( a.m_payload )
    {}
    /**
     * @}
     */

    /**
     * @brief Copy flit structure.
     * @param t Instance to copy.
     */
    EsynetFlit& operator=( const EsynetFlit& t )
    {
        m_id = t.m_id;
        m_pac_size = t.m_pac_size;
        m_type = t.m_type;
        m_src_ni = t.m_src_ni;
        m_src_router = t.m_src_router;
        m_src_port = t.m_src_port;
        m_des_ni = t.m_des_ni;
        m_des_router = t.m_des_router;
        m_des_port = t.m_des_port;
        m_flag = t.m_flag;
        m_flit_start_time = t.m_flit_start_time;
        m_payload = t.m_payload;

        return (*this);
    }

    /**
     * @name Function to access variables
     * @{
     */
    /**
     * @brief Return flit unique id.
     */
    inline long flitId() const { return m_id; }
    /**
     * @brief Return packet size.
     */
    inline long flitSize() const { return m_pac_size; }
    /**
     * @brief Return flit type, Head, Body, or Tail.
     */
    inline FlitType flitType() const { return m_type; }
    /**
     * @brief Return source NI.
     */
    inline long srcNi() const { return m_src_ni; }
    /**
     * @brief Return source router id.
     */
    inline long srcRouter() const { return m_src_router; }
    /**
     * @brief Return source port.
     */
    inline long srcPort() const { return m_src_port; }
    /**
     * @brief Return destination NI.
     */
    inline long desNi() const { return m_des_ni; }
    /**
     * @brief Return destination router id.
     */
    inline long desRouter() const { return m_des_ni; }
    /**
     * @brief Return destination port.
     */
    inline long desPort() const { return m_des_port; }
    /**
     * @brief Return flit start time.
     */
    inline double startTime() const { return m_flit_start_time; }
    /**
     * @brief Return payload data.
     */
    inline const esynet::EsynetPayload & data() const { return m_payload; }
    /**
     * @brief Return flit flags.
     */
    inline uint16_t flitFlag() const { return m_flag; }
    /**
     * @brief Return True if the flit is marked for latency measurement.
     */
    inline bool marked() const { return ( m_flag & FLIT_MARK ) == FLIT_MARK; }
    /**
     * @}
     */

    /**
     * @name Function to set variables
     * @{
     */
    /**
     * @brief Set flit flags.
     */
    void setFlitFlag( uint16_t flag ) { m_flag = flag; }
    /**
     * @}
     */

    /**
     * @brief Print flit to output stream.
     * @param os output stream.
     * @param ft flit structure.
     * @return output stream after print.
     */
    friend std::ostream& operator<<( std::ostream& os, const EsynetFlit & ft )
    {
        os << "id=" << ft.flitId() << " size=" << ft.flitSize() << " ";
        switch ( ft.flitType() )
        {
        case EsynetFlit::FLIT_HEAD: os << "FLIT_HEAD "; break;
        case EsynetFlit::FLIT_BODY: os << "FLIT_BODY "; break;
        case EsynetFlit::FLIT_TAIL: os << "FLIT_TAIL "; break;
        }
        os << "addr=" << ft.srcRouter() << "(" << ft.srcPort() << ")" << "-->" << ft.desRouter() << "(" << ft.desPort() << ")" << " ";
        os << "data={";
        for ( size_t i = 0; i < ft.data().size(); i ++ )
        {
            os << ft.data()[ i ] << ";";
        }
        os << "} ";
        os << "start=" << ft.startTime() << " ";

        return os;
    }

};

#endif


