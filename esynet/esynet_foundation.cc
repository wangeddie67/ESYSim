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

EsynetFoundation::EsynetFoundation( EsyNetworkCfg * network_cfg, EsynetConfig * argument_cfg ):
    m_network_cfg( network_cfg ),
    m_argument_cfg( argument_cfg ),
    m_statistic(),
    m_packet_generator(network_cfg, argument_cfg),
    m_latency_measure_state( MEASURE_INIT ),
    m_throughput_measure_state( MEASURE_INIT ),
    m_injection_state( MEASURE_INIT ),
    m_received_id_offset( 0 )
{
    long router_num = m_network_cfg->routerCount();

    for ( int router_id = 0; router_id < router_num; router_id ++ )
    {
        EsyNetworkCfgRouter router_cfg = m_network_cfg->router( router_id );
        m_router_list.push_back( EsynetRouter(m_network_cfg, router_id, m_argument_cfg));
        bool has_ni = false;
        for ( int l_port = 0; l_port < router_cfg.portNum(); l_port ++ )
        {
            if ( router_cfg.port( l_port ).networkInterface() )
            {
                m_ni_list.push_back( EsynetNI(m_network_cfg, router_id, m_argument_cfg));
                m_router_list[ router_id ].setInputNeighborAddr( l_port, EsynetVC ( m_ni_list.size() + router_num - 1, 0 ) );
                m_router_list[ router_id ].setOutputNeighborAddr( l_port, EsynetVC ( m_ni_list.size() + router_num - 1, 0 ) );
                has_ni = true;
            }
        }
        if (!has_ni)
        {
            m_ni_list.push_back(
                EsynetNI(m_network_cfg, router_id, m_argument_cfg));
            m_router_list[ router_id ].setInputNeighborAddr( 0, EsynetVC ( m_ni_list.size() + router_num - 1, 0 ) );
            m_router_list[ router_id ].setOutputNeighborAddr( 0, EsynetVC ( m_ni_list.size() + router_num - 1, 0 ) );
        }

        if ( m_network_cfg->router( router_cfg.routerId() ).pipeCycle() != PIPE_DELAY_ )
        {
            addEvent( EsynetEvent::generateRouterEvent( 0.0, m_network_cfg->router( router_cfg.routerId() ).pipeCycle(), router_cfg.routerId() ) );
        }
    }
}

ostream& operator<<(ostream& os, const EsynetFoundation& sf)
{
    os << "************Router list*************" << endl;
    for (vector< EsynetRouter >::const_iterator first = sf.m_router_list.begin();
        first != sf.m_router_list.end(); first ++ ) 
    {
        os << ( *first );
    }
    return os;
}

void EsynetFoundation::receiveEvgMessage(const EsynetEvent & mesg)
{
    long mesg_id = mesg.flit().flitId();
    if ( mesg_id < 0 )
    {
        mesg_id = m_statistic.injectedPacket();
    }

    long flag = mesg.flit().flitFlag();
    if ( m_latency_measure_state == MEASURE_PRE )
    {
        flag = flag | EsynetFlit::FLIT_MARK;
    }

    EsynetFlit t_flit = mesg.flit();
    m_ni_list[ mesg.srcId() ].injectPacket( EsynetFlit(mesg_id, t_flit.flitSize(), EsynetFlit::FLIT_HEAD, 
        _flit.srcAddr(), t_flit.desAddr(), m_current_time, esynet::EsynetPayload(), flag ) );
    m_statistic.incInjectPacket(m_current_time);
}

