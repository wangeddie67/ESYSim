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
    long m_src;         /*!< @brief Source address. */
    long m_des;         /*!< @brief Destination address. */
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
        , m_src( 0 )
        , m_des( 0 )
        , m_flag( 0 )
        , m_flit_start_time( 0 )
        , m_payload()
    {}
    /**
     * @brief Construct a flit with specified field.
     * @param id Unique id.
     * @param size Packet size.
     * @param type Flit type.
     * @param src Source address.
     * @param des Destination address.
     * @param st Start time.
     * @param data Payload data.
     * @param flag Flit flags.
     */
    EsynetFlit( long id, long size, FlitType type, long src, long des, double st, const esynet::EsynetPayload & data, long flag = 0 )
        : m_id( id )
        , m_pac_size( size )
        , m_type( type )
        , m_src( src )
        , m_des( des )
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
        , m_src( a.m_src )
        , m_des( a.m_des )
        , m_flag( a.m_flag )
        , m_flit_start_time( a.m_flit_start_time )
        , m_payload( a.m_payload )
    {}
    /**
     * @}
     */

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
     * @brief Return source router id.
     */
    inline long srcAddr() const { return m_src; }
    /**
     * @brief Return destination router id.
     */
    inline long desAddr() const { return m_des; }
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
    void setFlitFlag( long flag ) { m_flag = flag; }
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
        os << "addr=" << ft.srcAddr() << ">" << ft.desAddr() << " ";
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


