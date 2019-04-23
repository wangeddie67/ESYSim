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

#include "esynet_mess_event.h"

EsynetMessEvent::EsynetMessEvent() :
	m_event_time( 0.0 ), m_pipe_time( 0.0 ), m_mess_type( EVG ),
	m_src_id( -1 ), m_src_pc( -1 ), m_src_vc( -1 ),
	m_des_id( -1 ), m_des_pc( -1 ), m_des_vc( -1 ),
	m_flit()
{
}

/*************************************************
  Function : 
    MessEvent::MessEvent( double time, double pipe, EventType mtype, long 0id ):
  Description : 
    Constructs a ROUTER event with specific $pipe$, $id$
  Input :
    double  time   message time
    double  pipe   pipeline cycle
    EventType mtype  message type
        long id     id of router to handle this event, -1 means all router
*************************************************/
EsynetMessEvent EsynetMessEvent::generateRouterMessage(
	double time, double pipe, long id)
{
	EsynetMessEvent e;
	e.m_event_time = time;
	e.m_pipe_time = pipe;
	e.m_mess_type = ROUTER;
	e.m_src_id = id;
	return e;
}

/*************************************************
  Function :
    MessEvent::MessEvent( double time, EventType mtype, long src, long des,
        long ps, long msgno, long vc_s, double packet_start, long ack )
  Description : 
    Constructs a EVG event with $msgno$, $vc_s$ and $ack$.
  Input:
    time_type time   message time
    EventType mtype  message type
         long src    source id
         long des    destination id
         long ps     packet size
         long msgno  message number specific
         long vc_s   source virtual channel
         long ack    ack flag of packet, default is -1
*************************************************/
EsynetMessEvent EsynetMessEvent::generateEvgMessage(
    double time, long src, long des, long pac_size, long msgno, 
    double e2e_start, long flag, long ack
)
{
	EsynetMessEvent e;
	e.m_event_time = time;
	e.m_mess_type = EVG;
	e.m_src_id = src;
	e.m_des_id = des;
	if ( ack < 0 )
	{
		e.m_flit = EsynetFlit(msgno, pac_size, EsynetFlit::FLIT_HEAD, 
			src, des, time, e2e_start, DataType(), flag | 0, -1 );
	}
	else
	{
		e.m_flit = EsynetFlit(msgno, pac_size, EsynetFlit::FLIT_HEAD, 
			src, des, time,
			e2e_start, DataType(), flag | EsynetFlit::FLIT_ACK, ack);
	}
	return e;
}

/*************************************************
  Function : 
    MessEvent::MessEvent( double time, EventType mtype, long src, long des,
        long port, long vc )  
  Description : 
    constructor for CREDIT message
  Input :
    double  time   event time
    EventType mtype  event type
        long  src    source id
        long  des    destination id
        long  phy    destination physical channel
        long  vc     destination virtual channel
*************************************************/
EsynetMessEvent EsynetMessEvent::generateCreditMessage(
    double time, long src, long src_pc, long src_vc, 
    long des, long des_pc, long des_vc, long credit
)
{
	EsynetMessEvent e;
    e.m_event_time = time;
    e.m_mess_type = CREDIT;
    e.m_src_id = src;
    e.m_src_pc = src_pc;
    e.m_src_vc = src_vc;
    e.m_des_id = des;
    e.m_des_pc = des_pc;
    e.m_des_vc = des_vc;
	e.m_flit = EsynetFlit(credit, 0, EsynetFlit::FLIT_HEAD, 0, 0, 0, 0, 
		DataType(), 0, 0 );
	return e;
}


/*************************************************
  Function : 
    MessEvent::MessEvent( double time, EventType mtype, long src, long des,
        long phy, long vc, const EsynetFlit & flit )
  Description : 
    constructor for WIRE message with specific $flit$
  Input :
        double time   event time
        EventType mtype  event type
            long src    source id
            long des    destination id
            long phy    destination physical channel
            long vc     destination virtual channel
    const EsynetFlit & flit   flit
*************************************************/
EsynetMessEvent EsynetMessEvent::generateWireMessage(
    double time, long src, long src_pc, long src_vc, 
	long des, long des_pc, long des_vc, const EsynetFlit & flit
) 
{
	EsynetMessEvent e;
	e.m_event_time = time;
	e.m_mess_type = WIRE;
	e.m_src_id = src;
	e.m_src_pc = src_pc;
	e.m_src_vc = src_vc;
	e.m_des_id = des;
	e.m_des_pc = des_pc;
	e.m_des_vc = des_vc;
	e.m_flit = flit;
	return e;
}

EsynetMessEvent EsynetMessEvent::generateNIReadMessage(double time, long ni)
{
	EsynetMessEvent e;
	e.m_event_time = time;
	e.m_mess_type = NIREAD;
	e.m_src_id = ni;
	return e;
}

/*************************************************
  Function : 
    MessEvent::MessEvent( double time, EventType mtype, long src, long srcport,
        long srcvc, long des, long desport, long desvc, long ps, onst EsynetFlit & 
flit ) 
  Description : 
    Constructs a event with all specific parameter, used by event trace event
  Input:
        double time     message time
        EventType mtype    message type
            long src      source id
            long srcport  source port
            long srcvc    source virual channel
            long des      destination id
            long desport  destination port
            long desvc    destination virtual channel
            long ps       packet size
    const EsynetFlit & flit     flit
*************************************************/
EsynetMessEvent::EsynetMessEvent(
    double time, long mtype, long src, long srcport, long srcvc, 
	long des, long desport, long desvc, const EsynetFlit & flit
) :
    m_event_time( time ),
    m_pipe_time( 0.0 ),
    m_mess_type( mtype ) ,
    m_src_id( src ),
    m_src_pc( srcport ),
    m_src_vc( srcvc ),
    m_des_id( des ),
    m_des_pc( desport ),
    m_des_vc( desvc ),
    m_flit( flit )
{
}

/*************************************************
  Function : 
    MessEvent::MessEvent( const MessEvent & me )
  Description: 
    constructor and copy from exist entity
  Input:
    const MessEvent& me  message event for copying
*************************************************/
EsynetMessEvent::EsynetMessEvent(const EsynetMessEvent & me) :
	m_event_time( me.eventStart() ),
	m_pipe_time( me.pipeTime() ),
	m_mess_type( me.eventType() ),
	m_src_id( me.srcId() ),
	m_src_pc( me.srcPc() ),
	m_src_vc( me.srcVc() ),
	m_des_id( me.desId() ),
	m_des_pc( me.desPc() ),
	m_des_vc( me.desVc() ),
	m_flit( me.flit() )
{
}

/*************************************************
  Function : 
    ostream& operator<<( ostream & os, const MessEvent & sg )
  Description : 
    print message event to ostream os
  Input :
            ostream& os  output stream
    const MessEvent& sg  message event to print
  Return :
    ostream &  output stream
*************************************************/
ostream& operator<<(ostream & os, const EsynetMessEvent & sg)
{
	/* time */
	os << "time=" << sg.eventStart() << " ";
	/* type */
	os << "type=" << (int)sg.eventType() << " ";
	/* source */
	os << "src={Router=" << sg.srcId() << ",Port=" << sg.srcPc() << ",VC=" << 
		sg.srcVc() << "} ";
	/* destination */
	os << "des={Router=" << sg.desId() << ",Port=" << sg.desPc() << ",VC=" << 
		sg.desVc() << "} ";
	/* size and flit */
	os << " " << sg.flit();
	/* return ouput stream */
	return os;
}