void EsynetFoundation::receiveRouterMessage(const EsynetEvent & mesg)
{
    addEvent(EsynetEvent::generateRouterEvent( mesg.eventStart() + mesg.pipeTime(), mesg.pipeTime(), mesg.srcId() ) );
            
    if ( mesg.pipeTime() == PIPE_DELAY_  )
    {
        vector< EsynetFlit > pac_list = m_packet_generator.generatePacket();
        for (size_t i = 0; i < pac_list.size(); i ++)
        {
            EsynetFlit flit_t = pac_list[i];
            addEvent( EsynetEvent::generateEvgEvent(m_current_time, flit_t.srcAddr(), flit_t.desAddr(), flit_t.flitSize(), flit_t.flitId(), m_current_time));
        }
        
        for ( long i = 0; i < m_network_cfg->routerCount(); i ++ ) 
        {
            if ( m_network_cfg->router( i ).pipeCycle() == PIPE_DELAY_  )
            {
                m_ni_list[ i ].runBeforeRouter();
                m_router_list[ i ].routerSimPipeline();
                m_ni_list[ i ].runAfterRouter();
                updateStatistic(m_ni_list[i].acceptList());
#ifndef ESYNETINTERFACE
                m_ni_list[i].clearAcceptList();
#endif
            }
        }
        
        while (m_received_id.size() > 0)
        {
            if (m_received_id[0])
            {
                m_received_id_offset ++;
                m_received_id.erase(m_received_id.begin());
            }
            else
            {
                break;
            }
        }
        informationPropagate();
    }
    else
    {
        m_ni_list[ mesg.srcId() ].runBeforeRouter();
        m_router_list[ mesg.srcId() ].routerSimPipeline();
        m_ni_list[ mesg.srcId() ].runAfterRouter();
        updateStatistic(m_ni_list[ mesg.srcId() ].acceptList());
#ifndef ESYNETINTERFACE
        m_ni_list[ mesg.srcId() ].clearAcceptList();
#endif
    }
}

void EsynetFoundation::receiveWireMessage(const EsynetEvent & mesg)
{
    if ( mesg.srcId() >= m_network_cfg->routerCount() )
    {
        m_ni_list[ mesg.srcId() - m_network_cfg->routerCount() ].clearFlitOnLink();
    }
    else
    {
        m_router_list[ mesg.srcId() ].outputPort( mesg.srcPc() ).clearFlitOnLink();
    }
    
    if ( mesg.desId() >= m_network_cfg->routerCount() )
    {
        m_ni_list[ mesg.desId() - m_network_cfg->routerCount() ].receiveFlit( mesg.desVc(), mesg.flit() );
    }
    else
    {
        m_router_list[ mesg.desId() ].receiveFlit( mesg.desPc(), mesg.desVc(), mesg.flit() );
    }
}

void EsynetFoundation::receiveCreditMessage(const EsynetEvent & mesg)
{
    if ( mesg.desId() >= m_network_cfg->routerCount() )
    {
        m_ni_list[ mesg.desId() - m_network_cfg->routerCount() ].receiveCredit( mesg.desVc(), mesg.flit().flitId() );
    }
    else
    {
        m_router_list[ mesg.desId() ].receiveCredit( mesg.desPc(), mesg.desVc(), mesg.flit().flitId() );
    }
}

void EsynetFoundation::receiveNiReadMessage(const EsynetEvent& mesg)
{
    m_ni_list[ mesg.srcId() ].receivePacketHandler();
}


