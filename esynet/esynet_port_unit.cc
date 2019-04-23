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

#include "esynet_port_unit.h"

/*************************************************
  Function :
    ostream & operator<<(ostream& os, const InputPort& Ri)
  Description :
    print input buffers in unit Ri to ostream os
  Input:
            ostream& os  output stream
    const InputPort& Ri  input unit to print.
*************************************************/
ostream & operator<<(ostream& os, const EsynetInputPort& Ri)
{
//    os << " input_neighbor={";
//    for (int i = 0; i < Ri.m_input_buffer.size(); i ++)
//    {
//        os << "(" << Ri.inputNeighborAddrId( i ) <<"," <<
//            Ri.inputNeighborAddrPort( i ) << ")";
//        /* last item, donot print spacer mark */
//        if (i != Ri.m_input_buffer.size() - 1)
//        {
//            os << ",";
//        } /* else */
//    } /* for (int i = 0; i < sr.physic_ports_; i ++) */
//    os << "}";
    return os;
}

EsynetInputVirtualChannel::EsynetInputVirtualChannel():
    m_input_buffer(),
    m_vc_state(),
    m_routing(),
    m_connection()
{
}

EsynetInputVirtualChannel::EsynetInputVirtualChannel(EsyNetworkCfgPort * port_cfg) :
    m_input_buffer(),
    m_vc_state( VCS_INIT ),
    m_routing(),
    m_connection( VC_NULL )
{
}

EsynetInputPort::EsynetInputPort() :
    m_neighbor_addr(),
    m_input_buffer_size()
{
}

EsynetInputPort::EsynetInputPort(EsyNetworkCfgPort * port_cfg) :
    m_neighbor_addr( VC_NULL ),
    m_input_buffer_size( port_cfg->inputBuffer() )
{
    for ( int i = 0; i < port_cfg->inputVcNumber(); i ++ ) 
    {
		push_back(EsynetInputVirtualChannel(port_cfg));
    }
}

bool EsynetInputPort::isEmpty()
{
    for ( size_t i = 0; i < size(); i ++ )
    {
        if ( at( i ).flitCount() > 0 )
        {
            return false;
        }
    }
    return true;
}


ostream & operator<<(ostream& os, const EsynetOutputPort& Ri)
{
//    /* print output neighbor */
//    os << "output_neighbor={";
//    for (int i = 0; i < Ri.m_output_buffer.size(); i ++)
//    {
//        os << "(" << Ri.outputNeighborAddrId( i )  <<"," <<
//                    Ri.outputNeighborAddrPort( i ) << ")";
//        /* last item, donot print spacer mark */
//        if (i != Ri.m_output_buffer.size() - 1)
//        {
//            os << ",";
//        } /* else */
//    } /* end of  for (int i = 0; i < Ri.physic_ports_; i ++) */
//    os << "}";
    return os;
}

EsynetOutputVirtualChannel::EsynetOutputVirtualChannel() :
    m_vc_assign(),
    m_vc_usage(),
    m_credit_counter()
{
}

EsynetOutputVirtualChannel::EsynetOutputVirtualChannel(EsyNetworkCfgPort * port_cfg) :
    m_vc_assign( VC_NULL ),
    m_vc_usage( VC_FREE ),
    m_credit_counter( port_cfg->inputBuffer() )
{
}

EsynetOutputPort::EsynetOutputPort() :
    m_output_buffer(),
    m_out_add(),
    m_neighbor_addr(),
    m_output_buffer_size(),
    m_flit_on_link( false )
{
}

EsynetOutputPort::EsynetOutputPort(EsyNetworkCfgPort * port_cfg) :
    m_output_buffer(),
    m_out_add(),
    m_neighbor_addr( VC_NULL ),
    m_output_buffer_size( port_cfg->outputBuffer() ),
    m_flit_on_link( false )
{
    for ( int i = 0; i < port_cfg->outputVcNumber(); i ++ ) 
    {
		push_back(EsynetOutputVirtualChannel(port_cfg));
    }
}

bool EsynetOutputPort::isEmpty()
{
	if ( m_flit_on_link )
	{
		return false;
	}
	if ( flitCount() > 0 )
	{
		return false;
	}
    for ( size_t i = 0; i < size(); i ++ )
    {
        if ( at( i ).usage() != VC_FREE )
        {
            return false;
        }
    }
    return true;
}
