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

#include "esynet_flit.h"

/*************************************************
  Function : 
    ostream & operator<<( ostream & os, const EsynetFlit & ft )
  Description : 
    print flit $ft$ to ostream $os$. without '\n' at the end.
  Input :
       ostream & os  output stream
    const EsynetFlit & ft  flit to print
  Return :
    ostream &  output stream
*************************************************/
ostream & operator<<(ostream & os, const EsynetFlit & ft)
{
    /* flit id and size */ 
    os << "id=" << ft.flitId() << " size=" << ft.flitSize() << " ";
    /* flit type */
    switch ( ft.flitType() )
    {
	case EsynetFlit::FLIT_HEAD: os << "FLIT_HEAD "; break;
	case EsynetFlit::FLIT_BODY: os << "FLIT_BODY "; break;
	case EsynetFlit::FLIT_TAIL: os << "FLIT_TAIL "; break;
    } /* switch ( ft.flitType() ) */
    /* source address and destination address */
    os << "addr=" << ft.sorAddr() << ">" << ft.desAddr() << " ";
    /* payload data */
    os << "data={";
    for ( size_t i = 0; i < ft.data().size(); i ++ )
    {
        os << ft.data()[ i ] << ";";
    } /* for ( int i = 0; i < ft.data().size(); i ++ ) */
    os << "} ";
    /* ack flag */
    if ( ft.ack() )
    {
        os << "ACK " << endl;
    } /* if ( ft.ack() ) */
    /* drop flag */
    if ( ft.drop() )
    {
        os << "DROP " << endl;
    } /* if ( ft.drop() ) */
    /* fault pattern */
    os << "fault={";
    for ( size_t i = 0; i < ft.faultPattern().size(); i ++ )
    {
        os << ft.faultPattern()[ i ] << ";";
    } /* for ( int i = 0; i < ft.faultPattern().size(); i ++ ) */
    os << "} ";
    /* start time and finish time */
    os << "start=" << ft.startTime() << " ";
    /* end-to-end latency delay */
    os << "e2e=" << ft.e2eStartTime() << " ";
    /* return output stream */
    return os;
}

/*************************************************
  Function : 
    EsynetFlit::EsynetFlit()
  Description : 
    Constructs a flit with default value or empty.
*************************************************/
EsynetFlit::EsynetFlit() :
    m_flit_id( 0 ),
    m_flit_size( 1 ),
    m_flit_type( FLIT_HEAD ),
    m_sor_addr( 0 ),
    m_des_addr( 0 ),

    m_payload_data(),

    m_flit_start_time( 0 ),
    m_e2e_start_time( 0 ),
        
    m_fault_pattern(),

    m_ack_packet( -1 ),

    m_bypass_id(),
    
    m_flit_flag( 0 )
{
}

/*************************************************
  Function : 
    EsynetFlit::EsynetFlit( long id, long size, FlitType type, long src, long des, 
        double st, const DataType & data, bool mark, long ack )
  Description : 
    Constructs a flit with specific value of field.
  Input :
                 long     id  flit id
             FlitType   type  flit type
                 long    src  source address
                 long    des  destination address
             double     st  start time
             double e2e_st  end-to-end start time
     const DataType &   data  payload data
                 bool   mark  stastics mark flag
                 long    ack  ack flag
*************************************************/
EsynetFlit::EsynetFlit(long id, long size, FlitType type, long src, long des, double st,
    double e2e_st, const DataType & data, long flag, long ack ) :
    m_flit_id( id ),
    m_flit_size( size ),
    m_flit_type( type ),
    m_sor_addr( src ),
    m_des_addr( des ),

    m_payload_data( data ),

    m_flit_start_time( st ),
    m_e2e_start_time( e2e_st ),
        
    m_fault_pattern( data.size(), 0 ),

    m_ack_packet( ack ),

    m_bypass_id(),

    m_flit_flag( flag )
{
}

/*************************************************
  Function : 
    EsynetFlit::EsynetFlit( const EsynetFlit & a )
  Description : Constructs a flit as same as flit $a$.
  Input :
    const EsynetFlit & a  flit to copy
*************************************************/
EsynetFlit::EsynetFlit(const EsynetFlit & a) :
    m_flit_id( a.flitId() ),
    m_flit_size( a.flitSize() ),
    m_flit_type( a.flitType() ),
    m_sor_addr( a.sorAddr() ),
    m_des_addr( a.desAddr() ),

    m_payload_data( a.data() ),

    m_flit_start_time( a.startTime() ),
    m_e2e_start_time( a.e2eStartTime() ),
        
    m_fault_pattern( a.faultPattern() ),

    m_ack_packet( a.ackPacket() ),

    m_bypass_id( a.bypassId() ),
    
    m_flit_flag( a.flitFlag() )
{
}

long EsynetFlit::faultBitCount() const
{
    long faultbit = 0;
    /* loop all group and bit */
    for (size_t word = 0; word < m_fault_pattern.size(); word ++)
    {
        /* count faults in a group */
        for (int bit = 0; bit < ATOM_WIDTH_; bit ++)
        {
            long bitvalue = m_fault_pattern[ word ] & (0x01ULL << bit);
            /* if bitvalue is 1, increase the number of fault */
            if (bitvalue > 0)
            {
                faultbit ++;
            } /* end of  if (bitvalue) */
        } /* end of  for (int gbit = 0; gbit < ecc_word_width_; gbit */
    } /* end of  for (int group = 0; group < totalgroup; group ++) */

    return faultbit;
}

long EsynetFlit::faultPatternBit(long bit) const
{
	long byteid = bit / ATOM_WIDTH_;
	long bitid = bit % ATOM_WIDTH_;
	long bitvalue = m_fault_pattern[byteid] & (0x01ULL << bitid);
}

void EsynetFlit::setFaultPatternBit(long bit)
{
	long byteid = bit / ATOM_WIDTH_;
	long bitid = bit % ATOM_WIDTH_;
	m_fault_pattern[byteid] = 
		m_fault_pattern[byteid] | (0x01ULL << bitid);
}

void EsynetFlit::clearFaultPatternBit(long bit)
{
	long byteid = bit / ATOM_WIDTH_;
	long bitid = bit % ATOM_WIDTH_;
	m_fault_pattern[byteid] = 
		m_fault_pattern[byteid] & (~(0x01ULL << bitid));
}

bool EsynetFlit::faultPatternNotEmpty() const
{
	for (int i = 0; i < m_fault_pattern.size(); i ++)
	{
		if (m_fault_pattern[i] > 0)
		{
			return true;
		}
	}
	return false;
}
