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

#ifndef ESYNET_FLIT_H
#define ESYNET_FLIT_H

/* including head file */
#include "esynet_global.h"

/* including library file */
#include <vector>
#include <iostream>

/* namespace */
using namespace std;
using namespace esynet;

/*************************************************
  Class Name : 
    EsynetFlit
    
  Description : 
    Structure of flit.
    
  Public Types :
    FlitType : enum
    
  Properties :
    m_flit_id          : long
    m_flit_size        : long
    m_flit_type        : FlitType
    m_sor_addr         : long
    m_des_addr         : long

    m_payload_data     : DataType

    m_flit_start_time  : time_type
    m_flit_finish_time : time_type
    m_latency_marked   : bool

    m_fault_pattern    : DataType

    m_ack_packet       : long
    m_drop             : bool
    m_e2e_start_time   : double

    m_bypass           : bool
    m_bypass_id        : vector< long >
    
  Public functions :
    EsynetFlit();
    EsynetFlit( long id, FlitType type, long src,
    long des, double st, double e2e_st,
    const DataType & data, bool marked = false,
    bool ack = false );
    EsynetFlit( long id, long size, FlitType type, long src,
    long des, double st, double e2e_st,
    const DataType & data, bool marked = false,
    bool ack = false );
    EsynetFlit( EsynetFlitt Flit & a );

    long flitId() const
    long flitSize() const
    FlitType flitType() const
    long sorAddr() const
    long desAddr() const
    double startTime() const
    void setStartTime( double a )
    double finishTime() const
    void setFinishTime( double a )
    const DataType & data() const
    bool marked() const
    bool ack() const
    const DataType & faultPattern() const
    void setFaultPattern( DataType pattern )
    bool drop() const
    void setDrop(bool drop = true)
    bool bypass() const
    void setBypass(bool bypass = true)
    const vector< long > & bypassId() const
    void eraseBypassId()
    void setBypassId( long a, long b )
    double e2eStartTime() const
    void setE2EStartTime( double a )
                      
  Related Non-Members : 
    friend ostream& operator<<( ostream& os, const EsynetFlit & ft )
*************************************************/
class EsynetFlit
{
/* public types */
public:
	/* flit type */
	enum FlitType {
		/* Head flit */
		FLIT_HEAD,
		/* Body flit */
		FLIT_BODY,
		/* Tail flit */
		FLIT_TAIL
	};
    
	enum FlitFlag {
		FLIT_ACK  = 0x01,
		FLIT_DROP = 0x02,
		FLIT_FAULTY = 0x20,
		FLIT_MARK = 0x04,
		FLIT_FTLR_BYPASS = 0x08,
		FLIT_TESTING = 0x10,
	};
    
    /* Properties */
private:
	/* head field */
	/* flit id */
	long m_flit_id;
	/* flit size */
	long m_flit_size;
	/* flit type: head, body, tail */
	FlitType m_flit_type;
	/* source address */
	long m_sor_addr;
	/* destination address */
	long m_des_addr;

	long m_flit_flag;

	/* payload field */
	/* payload data */
	DataType m_payload_data;

	/* statistic field */
	/* start time */
	double m_flit_start_time;
	/* end-to-end start time */
	double m_e2e_start_time;

	/* fault injection */
	/* fault pattern, used by fault injection */
	DataType m_fault_pattern;

	/* retransmission */
	/* ack packet, used by retransmission, if it > 0, it means the id of 
		* orignal packets */
	/* or it means it is not ack packet */
	long m_ack_packet;

	/* ftlr field */
	/* bypass path, used by fltr */
	vector< long > m_bypass_id;
/* Public Functions */
public:
	/* constructor */
	EsynetFlit();
	EsynetFlit(long id, long size, FlitType type, long src, long des,
		double st, double e2e_st, const DataType & data, long flag = 0, 
		long ack = -1 );
	/* constructor and copy from exist entity */
	EsynetFlit(const EsynetFlit & a);

