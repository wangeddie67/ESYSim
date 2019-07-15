/*
 * File name : esynet_foundation.cc
 * Function : Implement network platform.
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
 * @file esynet_foundation.cc
 * @brief Implement network platform
 */

#include "esynet_foundation.h"

EsynetFoundation::EsynetFoundation( EsyNetworkCfg * network_cfg, EsynetConfig * argument_cfg )
    : m_network_cfg( network_cfg )
    , m_argument_cfg( argument_cfg )
    , m_packet_generator( network_cfg, argument_cfg )
    , m_statistic()
    , m_latency_measure_state( MEASURE_INIT )
    , m_throughput_measure_state( MEASURE_INIT )
    , m_injection_state( MEASURE_INIT )
{
    long router_num = m_network_cfg->routerCount();

    for ( int router_id = 0, ni_id = 0; router_id < router_num; router_id ++ )
    {
        EsyNetworkCfgRouter router_cfg = m_network_cfg->router( router_id );
        m_router_list.push_back( EsynetRouter( m_network_cfg, router_id, m_argument_cfg ) );
        bool has_ni = false;
        for ( int l_port = 0; l_port < router_cfg.portNum(); l_port ++ )
        {
            if ( router_cfg.port( l_port ).networkInterface() )
            {
                m_ni_list.push_back( EsynetNI( m_network_cfg, ni_id, router_id, l_port, m_argument_cfg ) );
                m_router_list[ router_id ].setInputNeighbor ( l_port, esynet::EsynetVC( ni_id + router_num, 0 ) );
                m_router_list[ router_id ].setOutputNeighbor( l_port, esynet::EsynetVC( ni_id + router_num, 0 ) );
                has_ni = true;
                ni_id += 1;
            }
        }

        if ( m_network_cfg->router( router_cfg.routerId() ).pipeCycle() != PIPE_DELAY_ )
        {
            addEvent(
                EsynetEvent::generateRouterEvent( 0.0, m_network_cfg->router( router_cfg.routerId() ).pipeCycle(), router_cfg.routerId() ) );
        }
    }
    
    // Read routing table
    if ( m_argument_cfg->routingAlg() == esynet::RA_TABLE )
    {
        long ni_count =  m_network_cfg->setNiCount();
        std::ifstream file_istream( m_argument_cfg->routingTable().c_str() );
        if ( !file_istream )
        {
            std::cout << "Cannot open routing table file. " << std::endl;
        }

        long id, dst, src, port, vc;

        while ( !file_istream.eof() )
        {
            file_istream >> id >> dst >> src >> port >> vc;
            if ( file_istream.eof() )
            {
                break;
            }
            assert( id >= 0 );
            assert( id < m_network_cfg->routerCount() );
            if ( dst >= 0 && dst < ni_count )
            {
                m_router_list[ id ].appendRoutingTable( dst, src, esynet::EsynetVC( port, vc ) );
            }
        }
    }
}

ostream& operator<<(ostream& os, const EsynetFoundation& sf)
{
    os << "**** Router list ****" << endl;
    for (vector< EsynetRouter >::const_iterator first = sf.m_router_list.begin(); first != sf.m_router_list.end(); first ++ ) 
    {
        os << ( *first );
    }
    return os;
}

void EsynetFoundation::receiveEvgMessage( const EsynetEvent & mesg )
{
    // Increase message id if not specified by message.
    long mesg_id = mesg.flit().flitId();
    if ( mesg_id < 0 )
    {
        mesg_id = m_statistic.injectedPacket();
    }

    // If latency measurement start, add mark on flag.
    long flag = mesg.flit().flitFlag();
    if ( m_latency_measure_state == MEASURE_PRE )
    {
        flag = flag | EsynetFlit::FLIT_MARK;
    }

    // Translate address.
    EsynetFlit t_flit = mesg.flit();
    long src_router = m_ni_list[ t_flit.srcNi() ].router();
    long src_port = m_ni_list[ t_flit.srcNi() ].port();
    long des_router = m_ni_list[ t_flit.desNi() ].router();
    long des_port = m_ni_list[ t_flit.desNi() ].port();

    // Inject packet to NI.
    m_ni_list[ mesg.srcId() ].injectPacket( EsynetFlit( mesg_id, t_flit.flitSize(), EsynetFlit::FLIT_HEAD
        , t_flit.srcNi(), src_router, src_port, t_flit.desNi(), des_router, des_port, m_current_time, esynet::EsynetPayload(), flag ) );
    m_statistic.incInjectPacket( m_current_time );
}

