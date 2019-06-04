/*
 * File name : esynet_router_unit.h
 * Function : implement router module.
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

#include "esynet_router_unit.h"

/*************************************************
  Function : 
    ostream & operator<<(ostream& os, const SimRouter& sr)
  Description : 
    print EsynetConfig of router $sr$ to ostream os
  Input:
            ostream& os  output stream
    const SimRouter& Ri  router unit to print.
*************************************************/
ostream & operator<<(ostream& os, const EsynetRouter & sr)
{
    /* print router id, pipeline scalar */
    os << "router(" << sr.m_router_id << ")";
    os << " pipe_scalar=" << sr.m_router_cfg->pipeCycle();
    /* print input neighbor */
    os << " input_neighbor={";
    for ( size_t i = 0; i < sr.m_input_port.size(); i ++ )
    {
        os << "(" << sr.m_input_port[ i ].inputNeighborRouter() <<"," << sr.m_input_port[ i ].inputNeighborPort() << ")";
        /* last item, donot print spacer mark */
        if (i != sr.m_input_port.size() - 1)
        {
            os << ",";
        } /* else */
    } /* for (int i = 0; i < sr.physic_ports_; i ++) */
    os << "}";
    /* print output neighbor */
    os << " output_neighbor={";
    for (size_t i = 0; i < sr.m_output_port.size(); i ++)
    {
        os << "(" << sr.m_output_port[ i ].outputNeighborRouter()  <<"," << sr.m_output_port[ i ].outputNeighborPort() << ")";
        /* last item, donot print spacer mark */
        if (i != sr.m_output_port.size() - 1)
        {
            os << ",";
        } /* else */
    } /* end of  for (int i = 0; i < Ri.physic_ports_; i ++) */
    os << "}";
    /* print return */
    os << endl;

    return os;
}

EsynetRouter::EsynetRouter()
    : m_network_cfg()
    , m_router_cfg()
    , m_argu_cfg()
    , m_router_id()
    , m_router_addr()
    , m_input_port()
    , m_output_port()
    , m_power_unit()
    , m_vc_input_arbiter()
    , m_vc_output_arbiter()
    , m_port_input_arbiter()
    , m_port_output_arbiter()
    , m_flow_control()
    , m_curr_algorithm()
{
}