	/* functions access to m_flit_id */
	long flitId() const { return m_flit_id; }
	/* functions access to m_flit_size */
	long flitSize() const { return m_flit_size; }
	/* functions access to m_flit_type */
	FlitType flitType() const { return m_flit_type; }
	/* functions access to m_sor_addr */
	long sorAddr() const { return m_sor_addr; }
	/* functions access to m_des_addr */
	long desAddr() const { return m_des_addr; }
	/* functions access to m_flit_start_time */
	double startTime() const { return m_flit_start_time; }
	void setStartTime( double a ) { m_flit_start_time = a; }
	/* functions access to m_payload_data */
	const DataType & data() const { return m_payload_data; }
	/* functions access to m_flit_flag */
	long flitFlag() const { return m_flit_flag; }
	void setFlitFlag( long flag ) { m_flit_flag = flag; }
	/* functions access to FLIT_MARK */
	bool marked() const { return ( m_flit_flag & FLIT_MARK ) == FLIT_MARK; }
	/* functions access to FLIT_ACK */
	bool ack() const { return ( m_flit_flag & FLIT_ACK ) == FLIT_ACK; }
	/* functions access to FLIT_DROP */
	bool drop() const { return ( m_flit_flag & FLIT_DROP ) == FLIT_DROP; }
	void setDrop() { m_flit_flag = m_flit_flag | FLIT_DROP; }
	void clearDrop() { m_flit_flag = m_flit_flag & ( ~FLIT_DROP ); }
	/* functions access to FLIT_FAULTY */
	bool faulty() const { return ( m_flit_flag & FLIT_FAULTY ) == FLIT_FAULTY; }
	void setFaulty() { m_flit_flag = m_flit_flag | FLIT_FAULTY; }
	void clearFaulty() { m_flit_flag = m_flit_flag & ( ~FLIT_FAULTY ); }
	/* functions access to FLIT_FTLR_BYPASS */
	bool bypass() const { return ( m_flit_flag & FLIT_FTLR_BYPASS ) == 
		FLIT_FTLR_BYPASS; }
	void setBypass() { m_flit_flag = m_flit_flag | FLIT_FTLR_BYPASS; }
	void clearBypass() { m_flit_flag = m_flit_flag & ( ~FLIT_FTLR_BYPASS ); }
	/* functions access to FLIT_FTLR_BYPASS */
	bool testFlit() const { return ( m_flit_flag & FLIT_TESTING ) == 
		FLIT_TESTING; }
	void setTestFlit() { m_flit_flag = m_flit_flag | FLIT_TESTING; }
	void clearTestFlit() { m_flit_flag = m_flit_flag & ( ~FLIT_TESTING ); }
	/* functions access to m_ack_packet */
	long ackPacket() const { return m_ack_packet; }
	/* functions access to m_fault_pattern */
	const DataType & faultPattern() const { return m_fault_pattern; }
	void setFaultPattern( DataType pattern ) { m_fault_pattern = pattern; }
	long faultBitCount() const;
	long faultPatternBit(long bit) const;
	void setFaultPatternBit(long bit);
	void clearFaultPatternBit(long bit);
	bool faultPatternNotEmpty() const;
	/* functions access to m_bypass_id */
	const vector< long > & bypassId() const { return m_bypass_id; }
	void setBypassId( long a, long b ) { m_bypass_id.clear(); 
		m_bypass_id.push_back( a ); m_bypass_id.push_back( b ); }
	void eraseBypassId() { m_bypass_id.erase( m_bypass_id.begin() ); }
	/* functions access to m_e2e_start_time */
	double e2eStartTime() const { return m_e2e_start_time; }
	void setE2EStartTime( double a ) { m_e2e_start_time = a; }
	/* functions access to m_bypass_id, counter */
	void eccCounterReset() { m_bypass_id.clear(); m_bypass_id.push_back( 0 ); }
	long eccCounter() { return m_bypass_id[ 0 ]; }
	void eccCounterInc() { m_bypass_id[ 0 ] ++; }

	friend ostream& operator<<(ostream& os, const EsynetFlit & ft);
};

#endif


