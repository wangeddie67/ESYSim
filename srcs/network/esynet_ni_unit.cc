/*
 * File name : esynet_ni_unit.cc
 * Function : Implement NI module.
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
 * @ingroup ESYNET_NI
 * @file esynet_ni_unit.cc
 * @brief Implement NI module.
 */

#include "esynet_ni_unit.h"
#include "esynet_random_unit.h"

EsynetNI::EsynetNI( const EsyNetCfg & network_cfg, long id, long router, long port )
    : EsynetSimBaseUnit()
    , m_id( id )
    , m_router( router )
    , m_port( port )
    , m_network_cfg( network_cfg )
    , m_ni_cfg( network_cfg.ni( id ) )
    , m_inject_vc( 0 )
    , m_vc_counter( network_cfg.router( router ).port( port ).inputVcNumber(), 
                    network_cfg.router( router ).port( port ).inputBuffer() )
    , m_init_data( ATOM_WIDTH_ / 64, 0 )
    , m_flit_size( ATOM_WIDTH_ / 64 )
    , m_flit_on_link( false )
{
    // initalization data
    m_init_data.resize( m_flit_size );
    for ( long l_byte = 0; l_byte < m_flit_size; l_byte ++ )
    {
        m_init_data[ l_byte ] = EsynetSRGenFlatUnsignedLongLong(0, MAX_64_);
    }
}

void EsynetNI::receiveFlit( long vc, const EsynetFlit & flit )
{
    EsynetFlit flit_t = flit;

    if ( m_ni_cfg.interruptDelay() > 0 )
    {
        m_eject_queue.push_back( flit_t );
        addEvent( EsynetEvent::generateCreditEvent( m_current_time + CREDIT_DELAY_
            , m_id + m_network_cfg.routerCount(), 0, 0, m_router, m_port, vc, m_ni_cfg.bufferSize() - m_eject_queue.size() ) );

        // Raise interrupt when receive the last flit of NI.
        if ( flit.flitType() == EsynetFlit::FLIT_TAIL || flit.flitSize() == 1 )
        {
            addEvent( EsynetEvent::generateNIReadEvent( m_current_time + m_ni_cfg.interruptDelay(), m_id ) );
        }
    }
    else
    {
        addEvent( EsynetEvent::generateCreditEvent( m_current_time + CREDIT_DELAY_
            , m_id + m_network_cfg.routerCount(), 0, 0, m_router, m_port, vc, 10 ) );
        receivePacket( flit_t );
    }
}

void EsynetNI::receiveNiInterrupt()
{
    while ( m_eject_queue.size() > 0 )
    {
        EsynetFlit flit_t = m_eject_queue[ 0 ];
        receivePacket( flit_t );
        m_eject_queue.erase( m_eject_queue.begin() );

        // Receive flit until the end of the packet.
        if ( flit_t.flitType() == EsynetFlit::FLIT_TAIL || flit_t.flitSize() == 1 )
        {
            break;
        }
    }
    addEvent( EsynetEvent::generateCreditEvent( m_current_time + CREDIT_DELAY_
        , m_id + m_network_cfg.routerCount(), 0, 0, m_router, m_port, 0, m_ni_cfg.bufferSize() - m_eject_queue.size() ) );
}

void EsynetNI::receivePacket( const EsynetFlit & flit )
{
    if ( flit.desNi() != m_id )
    {
        return;
    }

    if ( flit.flitType() == EsynetFlit::FLIT_TAIL || flit.flitSize() == 1 )
    {
        m_statistic.incAcceptPacket( m_current_time, m_current_time - flit.startTime() );
    }

    if ( flit.flitType() == EsynetFlit::FLIT_HEAD )
    {
        // if the packet only contain one flit, return directly */
        if ( flit.flitSize() == 1 )
        {
            // Event trace event
            addEvent( EsynetEvent( m_current_time, ET_PACKET_ACCEPT, flit.srcNi(), -1, -1, flit.desNi(), -1, -1, flit ) );
            m_accept_list.push_back( EsynetEvent( m_current_time, ET_PACKET_ACCEPT, flit.srcNi(), -1, -1, flit.desNi(), -1, -1, flit ) );
        }
    }
    else if ( flit.flitType() == EsynetFlit::FLIT_BODY )
    {
        // if the packet only contain one flit, return directly */
    }
    else if ( flit.flitType() == EsynetFlit::FLIT_TAIL )
    {
        // Event trace event
        addEvent( EsynetEvent( m_current_time, ET_PACKET_ACCEPT, flit.srcNi(), -1, -1, flit.desNi(), -1, -1, flit ) );
        m_accept_list.push_back( EsynetEvent( m_current_time, ET_PACKET_ACCEPT, flit.srcNi(), -1, -1, flit.desNi(), -1, -1, flit ) );
    }
}

