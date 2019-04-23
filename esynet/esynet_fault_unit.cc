/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#include "esynet_fault_unit.h"

#include <vector>
#include <string>
#include <iostream>

EsynetFaultStateMachine::EsynetFaultStateMachine() :
    m_state( 0 ),
    m_state_trans_matrix(),
    m_state_out()
{
    setConstantState( false );
}

void EsynetFaultStateMachine::configFault(EsyFaultConfigItem & faultcfg)
{
    m_state = 0;
    m_state_out.clear();
    for ( size_t i = 0; i < faultcfg.state().size(); i ++ )
    {
        if ( faultcfg.state()[ i ] > 0 )
        {
            m_state_out.push_back( true );
        }
        else
        {
            m_state_out.push_back( false );
        }
    }
    m_state_trans_matrix = faultcfg.transferMatrix();
}

long EsynetFaultStateMachine::nextState()
{
    if ( m_state_out.size() <= 1 )
    {
        return 0;
    }
	double prob = EsynetSRGenFlatDouble(0, 1);
    size_t next_state = 0;
    for ( next_state = 0; next_state < m_state_out.size(); next_state ++ )
    {
        if ( prob < m_state_trans_matrix[ m_state ][ next_state ] )
        {
            break;
        }
        else
        {
            prob -= m_state_trans_matrix[ m_state ][ next_state ];
        }
    }

    if ( next_state < m_state_out.size() )
    {
        m_state = next_state;
    }
    return m_state;
}

void EsynetFaultStateMachine::setConstantState(bool state)
{
    m_state = 0;

    m_state_out.clear();
    m_state_out.push_back( state );

    m_state_trans_matrix.clear();
    m_state_trans_matrix.resize( 1 );
    m_state_trans_matrix[ 0 ].push_back( 1.0 );
}

ostream& operator<<(ostream& os, const EsynetFaultStateMachine & fsm)
{
    if ( fsm.constLiving() )
    {
        return os;
    }
    os << "Matrix={";
    for ( int c_state = 0; c_state < fsm.stateNumber(); c_state ++ )
    {
        os << "{";
        for ( int n_state = 0; n_state < fsm.stateNumber(); n_state ++ )
        {
            os << fsm.stateTransMatrix()[ c_state ][ n_state ] << ",";
        }
        os << "},";
    }
    os << "} ";
    os << "Out={";
    for ( int c_state = 0; c_state < fsm.stateNumber(); c_state ++ )
    {
        os << fsm.stateOut()[ c_state ] << ",";
    }
    os << "} ";
    os << "State=" << fsm.currentState() << " ";
    os << "StateOut=" << (fsm.currentStateOut()?"True":"False");
    return os;
}

void EsynetRouterFaultUnit::configFaultList(EsyFaultConfigList & faultlist)
{
	EsyFaultConfigItem t_item =
		faultlist.bestMatchItem(EsyFaultConfigItem::FAULT_ROUTER, m_id);
    configFault( t_item );
}

ostream& operator<<(ostream& os, const EsynetRouterFaultUnit & sr)
{
    if ( !sr.constLiving() )
    {
		EsynetFaultStateMachine t_base = sr;
        os << "\tRouter fault " << "( router:" << sr.m_id << " ) " <<
                t_base << endl;
    }
    return os;
}

void EsynetPortFaultUnit::configFaultList(EsyFaultConfigList & faultlist)
{
	EsyFaultConfigItem t_item =
		faultlist.bestMatchItem(EsyFaultConfigItem::FAULT_PORT, m_id, m_port);
    configFault( t_item );
}

ostream& operator<<(ostream& os, const EsynetPortFaultUnit & sr)
{
    if ( !sr.constLiving() )
    {
		EsynetFaultStateMachine t_base = sr;
        os << "\tRouter fault " << 
            "( router:" << sr.m_id << "; port:" << sr.m_port << " ) " <<
                t_base << endl;
    }
    return os;
}

void EsynetLinkFaultUnit::configFaultList(EsyFaultConfigList & faultlist)
{
    m_const_flag = true;
    for ( size_t l_point = 0; l_point < size(); l_point ++ )
    {
		EsyFaultConfigItem t_item =
            faultlist.bestMatchItem( m_type, m_id, m_port, -1, l_point );
        at( l_point ).configFault( t_item );
        
        if ( !t_item.isConstant() )
        {
            m_const_flag = false;
        }
    }

}

void EsynetLinkFaultUnit::nextState()
{
    if ( m_const_flag )
    {
        return;
    }
    for ( size_t l_point = 0; l_point < size(); l_point ++ )
    {
        at( l_point ).nextStateOut();
    }
}

void EsynetLinkFaultUnit::updateFaultPattern(DataType & pattern)
{
    for ( size_t l_point = 0; l_point < size(); l_point ++ )
    {
        int word = l_point / ATOM_WIDTH_;
        int bit = l_point % ATOM_WIDTH_;

        if ( at( l_point ).currentStateOut() )
        {
            pattern[ word ] |= (0x01ULL << bit);
        }
    }
}

long int EsynetLinkFaultUnit::faultBitCount()
{
	long count = 0;
    for ( size_t l_point = 0; l_point < size(); l_point ++ )
    {
        if ( at( l_point ).currentStateOut() )
        {
            count ++;
        }
    }
    return count;
}

long int EsynetLinkFaultUnit::stateBitCount(long int state)
{
	long count = 0;
    for ( size_t l_point = 0; l_point < size(); l_point ++ )
    {
        if ( at( l_point ).currentState() ==  state )
        {
            count ++;
        }
    }
    return count;
}



ostream& operator<<(ostream& os, const EsynetLinkFaultUnit & sr)
{
    for ( size_t l_point = 0; l_point < sr.size(); l_point ++ )
    {
        if ( !sr.at( l_point ).constLiving() )
        {
            os << "\tPort " << sr.m_port << " bit " << l_point << ": " << 
                sr.at( l_point ) << endl;
        }
    } /* for ( int l_point = 0; l_point < sr.size(); l_point ++ ) */
    return os;
}

