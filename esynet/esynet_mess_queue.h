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

#ifndef ESYNET_MESS_QUEUE_H
#define ESYNET_MESS_QUEUE_H

/* including head file */
#include "esy_interdata.h"
#include "esynet_foundation.h"
#include "esynet_mess_event.h"

/* including library file */
#include <set>

/* return if message $a$ is before message $b$ */
bool operator<(const EsynetMessEvent & a, const EsynetMessEvent & b);

/*************************************************
  Class name : 
    MessQueue
  
  Inhert :
    multiset< MessEvent >
    
  Description : 
    Class for message queue
    
  Properties :
    m_current_time : double
    m_mess_counter : long
    m_traceout     : ODataFileStream< EventtraceItem > *
    
  Static Properties :
    mp_global_pointer : static MessQueue *
    
  Public functions :
             MessQueue( double start_time );
             ~MessQueue()
    double currentTime() const
        long messCounter() const
        void addMessage( const MessEvent & x )
        void insertMsg( int src, int dest, long long int sim_cycle, long packetSize,
             long long int msgNo, long long addr, long vc );
        void simulator( long long int sim_cycle );
        bool openEventTrace();
        bool closeEventTrace();
  Static Public Members
    static const MessQueue & globalPointerConst()
          static MessQueue & globalPointer()
  Related Non-Members
    ostream & operator<<( ostream& os, const MessQueue & sgq );
*************************************************/
class EsynetMessQueue : public multiset< EsynetMessEvent >
{
/* Properties */
private:
	EsynetConfig * mp_argument_cfg;
    /* current time */
    double m_current_time;
    /* number of message injected */
    long m_mess_counter;
    /* pointer to interface with event trace file */
    EsyDataFileOStream< EsyDataItemEventtrace > * mp_eventtrace;
	EsyDataFileOStream< EsyDataItemBenchmark > * mp_outputtrace;

    /* simulation platform */
	EsynetSimBaseUnit * mp_sim_platform;

/* Public Functions */
public:
    /* constructor */
	EsynetMessQueue(double start_time, EsynetSimBaseUnit * sim_platform,
		EsynetConfig * argument_cfg);

    /* function to operate message queue */
    /* insert message $x$ at the end of queue */
	void addMessage(const EsynetMessEvent & x) { m_mess_counter++; insert(x); }

    /* insert message, called by Simplescalar */
    void insertMsg( int src, int dest, long long int sim_cycle, long packetSize,
        long int msgNo, long long addr, long vc );
    /* simulator function, headle event before sim_cycle */
    void simulator( long long int sim_cycle );

    /* event trace functions */
    /* open interface to event trace */
    bool openEventTrace();
    /* close interface to event trace */
    bool closeEventTrace();
    
    /* output trace functions */
    /* open interface to output trace */
    bool openOutputTrace();
    /* close interface to output trace */
    bool closeOutputTrace();

	/* empty check */
    bool linkEmptyCheck( long id, long port = -1 ) const;

    /* print message queue sgq to stream os */
	friend ostream & operator<<(ostream& os, const EsynetMessQueue & sgq);
};

#endif
