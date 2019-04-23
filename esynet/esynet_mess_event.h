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

#ifndef ESYNET_MESS_EVENT_H
#define ESYNET_MESS_EVENT_H

/* including head file */
#include "esy_eventtype.h"
#include "esynet_flit.h"

/* including library file */
#include <vector>
#include <iostream>

/*************************************************
  Class name : 
    MessEvent

  Description : 
    Structure for message event

  Properties :
    m_event_time   : double
    m_pipe_time    : double
    m_mess_type    : mess_type
    m_src_id       : long
    m_src_pc       : long
    m_src_vc       : long
    m_des_id       : long
    m_des_pc       : long
    m_des_vc       : long
    m_packet_size  : long
    m_flit         : EsynetFlit
    m_packet_start : double
    m_ack          : long
  Public functions
    MessEvent(double time, double pipe, MessType mtype)
    MessEvent(double time, double pipe, MessType mtype, long id)
    MessEvent(double time, MessType mtype, long src, long des, long ps)
    MessEvent(double time, MessType mtype, long src,
        long des, long ps, long msgno, long vc_s, bool ack = false)
    MessEvent(double time, MessType mtype, long src, long dst, long port, 
        long vc)
    MessEvent(double time, MessType mtype, long src, long dst, long phy, 
        long vc, const EsynetFlit & flit)
    MessEvent(double time, MessType mtype,
        const EsynetFlit & flit)
    MessEvent(double time, MessType mtype, long src, long srcport, long srcvc, 
        long dst, long dstport, long dstvc, long ps, const EsynetFlit & flit )
    MessEvent( const MessEvent& me )
    double eventStart() const
    double pipeTime() const
    MessType eventType() const
    long srcId() const
    long desId() const
    long packetSize() const
    long desPc() const
    long desVc() const
    long srcPc() const
    long srcVc() const
    const EsynetFlit & flit() const
    long ack() const
    double packetStart() const
  Related Non-Members
    ostream & operator<<( ostream& os, const MessEvent & sg );
*************************************************/
class EsynetMessEvent
{
/* Public Type */
public:
	enum EventType {
		/* esynet event */
		/* event generator */
		EVG = 0,
		/* router pipeline event */
		ROUTER = 1,
		/* wire deliver event */
		WIRE = 2,
		/* credit event */
		CREDIT = 3,
		/* Ni Read event */
		NIREAD = 4,
	};
/* Properties */
private:
	/* event time */
	double m_event_time;
	/* pipeline time for different frequency field, only used by ROUTER event */
	double m_pipe_time;
	/* message type */
	long m_mess_type;
	/* source id */
	long m_src_id;
	/* source physical channel */
	long m_src_pc;
	/* source virtual channel */
	long m_src_vc;
	/* destination id */
	long m_des_id;
	/* destination physical channel */
	long m_des_pc;
	/* destination virtual channel */
	long m_des_vc;
	/* flit entity */
	EsynetFlit m_flit;
public:
	/* function to access time_ */
	double eventStart() const { return m_event_time; }
	/* function to access time_ */
	double pipeTime() const { return m_pipe_time; }
	/* function to access time_ */
	EventType eventType() const { return (EventType)m_mess_type; }
	long type() const { return m_mess_type; }
	/* function to access time_ */
	long srcId() const { return m_src_id; }
	/* function to access time_ */
	long desId() const { return m_des_id; }
	/* function to access time_ */
	long desPc() const { return m_des_pc; }
	/* function to access time_ */
	long desVc() const { return m_des_vc; }
	/* function to access time_ */
	long srcPc() const { return m_src_pc; }
	/* function to access time_ */
	long srcVc() const { return m_src_vc; }
	/* function to access time_ */
	const EsynetFlit & flit() const { return m_flit; }
	/* constructor for EVG message */
	EsynetMessEvent();
	EsynetMessEvent(const EsynetMessEvent& me);
	/* constructor with full parameter, used by event trace */
	EsynetMessEvent(double time, long mtype, long src, long srcport,
		long srcvc, long dst, long dstport, long dstvc, 
		const EsynetFlit & flit);
	/* constructor and copy from exist entity */

	static EsynetMessEvent generateEvgMessage(
		double time, long src, long des, long pac_size, long msgno,
		double e2e_start, long flag = 0, long ack = -1 );
	/* constructor for ROUTER message */
	static EsynetMessEvent generateRouterMessage(
		double time, double pipe, long id = -1 );
	/* constructor for CREDIT message */
	static EsynetMessEvent generateCreditMessage(
		double time, long src, long src_pc, long src_vc, 
		long des, long des_pc, long des_vc, long credit );
	/* constructor for WIRE message */
	static EsynetMessEvent generateWireMessage(
		double time, long src, long src_pc, long src_vc, 
		long des, long des_pc, long des_vc, const EsynetFlit & flit);
	static EsynetMessEvent generateNIReadMessage(
		double time, long ni);
/* Related Non-Members */
public:
	/* print event to stream */
	friend ostream & operator<<(ostream& os, const EsynetMessEvent & sg);
};
#endif