EsynetRouter::EsynetRouter( EsyNetworkCfg * network_cfg, long router_id, EsynetConfig * argument_cfg )
    : m_network_cfg( network_cfg )
    , m_router_cfg( &network_cfg->router( router_id ) )
    , m_argu_cfg( argument_cfg )
    , m_router_id( router_id )
    , m_router_addr()
    , m_input_port()
    , m_output_port()
    , m_power_unit( network_cfg->router( router_id ).portNum()
        , network_cfg->router( router_id ).maxVcNum()
        , network_cfg->flitSize( ATOM_WIDTH_ ), 0 )
    , m_statistics_unit()
    , m_vc_input_arbiter( network_cfg->router( router_id ).portNum() )
    , m_vc_output_arbiter( network_cfg->router( router_id ).portNum() )
    , m_port_input_arbiter()
    , m_port_output_arbiter()
    , m_flow_control( argument_cfg->flowControl() )
    , m_curr_algorithm()
{
    // Router address.
    m_router_addr = m_network_cfg->seq2Coord( m_router_id );

    // Initializate routing algorithm pointer
    switch (m_argu_cfg->routingAlg())
    {
    case esynet::RA_XY:    m_curr_algorithm = &EsynetRouter::algorithmXY;    break;
    case esynet::RA_TXY:   m_curr_algorithm = &EsynetRouter::algorithmTXY;   break;
    case esynet::RA_DYXY:  m_curr_algorithm = &EsynetRouter::algorithmDyXY;  break;
    case esynet::RA_TABLE: m_curr_algorithm = &EsynetRouter::algorithmTable; break;
    case esynet::RA_SINGLERING: m_curr_algorithm = &EsynetRouter::algorithmSingleRing; break;
    case esynet::RA_DOUBLERING: m_curr_algorithm = &EsynetRouter::algorithmDoubleRing; break;
    case esynet::RA_DIAMESH:  m_curr_algorithm = &EsynetRouter::algorithmDiaMesh;  break;
    case esynet::RA_DIATORUS: m_curr_algorithm = &EsynetRouter::algorithmDiaTorus; break;
    }
    m_routing_table.resize( m_network_cfg->setNiCount() );

    for ( long l_port = 0; l_port < m_router_cfg->portNum(); l_port ++ )
    {
        // Construct input and output ports
        m_input_port.push_back ( EsynetInputPort ( &m_router_cfg->port( l_port ) ) );
        m_output_port.push_back( EsynetOutputPort( &m_router_cfg->port( l_port ) ) );

        // Set neighbor to output ports.
        m_output_port[ l_port ].setOutputNeighbor(
            esynet::EsynetVC( m_router_cfg->port( l_port ).neighborRouter(), m_router_cfg->port( l_port ).neighborPort() ) ); 

        // Set neighbor to input ports.
        for ( int i = 0; i < m_network_cfg->routerCount(); i ++ )
        {
            for ( int j = 0; j < m_network_cfg->router( i ).portNum(); j ++ )
            {
                if ( m_network_cfg->router( i ).port( j ).neighborRouter() == router_id
                    &&  m_network_cfg->router( i ).port( j ).neighborPort() == l_port )
                {
                    m_input_port[ l_port ].setInputNeighbor( esynet::EsynetVC ( i, j ) );
                }
            }
        }

        // Construct arbiters
        for ( int vc = 0; vc < m_router_cfg->port( l_port ).inputVcNumber(); vc ++ )
        {
            m_vc_input_arbiter[ l_port ].push_back( EsynetArbiter( m_argu_cfg->arbiter(), m_router_cfg->totalOutputVc() ) );
            m_vc_input_arbiter[ l_port ][ vc ].setOutputPortMap( m_network_cfg->router( router_id ) );
        }
        for ( int vc = 0; vc < m_router_cfg->port( l_port ).outputVcNumber(); vc ++ )
        {
            m_vc_output_arbiter[ l_port ].push_back( EsynetArbiter( m_argu_cfg->arbiter(), m_router_cfg->totalInputVc() ) );
            m_vc_output_arbiter[ l_port ][ vc ].setInputPortMap( m_network_cfg->router( router_id ) );
        }
        m_port_input_arbiter.push_back( EsynetArbiter( m_argu_cfg->arbiter(), m_router_cfg->port( l_port ).inputVcNumber() ) );
        m_port_output_arbiter.push_back( EsynetArbiter( m_argu_cfg->arbiter(), m_router_cfg->totalInputVc() ) );
        m_port_output_arbiter[ l_port ].setInputPortMap( m_network_cfg->router( router_id ) );
    }
}

void EsynetRouter::receiveCredit(long phy, long vc, long credit)
{
    m_output_port[ phy ][ vc ].setCreditCounter( credit );
}