void EsynetFoundation::simulationResults()
{
    double curr_time = m_current_time;
    long router_num = m_network_cfg->routerCount();

    EsynetRouterStatistic statistics_unit;
    for( size_t i = 0; i < m_router_list.size(); i ++ )
    {
        statistics_unit.add( m_router_list[ i ].statistics() );
    }
    EsynetNIStatistic ni_statistic;
    for( size_t i = 0; i < m_ni_list.size(); i ++ )
    {
        ni_statistic.add( m_ni_list[ i ].statistic() );
    }

    double total_mem_power = 0;
    double total_crossbar_power = 0;
    double total_arbiter_power = 0;
    double total_power = 0;
    double total_link_power = 0;

    for( size_t i = 0; i < m_router_list.size(); i ++ )
    {
        total_mem_power += m_router_list[ i ].powerUnit().powerBufferReport( curr_time, router_num );
        total_crossbar_power += m_router_list[ i ].powerUnit().powerCrossbarReport( curr_time, router_num );
        total_arbiter_power += m_router_list[ i ].powerUnit().powerArbiterReport( curr_time, router_num );
        total_link_power += m_router_list[ i ].powerUnit().powerLinkReport( curr_time, router_num );
    }
    total_mem_power /= curr_time;
    total_crossbar_power /= curr_time;
    total_link_power /= curr_time;
    total_arbiter_power /= curr_time;
    total_power = total_mem_power + total_crossbar_power + total_arbiter_power + total_link_power;
 
    double throughput_measure_cycle = m_statistic.throughputMeasureStopTime() - m_statistic.throughputMeasureStartTime();
    long throughput_measure_packet = m_statistic.throughputMeasureStopPacket() - m_statistic.throughputMeasureStartPacket();
    double throughput_measure = 0.0;
    if ( throughput_measure_cycle > 0 )
    {
        throughput_measure = throughput_measure_packet / throughput_measure_cycle;
    }

    double average_latency = 0.0;
    if ( m_statistic.acceptMarkPacket() > 0 )
    {
        average_latency = m_statistic.totalMarkDelay() / m_statistic.acceptMarkPacket();
    }
    double deliver_rate = 0.0;
    if ( ni_statistic.injectedPacket() > 0 )
    {
        deliver_rate = (double)ni_statistic.nonDropPacket() / (double)ni_statistic.injectedPacket();
    }
    double average_e2e_delay = 0.0;
    if ( ni_statistic.acceptedPacket() > 0 )
    {
        average_e2e_delay = m_statistic.totalE2EDelay() / (double)m_statistic.acceptedPacket(); 
    }
    double packet_inject_rate = 0.0;
    if ( m_statistic.injectStopTime() - m_statistic.injectStartTime() > 0 )
    {
        packet_inject_rate = ni_statistic.injectedPacket() / ( m_statistic.injectStopTime() - m_statistic.injectStartTime() );
    }

    cout.precision(6);
    cout << "**** General Information *************************" << endl;
    cout << "packet injected:      " << ni_statistic.injectedPacket() << endl;
    cout << "total finished:       " << ni_statistic.acceptedPacket() << endl;
    cout << "average Delay:        " << average_latency << endl;
    cout << "**** Power Consumption ***************************" << endl;
    cout << "total mem power:      " << total_mem_power * POWER_NOM_ << endl;
    cout << "total crossbar power: " << total_crossbar_power * POWER_NOM_ << endl;
    cout << "total arbiter power:  " << total_arbiter_power * POWER_NOM_ << endl;
    cout << "total link power:     " << total_link_power * POWER_NOM_ << endl;
    cout << "total power:          " << total_power * POWER_NOM_ << endl;
    cout << "**** Latency/Throughput Measurement **************" << endl;
    cout << "measured delay:       " << m_statistic.totalMarkDelay() << endl;
    cout << "marked packet:        " << m_statistic.acceptMarkPacket() << endl;
    cout << "marked latency:       " << average_latency << endl;
    cout << "measured finished:    " << throughput_measure_packet << endl;
    cout << "measured cycle:       " << throughput_measure_cycle  << endl;
    cout << "throughput:           " << throughput_measure << endl;
    cout << "**** Fault Tolerant Methods **********************" << endl;
    cout << "total drop flit:      " << ni_statistic.dropFlit()  << endl;
    cout << "total non-drop packet:" << ni_statistic.nonDropPacket() << endl;
    cout << "deliver rate:         " << deliver_rate << endl;
    cout << "total acknowledge :   " << ni_statistic.injectAckPacket() << endl;
    cout << "total retransmission: " << ni_statistic.retransmissionPacket()  << endl;
    cout << "average e2e delay:    " << average_e2e_delay << endl;
    cout << "**************************************************" << endl;

    /* print result for parallel */
    LINK_RESULT_APPEND( 12,
        (double)ni_statistic.injectedPacket(),
        (double)ni_statistic.acceptedPacket(),
        average_latency, throughput_measure, packet_inject_rate,
        (double)statistics_unit.bistEmptyCounter(),
        (double)statistics_unit.bistEmptyCycle(),
        (double)statistics_unit.bistRecoverCounter(),
        (double)statistics_unit.bistRecoverCycle(),
        average_e2e_delay,
        (double)ni_statistic.retransmissionPacket(),
        (double)m_statistic.acceptedPacket()
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

void EsynetFoundation::updateStatistic(const vector< EsynetEvent > & accepted)
{
    for (size_t i = 0; i < accepted.size(); i ++)
    {
        long flit_id = accepted[i].flit().flitId();
        if (flit_id < m_received_id_offset)
        {
            continue;
        }
        long flit_id_offset = flit_id - m_received_id_offset;
        if (flit_id_offset < m_received_id.size())
        {
            if (m_received_id[flit_id_offset])
            {
                continue;
            }
        }
        
        m_statistic.incAcceptPacket(m_current_time, 
            m_current_time - accepted[i].flit().e2eStartTime() );
        if (accepted[i].flit().marked())
        {
            m_statistic.incAcceptMarkPacket(m_current_time - accepted[i].flit().e2eStartTime() );
        }
        
        if (m_received_id.size() <= flit_id_offset)
        {
            for (int i = m_received_id.size(); i < flit_id_offset; i ++)
            {
                m_received_id.push_back(false);
            }
            m_received_id.push_back(true);
        }
        else
        {
            m_received_id[flit_id_offset] = true;
        }
    }
    
    if ( m_argument_cfg->injectedPacket() >= 0 &&
         m_statistic.acceptedPacket() >= m_argument_cfg->injectedPacket() )
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
            ( m_statistic.acceptMarkPacket() >=
                m_argument_cfg->latencyMeasurePacket() ) )
        {
            m_latency_measure_state = MEASURE_END;
        }
        break;
    }

    switch ( m_throughput_measure_state )   
    {
    case MEASURE_INIT:
        if ( m_statistic.acceptedPacket() >= m_argument_cfg->warmUpPacket() )
        {
            m_throughput_measure_state = MEASURE_ING;
            m_statistic.setThroughputMeasureStartTime( m_current_time );
            m_statistic.setThroughputMeasureStartPacket( 
                m_statistic.acceptedPacket() );
        }
        break;
    case MEASURE_ING:
        m_statistic.setThroughputMeasureStopTime( m_current_time );
        m_statistic.setThroughputMeasureStopPacket( 
                        m_statistic.acceptedPacket() );
        if ( m_argument_cfg->throughputMeasurePacket() >= 0 &&
            m_statistic.acceptedPacket() >= m_argument_cfg->warmUpPacket() + 
            m_argument_cfg->throughputMeasurePacket() )
        {
            m_throughput_measure_state = MEASURE_END;
        }
        break;
    }
}