void EsynetFoundation::receiveRouterMessage( const EsynetEvent & mesg )
{
    // Generate next pipeline event so that the simulation can continue.
    addEvent( EsynetEvent::generateRouterEvent( mesg.eventStart() + mesg.pipeTime(), mesg.pipeTime(), mesg.srcId() ) );

    // If the pipeline event has same period as simulation period, generate packet and loop router event.
    if ( mesg.pipeTime() == PIPE_DELAY_  )
    {
        // Generate packets.
        std::vector< EsynetFlit > pac_list = m_packet_generator.generatePacket( m_current_time );
        for ( std::size_t i = 0; i < pac_list.size(); i ++ )
        {
            EsynetFlit flit_t = pac_list[ i ];
            addEvent( EsynetEvent::generateEvgEvent(
                m_current_time, flit_t.srcNi(), flit_t.desNi(), flit_t.flitSize(), flit_t.flitId() ) );
        }

        // Loop routers
        for ( long i = 0; i < m_ni_list.size(); i ++ )
        {
            m_ni_list[ i ].niSimPipeline();
        }
        for ( long i = 0; i < m_network_cfg->routerCount(); i ++ ) 
        {
            if ( m_network_cfg->router( i ).pipeCycle() == PIPE_DELAY_  )
            {
                m_router_list[ i ].routerSimPipeline();
            }
        }
        for ( long i = 0; i < m_ni_list.size(); i ++ )
        {
            const std::vector< EsynetEvent >& accepted = m_ni_list[ i ].acceptList();
            for ( int i = 0; i < accepted.size(); i ++ )
            {
                m_statistic.incAcceptPacket( m_current_time, m_current_time - accepted[ i ].flit().startTime() );
                if ( accepted[ i ].flit().marked() )
                {
                    m_statistic.incAcceptMarkPacket( m_current_time - accepted[ i ].flit().startTime() );
                }
            }
#ifndef ESYNETINTERFACE
            m_ni_list[ i ].clearAcceptList();
#endif
        }

        // Information propagate
        informationPropagate();
    }
    // If the pipeline event has different period as simulation period, update specified router.
    else
    {
        m_router_list[ mesg.srcId() ].routerSimPipeline();
//        m_ni_list[ mesg con_ni[ j ] ].runAfterRouter();
//        updateStatistic( m_ni_list[ con_ni[ j ] ].acceptList());
//#ifndef ESYNETINTERFACE
//        m_ni_list[ con_ni[ j ] ].clearAcceptList();
//#endif
    }
    updateStatistic();
}

void EsynetFoundation::receiveWireMessage(const EsynetEvent & mesg)
{
    // Destination port connects to NI, thus, the source is NI.
    if ( mesg.srcId() >= m_network_cfg->routerCount() )
    {
        m_ni_list[ mesg.srcId() - m_network_cfg->routerCount() ].clearFlitOnLink();
    }
    else
    {
        m_router_list[ mesg.srcId() ].outputPort( mesg.srcPc() ).clearFlitOnLink();
    }

    // Source port connects to NI, thus, the destination is NI.
    if ( mesg.desId() >= m_network_cfg->routerCount() )
    {
        // Flit is accepted by NI.
        m_ni_list[ mesg.desId() - m_network_cfg->routerCount() ].receiveFlit( mesg.desVc(), mesg.flit() );
    }
    else
    {
        // Flit is accepted by router.
        m_router_list[ mesg.desId() ].receiveFlit( mesg.desPc(), mesg.desVc(), mesg.flit() );
    }
}

void EsynetFoundation::receiveCreditMessage(const EsynetEvent & mesg)
{
    // Source port connects to NI, thus, the destination is NI.
    if ( mesg.desId() >= m_network_cfg->routerCount() )
    {
        // Flit is accepted by NI.
        m_ni_list[ mesg.desId() - m_network_cfg->routerCount() ].receiveCredit( mesg.desVc(), mesg.flit().flitId() );
    }
    else
    {
        // Flit is accepted by router.
        m_router_list[ mesg.desId() ].receiveCredit( mesg.desPc(), mesg.desVc(), mesg.flit().flitId()  );
    }
}

void EsynetFoundation::receiveNiReadMessage(const EsynetEvent& mesg)
{
    m_ni_list[ mesg.srcId() ].receiveNiInterrupt();
}


void EsynetFoundation::simulationResults()
{
    // Collect statistic of routers and network interfaces.
    for( size_t i = 0; i < m_router_list.size(); i ++ )
    {
        m_statistic = m_statistic + m_router_list[ i ].statistics();
        m_statistic.addRouterPower( m_current_time, i, m_router_list[ i ].powerUnit() );
    }

    m_statistic.printStatistics( std::cout, m_current_time
        , ( m_argument_cfg->latencyMeasurePacket() > 0 ), ( m_argument_cfg->throughputMeasurePacket() > 0 ) );

    /* print result for parallel */
    LINK_RESULT_APPEND( 7
        , (double)m_statistic.injectedPacket()
        , (double)m_statistic.acceptedPacket()
        , (double)m_statistic.packetInjectRate()
        , (double)m_statistic.latency()
        , (double)m_statistic.throughput()
        , (double)m_statistic.latencyMeasure()
        , (double)m_statistic.throughputMeasure()
    )
    LINK_RESULT_OUT
}