void EsynetRouter::receiveFlit(long phy, long vc, const EsynetFlit & flit)
{
    EsynetFlit flit_t = flit;

    /* insert flit into port phy virtual channel vc */
    m_input_port[ phy ][ vc ].addFlit( flit_t );
    /* power record */
    m_power_unit.powerBufferWrite( phy, flit_t.data() );
    /* insert ET_FLIT_RECEIVE event */
    addEvent( EsynetEvent( m_current_time, ET_FLIT_RECEIVE, -1, -1, -1, m_router_id, phy, vc, flit_t ) );
    /* head flit */
    if (flit_t.flitType() == EsynetFlit::FLIT_HEAD)
    {
        /* and there is no flit before it */
        if ( m_input_port[ phy ][ vc ].flitCount() == 1 )
        {
            /* change the state to ROUTING_ and insert ET_VC_STATE event */
            esynet::EsynetVCStatus oldstate = m_input_port[ phy ][ vc ].state();
            m_input_port[ phy ][ vc ].updateState( esynet::VCS_ROUTING );
            esynet::EsynetVCStatus newstate = m_input_port[ phy ][ vc ].state();
            addEvent( EsynetEvent( m_current_time, ET_VC_STATE, m_router_id, phy, vc, m_router_id, phy, vc, 
                EsynetFlit( 0, 0, EsynetFlit::FLIT_HEAD, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );
        } /* end of  if (input_module_.input(phy,vc).size() == 1) */
    } /* end of  if (flit.type() == EsynetFlit::FLIT_HEAD) */
    /* not head flit */
    else
    {
        /* and the state of port phy virtual channel vc is INIT_ */
        if ( m_input_port[ phy ][ vc ].state() == esynet::VCS_INIT )
        {
            /* change the state to SW_AB_ and insert ET_VC_STATE event */
            esynet::EsynetVCStatus oldstate = m_input_port[ phy ][ vc ].state();
            m_input_port[ phy ][ vc ].updateState( esynet::VCS_SW_AB );
            esynet::EsynetVCStatus newstate = m_input_port[ phy ][ vc ].state();
            addEvent( EsynetEvent( m_current_time, ET_VC_STATE, m_router_id, phy, vc, m_router_id, phy, vc, 
                EsynetFlit( 0, 0, EsynetFlit::FLIT_HEAD, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );
        } /* end of  if (input_module_.state(phy, vc) == INIT_) */
    } /* end of  else */
}

void EsynetRouter::routingDecision()
{
    /* for other physical ports */
    for ( size_t pc_l = 0; pc_l < m_input_port.size(); pc_l ++ )
    {
        /* for each vc in physical port pc */
        for ( long vc_l = 0; vc_l < m_input_port[ pc_l ].vcNumber(); vc_l ++ )
        {
            EsynetFlit flit_t;

            /* for the FLIT_HEAD flit, the state is ROUTING_ */
            if( m_input_port[ pc_l ][ vc_l ].state() == esynet::VCS_ROUTING )
            {
                /* get source and destination and source address */
                flit_t = m_input_port[ pc_l ][ vc_l ].getFlit();
                long des_t = flit_t.desRouter();
                long sor_t = flit_t.srcRouter();

                /* remove all routing requirement and insert ET_ROUTING_END event */
                for ( size_t seq = 0; seq < m_input_port[ pc_l ][ vc_l ].routing().size(); seq ++ )
                {
                    addEvent( EsynetEvent( m_current_time, ET_ROUTING_END,
                        m_router_id, pc_l, vc_l, m_router_id,
                        m_input_port[ pc_l ][ vc_l ].routing()[ seq ].first,
                        m_input_port[ pc_l ][ vc_l ].routing()[ seq ].second, 
                        flit_t ) );
                } /* end of  for (size_t seq = 0;  */
                m_input_port[ pc_l ][ vc_l ].clearRouting();
                /* remove all crouting requirement andvcSelection insert 
                    * ET_CROUTING_END event */
                m_input_port[ pc_l ][ vc_l ].clearConnection();
                /* call routing algorithm to insert routing requirement and
                    * insert ET_ROUTING event */
                (this->*m_curr_algorithm)( des_t, sor_t, pc_l, vc_l );
                if ( m_input_port[ pc_l ][ vc_l ].routing().size() > 0 )
                {
                    for ( size_t seq = 0; seq < 
                        m_input_port[ pc_l ][ vc_l ].routing().size(); seq ++ )
                    {
                        addEvent( EsynetEvent (m_current_time, ET_ROUTING, m_router_id, pc_l, vc_l, m_router_id,
                            m_input_port[ pc_l ][ vc_l ].routing()[ seq ].first, m_input_port[ pc_l ][ vc_l ].routing()[ seq ].second, 
                            flit_t ) );
                    } /* end of  for (size_t seq = 0;  */
                    /* change the state to VC_AB_, insert ET_VC_STATE event */
                    esynet::EsynetVCStatus oldstate = m_input_port[ pc_l ][ vc_l ].state();
                    m_input_port[ pc_l ][ vc_l ].updateState( esynet::VCS_VC_AB );
                    esynet::EsynetVCStatus newstate = m_input_port[ pc_l ][ vc_l ].state();
                    addEvent( EsynetEvent (m_current_time, ET_VC_STATE, m_router_id, pc_l, vc_l, m_router_id, pc_l, vc_l,  
                        EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );
                }
            } /* end of  if(input_module_.state(pc_l, vc_l) == ROUTING_) */
        } /* for(long vc_l = 0; vc_l < vc_number_; vc_l++) */
    } /* end of  for(long i = 1; i < physic_ports_; i ++) */
}

// *****************************************************//
// vc arbitration algorithms, which is the implementation//
// of selection function in routing                     //
// *****************************************************//
//***************************************************************************//
//choose one of the candidate routing vc
pair< long, long > EsynetRouter::vcSelection(long a, long b)
{
    const vector< esynet::EsynetVC > & vc_can_t = m_input_port[ a ][ b ].routing();
    long r_size_t = (long)vc_can_t.size();

    for ( long i = 0; i < r_size_t; i ++ )
    {
        esynet::EsynetVC v_t = vc_can_t[ i ];
        bool port_avail = false;
        if ( m_flow_control == esynet::FC_RING )
        {
            // NI channel, only forward packet if there is enough space for how packet.
            if ( m_router_cfg->port( a ).networkInterface() )
            {
                EsynetFlit& flit_t = m_input_port[ a ][ b ].getFlit();
                if( m_output_port[ v_t.first ][ v_t.second ].usage() == esynet::VC_FREE &&
                    m_output_port[ v_t.first ][ v_t.second ].creditCounter() > flit_t.flitSize() )
                {
                    port_avail = true;
                }
            }
            // other channel forward packet if there is free space.
            else
            {
                if( m_output_port[ v_t.first ][ v_t.second ].usage() == esynet::VC_FREE )
                {
                    port_avail = true;
                }
            }
        }
        else
        {
            if ( m_output_port[ v_t.first ][ v_t.second ].usage() == esynet::VC_FREE )
            {
                port_avail = true;
            }
        }

        if ( port_avail )
        {
            m_vc_input_arbiter[ a ][ b ].setRequest( vc_can_t[ i ] );
        }
    }

    esynet::EsynetVC win = m_vc_input_arbiter[ a ][ b ].grantVc();
    if ( win.first >= 0 && win.second >= 0 )
    {
        return win;
    }
    else
    {
        return esynet::VC_NULL;
    }
}

//***************************************************************************//
void EsynetRouter::vcArbitration()
{
    esynet::EsynetAtomType vc_request = 0;
    
    for ( size_t inphy = 0; inphy < m_input_port.size(); inphy ++ )
    {
        for ( long invc = 0; invc < m_input_port[ inphy ].vcNumber(); invc ++ )
        {
            esynet::EsynetVC vc_t;
            if ( m_input_port[ inphy ][ invc ].state() == esynet::VCS_VC_AB )
            {
                vc_t = vcSelection( inphy, invc );

                if ( ( vc_t.first >= 0 ) && ( vc_t.second >= 0 ) )
                {
                    m_vc_output_arbiter[ vc_t.first ][ vc_t.second ].setRequest( esynet::EsynetVC ( inphy, invc ) );
                    vc_request = vc_request | (0x1LL << (inphy * m_router_cfg->maxVcNum() + invc));
                }
            }
        }
    }
    
    for ( size_t outphy = 0; outphy < m_output_port.size(); outphy ++ )
    {
        for ( long outvc = 0; outvc < m_output_port[ outphy ].vcNumber(); outvc ++ )
        {
            if ( m_output_port[ outphy ][ outvc ].usage() == esynet::VC_FREE )
            {
                esynet::EsynetVC vc_win = m_vc_output_arbiter[ outphy ][ outvc ].grantVc();
                if ( vc_win.first >= 0 && vc_win.second >= 0 )
                {
                    esynet::EsynetVCStatus oldstate = m_input_port[ vc_win.first ][ vc_win.second ].state();
                    m_input_port[ vc_win.first ][ vc_win.second ].updateState( esynet::VCS_SW_AB );
                    esynet::EsynetVCStatus newstate = m_input_port[ vc_win.first ][ vc_win.second ].state();
                    addEvent( EsynetEvent( m_current_time, ET_VC_STATE,
                        m_router_id, vc_win.first, vc_win.second, m_router_id, vc_win.first, vc_win.second,  
                        EsynetFlit(0, 0, EsynetFlit::FLIT_HEAD, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );

                    m_input_port[ vc_win.first ][ vc_win.second ].assignConnection( esynet::EsynetVC ( outphy, outvc ) );
                    m_output_port[ outphy ][ outvc ].acquire( vc_win );
                    addEvent( EsynetEvent( m_current_time, ET_ASSIGN,
                        m_router_id, vc_win.first, vc_win.second, 
                        m_router_id, outphy, outvc, EsynetFlit()));
                    m_power_unit.powerVCArbiter( outphy, outvc, vc_request,
                        vc_win.first * m_input_port[ vc_win.first ].vcNumber() + vc_win.second );
                }
            }
            else
            {
                m_vc_output_arbiter[ outphy ][ outvc ].clearRequest();    
            }
        }
    }
}
//***************************************************************************//

//***************************************************************************//
//switch arbitration pipeline stage
void EsynetRouter::swArbitration()
{
    for ( size_t inphy = 0; inphy < m_input_port.size(); inphy ++ )
    {
        for( long invc = 0; invc < m_input_port[ inphy ].vcNumber(); invc ++ )
        {
            if( m_input_port[ inphy ][ invc ].state() == esynet::VCS_SW_AB ) 
            {
                esynet::EsynetVC out_t = m_input_port[ inphy ][ invc ].connection();
                if( m_output_port[ out_t.first ][ out_t.second ].creditCounter() > 0 )
                {
                    m_port_input_arbiter[ inphy ].setRequest( invc );
                }
            }
        }

        long win_t = m_port_input_arbiter[ inphy ].grant();
        if ( win_t >= 0 )
        {
            esynet::EsynetVC r_t = m_input_port[ inphy ][ win_t ].connection();
            m_port_output_arbiter[ r_t.first ].setRequest( esynet::EsynetVC ( inphy, win_t ) );
        }
    }

    for( size_t outph = 0; outph < m_output_port.size(); outph ++ )
    {
        esynet::EsynetVC vc_win = m_port_output_arbiter[ outph ].grantVc();
        if ( vc_win.first >= 0 && vc_win.second >= 0 )
        {
            esynet::EsynetVCStatus oldstate = m_input_port[ vc_win.first ][ vc_win.second ].state();
            m_input_port[ vc_win.first ][ vc_win.second ].updateState( esynet::VCS_SW_TR );
            esynet::EsynetVCStatus newstate = m_input_port[ vc_win.first ][ vc_win.second ].state();
            addEvent( EsynetEvent ( m_current_time, ET_VC_STATE,
                m_router_id, vc_win.first, vc_win.second, m_router_id, vc_win.first, vc_win.second, 
                EsynetFlit( 0, 0, (EsynetFlit::FlitType)0, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );
        }
    }
}

//***************************************************************************//
//flit out buffer to the output buffer
void EsynetRouter::flitOutbuffer()
{
    for( size_t phy = 0; phy < m_input_port.size(); phy ++ )
    {
        for( long vc = 0; vc < m_input_port[ phy ].vcNumber(); vc ++ )
        {
            if( m_input_port[ phy ][ vc ].state() == esynet::VCS_SW_TR ) 
            {
                esynet::EsynetVC out_t = m_input_port[ phy ][ vc ].connection();
                m_output_port[ out_t.first ][ out_t.second ].decCreditCounter();

                long credit_id = m_input_port[ phy ].inputNeighborRouter();
                long credit_port = m_input_port[ phy ].inputNeighborPort();
                if ( credit_id >= 0 && credit_port >= 0 )
                {
                    addEvent( EsynetEvent::generateCreditEvent( m_current_time + CREDIT_DELAY_,
                        m_router_id, phy, vc, credit_id, credit_port, vc,
                        m_router_cfg->port( phy ).inputBuffer() - m_input_port[ phy ][ vc ].flitCount() - 1 ) );
                }
                long in_size_t = (long)m_input_port[ phy ][ vc ].flitCount();
                EsynetFlit flit_t = m_input_port[ phy ][ vc ].getFlit();
                m_input_port[ phy ][ vc ].removeFlit();

                m_power_unit.powerBufferRead( phy, flit_t.data() );
                m_power_unit.powerCrossbarTraversal( phy, out_t.first, flit_t.data() );

                m_output_port[ out_t.first ].addFlit( flit_t );
                addEvent( EsynetEvent( m_current_time, ET_FLIT_SWITCH, m_router_id, phy, vc, m_router_id, out_t.first, out_t.second, flit_t) );

                m_output_port[ out_t.first ].addAdd( out_t);
                if (flit_t.flitType() == EsynetFlit::FLIT_TAIL || flit_t.flitSize() == 1 ) 
                {
                    addEvent( EsynetEvent( m_current_time, ET_ASSIGN_END, m_router_id, 
                        m_output_port[ out_t.first ][  out_t.second ].assign().first, 
                        m_output_port[ out_t.first ][ out_t.second ].assign().second, 
                        m_router_id, out_t.first, out_t.second, EsynetFlit()));
                    m_output_port[ out_t.first ][ out_t.second ].release();
                }
                if( in_size_t > 1 ) 
                {
                    if (flit_t.flitType() == EsynetFlit::FLIT_TAIL || flit_t.flitSize() == 1 ) 
                    {
                        esynet::EsynetVCStatus oldstate = m_input_port[ phy ][ vc ].state();
                        m_input_port[ phy ][ vc ].updateState( esynet::VCS_ROUTING );
                        esynet::EsynetVCStatus newstate = m_input_port[ phy ][ vc ].state();
                        addEvent( EsynetEvent( m_current_time, ET_VC_STATE, m_router_id, phy, vc, m_router_id, phy, vc, 
                            EsynetFlit(0, 0, (EsynetFlit::FlitType)0, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );
                    }
                    else 
                    {
                        esynet::EsynetVCStatus oldstate = m_input_port[ phy ][ vc ].state();
                        m_input_port[ phy ][ vc ].updateState( esynet::VCS_SW_AB );
                        esynet::EsynetVCStatus newstate = m_input_port[ phy ][ vc ].state();
                        addEvent( EsynetEvent( m_current_time, ET_VC_STATE, m_router_id, phy, vc, m_router_id, phy, vc, 
                            EsynetFlit(0, 0, (EsynetFlit::FlitType)0, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );
                    }
                }
                else 
                {
                    esynet::EsynetVCStatus oldstate = m_input_port[ phy ][ vc ].state();
                    m_input_port[ phy ][ vc ].updateState( esynet::VCS_INIT );
                    esynet::EsynetVCStatus newstate = m_input_port[ phy ][ vc ].state();
                    addEvent( EsynetEvent( m_current_time, ET_VC_STATE, m_router_id, phy, vc, m_router_id, phy, vc, 
                        EsynetFlit(0, 0, (EsynetFlit::FlitType)0, oldstate, newstate, 0, esynet::EsynetPayload() ) ) );
                }
            }
        }
    }
}

void EsynetRouter::flitTraversal()
{
    for ( size_t phy = 0; phy < m_output_port.size(); phy ++ )
    {
        if ( m_output_port[ phy ].flitCount() > 0 )
        {
            double flit_delay_t = WIRE_DELAY_ + m_current_time;
            long wire_add_t = m_output_port[ phy ].outputNeighborRouter();
            long wire_pc_t = m_output_port[ phy ].outputNeighborPort();

            EsynetFlit flit_t = m_output_port[ phy ].getFlit();
            esynet::EsynetVC outadd_t = m_output_port[ phy ].getAdd();

            m_power_unit.powerLinkTraversal( phy, flit_t.data() );

            m_output_port[ phy ].removeFlit();
            m_output_port[ phy ].removeAdd();
            m_output_port[ phy ].setFlitOnLink();

            addEvent( EsynetEvent::generateWireEvent( flit_delay_t, m_router_id, phy, 0,  wire_add_t, wire_pc_t, outadd_t.second, flit_t ) );
            addEvent( EsynetEvent( m_current_time, ET_FLIT_SEND, m_router_id, phy, outadd_t.second, -1, -1, -1, flit_t ) );
        }
    }
}

void EsynetRouter::routerSimPipeline()
{
#if 0
    // stage 5, flit traversal
    flitTraversal();
    // stage 4, flit output buffer
    flitOutbuffer();
    // stage 3, switch arbitration
    swArbitration();
    // stage 2, vc arbitration
    vcArbitration();
    // stage 1, routing decision
    routingDecision();
#endif

    // stage 5, flit traversal
    flitTraversal();
    // stage 1, routing decision
    routingDecision();
    // stage 2, vc arbitration
    vcArbitration();
    // stage 3, switch arbitration
    swArbitration();
    // stage 4, flit output buffer
    flitOutbuffer();
}