void EsynetFoundation::informationPropagate()
{
    for ( long id = 0; id < m_network_cfg->routerCount(); id ++ )
    {
        m_ni_list[ id ].setEmptyReqire( (m_router_list[ id ].bistPortState( 0 ) == BIST_PORT_EMPTY) );
        m_router_list[ id ].setPortEmptyAck( 0, m_ni_list[ id ].isEmpty() );
        for ( long phy = 1; phy < m_network_cfg->router( id ).portNum(); 
             phy ++ )
        {
            bool t_faulty_state = m_router_list[ id ].neighborFaulty( phy );
            
            long t_neighbor_id = 
                m_network_cfg->router( id ).port( phy ).neighborRouter();
            long t_neighbor_port = 
                m_network_cfg->router( id ).port( phy ).neighborPort();
            
            if ( t_neighbor_id >= 0 && t_neighbor_port >= 0 )
            {
                m_router_list[ t_neighbor_id ].
                    setNeighborFaulty( t_neighbor_port, t_faulty_state );
                m_router_list[ t_neighbor_id ].setPortEmptyAck( 
                    t_neighbor_port, 
                    m_router_list[ id ].outputPort( phy ).isEmpty() );
                m_router_list[ id ].setBistPortState( phy, 
                    m_router_list[ t_neighbor_id ].
                    bistPortState( t_neighbor_port ) );

                long t_corner_in_port = 0;
                switch ( phy )
                {
                case 1: 
                case 5: t_corner_in_port = 4; break;
                case 2: 
                case 6: t_corner_in_port = 3; break;
                case 3: t_corner_in_port = 1; break;
                case 4: t_corner_in_port = 2; break;
                }
                long t_corner_id = m_network_cfg->router( id ).
                    port( t_corner_in_port ).neighborRouter();
                long t_corner_port = m_network_cfg->router( id ).
                    port( t_corner_in_port ).neighborPort();
                
                if ( t_corner_id >= 0 && t_corner_port >= 0 )
                {
                    bool t_cornor_faulty_state = m_router_list[ t_corner_id ].
                        neighborFaulty( t_corner_port);
                    m_router_list[ t_neighbor_id ].setCornerFaulty(
                         t_neighbor_port, t_cornor_faulty_state );
                }
            }
        }        
    }
}

vector< EsynetEvent > EsynetFoundation::acceptList()
{
    vector< EsynetEvent > accept_list;

    for ( int ni = 0; ni < m_ni_list.size(); ni ++ )
    {
        vector< EsynetEvent > ni_accept_list = 
            m_ni_list[ ni ].acceptList();
        m_ni_list[ ni ].clearAcceptList();
        for ( int i = 0; i < ni_accept_list.size(); i ++ )
        {
            accept_list.push_back( ni_accept_list[ i ] );
        }
    }

    return accept_list;
}