void EsynetFoundation::eventHandler(double time, const EsynetEvent& mess)
{
    EsynetSimBaseUnit::eventHandler(time, mess);
    for ( size_t i = 0; i < m_router_list.size(); i ++ )
    {
        m_router_list[ i ].setTime( time );
    }
    for ( size_t i = 0; i < m_ni_list.size(); i ++ )
    {
        m_ni_list[ i ].setTime( time );
    }
    
    switch( mess.eventType() ) 
    {
    case EsynetEvent::EVG:    receiveEvgMessage(mess); break;
    case EsynetEvent::ROUTER: receiveRouterMessage(mess); break;
    case EsynetEvent::WIRE:   receiveWireMessage(mess); break;
    case EsynetEvent::CREDIT: receiveCreditMessage(mess); break;
    case EsynetEvent::NIREAD: receiveNiReadMessage(mess); break;
    default: cout << "This message type " << mess.eventType() << " is not supported.\n"; break;
    }
    
    for ( size_t i = 0; i < m_router_list.size(); i ++ )
    {
        const std::vector< EsynetEvent > & mess_list = m_router_list[i].eventQueue();
        for ( size_t j = 0; j < mess_list.size(); j ++ )
        {
            addEvent( mess_list[ j ] );
        }
        m_router_list[ i ].clearEventQueue();
    }
    for ( size_t i = 0; i < m_ni_list.size(); i ++ )
    {
        const std::vector< EsynetEvent > & mess_list = m_ni_list[i].eventQueue();
        for ( size_t j = 0; j < mess_list.size(); j ++ )
        {
            addEvent( mess_list[ j ] );
        }
        m_ni_list[ i ].clearEventQueue();
    }
}

void EsynetFoundation::updateStatistic()
{
    if ( m_argument_cfg->injectedPacket() >= 0 && m_statistic.acceptedPacket() >= m_argument_cfg->injectedPacket() )
    {
        m_injection_state = MEASURE_END;
    }

    switch ( m_latency_measure_state )
    {
    case MEASURE_INIT:
        if ( m_statistic.injectedPacket() >= m_argument_cfg->warmUpPacket() )
        {
            m_latency_measure_state = MEASURE_PRE;
        }
        break;
    case MEASURE_PRE:
        if ( ( m_argument_cfg->latencyMeasurePacket() >= 0 ) &&
             ( m_statistic.injectedPacket() >= m_argument_cfg->warmUpPacket() + 
               m_argument_cfg->latencyMeasurePacket() ) )
        {
            m_latency_measure_state = MEASURE_ING;
        }
    case MEASURE_ING:
        if ( ( m_argument_cfg->latencyMeasurePacket() >= 0 ) &&
            ( m_statistic.acceptMarkPacket() >= m_argument_cfg->latencyMeasurePacket() ) )
        {
            m_latency_measure_state = MEASURE_END;
        }
        break;
    case MEASURE_END:
        break;
    }

    switch ( m_throughput_measure_state )
    {
    case MEASURE_INIT:
        if ( m_statistic.acceptedPacket() >= m_argument_cfg->warmUpPacket() )
        {
            m_throughput_measure_state = MEASURE_ING;
            m_statistic.setThroughputMeasureStart( m_current_time, m_statistic.acceptedPacket() );
        }
        break;
    case MEASURE_PRE:
        break;
    case MEASURE_ING:
        m_statistic.setThroughputMeasureStop( m_current_time, m_statistic.acceptedPacket() );
        if ( m_argument_cfg->throughputMeasurePacket() >= 0 &&
            m_statistic.acceptedPacket() >= m_argument_cfg->warmUpPacket() + m_argument_cfg->throughputMeasurePacket() )
        {
            m_throughput_measure_state = MEASURE_END;
        }
        break;
    case MEASURE_END:
        break;
    }
}

void EsynetFoundation::informationPropagate()
{
    // Add Code
}

std::vector< EsynetEvent > EsynetFoundation::acceptList()
{
    std::vector< EsynetEvent > accept_list;

    for ( int ni = 0; ni < m_ni_list.size(); ni ++ )
    {
        vector< EsynetEvent > ni_accept_list = m_ni_list[ ni ].acceptList();
        m_ni_list[ ni ].clearAcceptList();
        for ( int i = 0; i < ni_accept_list.size(); i ++ )
        {
            accept_list.push_back( ni_accept_list[ i ] );
        }
    }

    return accept_list;
}