void EsynetNI::niSimPipeline()
{
    // If the injection queue is not empty, send out one flit.
    if( m_inject_queue.size() > 0 )
    {
        long vs = 0;

        if ( m_vc_counter[ vs ] > 0 )
        {
            EsynetFlit flit_t = m_inject_queue[ 0 ];

            m_inject_queue.erase( m_inject_queue.begin() );

            addEvent( EsynetEvent::generateWireEvent( m_current_time + WIRE_DELAY_
                , m_id + m_network_cfg.routerCount(), 0, 0, m_router, m_port, vs, flit_t) );
            m_vc_counter[ vs ] --;

            m_flit_on_link = true;
        }
    }

}

void EsynetNI::injectPacket( const EsynetFlit& t_flit )
{
    // inject flits
    long pac_size = t_flit.flitSize();

    // loop all flits in packet
    for ( long l = 0; l < pac_size; l ++ )
    {
        esynet::EsynetPayload flit_data;
        // generate flit data
        for( long i = 0; i < m_flit_size; i ++ )
        {
            // set data, the next data is the previous data * 0.8 +a reandom number
            m_init_data[ i ] = static_cast<esynet::EsynetAtomType>( m_init_data[i] * CORR_EFF_ + EsynetSRGenFlatUnsignedLongLong(0, MAX_64_) );
            flit_data.push_back( m_init_data[ i ] );
        }

        // insert head flit
        if ( l == 0 )
        {
            m_inject_queue.push_back(
                EsynetFlit( t_flit.flitId(), t_flit.flitSize(), EsynetFlit::FLIT_HEAD
                    , t_flit.srcNi(), t_flit.srcRouter(), t_flit.srcPort(), t_flit.desNi(), t_flit.desRouter(), t_flit.desPort()
                    , t_flit.startTime(), flit_data, t_flit.flitFlag() ) );
        }
        // Insert tail flit.
        else if( l == ( pac_size - 1) )
        {
            m_inject_queue.push_back(
                EsynetFlit( t_flit.flitId(), t_flit.flitSize(), EsynetFlit::FLIT_TAIL
                    , t_flit.srcNi(), t_flit.srcRouter(), t_flit.srcPort(), t_flit.desNi(), t_flit.desRouter(), t_flit.desPort()
                    , t_flit.startTime(), flit_data, t_flit.flitFlag() ) );
        }
        // Insert body flit.
        else 
        {
            m_inject_queue.push_back(
                EsynetFlit( t_flit.flitId(), t_flit.flitSize(), EsynetFlit::FLIT_BODY
                    , t_flit.srcNi(), t_flit.srcRouter(), t_flit.srcPort(), t_flit.desNi(), t_flit.desRouter(), t_flit.desPort()
                    , t_flit.startTime(), flit_data, t_flit.flitFlag() ) );
        }
    }

    // Event trace event
    addEvent( EsynetEvent( m_current_time, ET_PACKET_INJECT, t_flit.srcNi(), -1, -1, t_flit.desNi(), -1, -1, t_flit ) );

    // Increase statistics
    m_statistic.incInjectPacket( m_current_time );
}


bool EsynetNI::isEmpty()
{
    if ( m_flit_on_link )
    {
        return false;
    }

    if ( m_inject_queue.size() == 0 )
    {
        return true;
    }
    else
    {
        if ( m_inject_queue[ 0 ].flitType() == EsynetFlit::FLIT_HEAD )
        {
            return true;
        }
    }
    return false;
}

