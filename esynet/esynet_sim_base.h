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

#ifndef ESYNET_SIM_BASE_H
#define ESYNET_SIM_BASE_H

#include "esynet_mess_event.h"

#include <vector>

class EsynetSimBaseUnit
{
protected:
    double m_current_time;
	vector< EsynetMessEvent > m_event_queue;
    
public:
	EsynetSimBaseUnit() :
        m_current_time( 0.0 ), m_event_queue()
    {}
    
	const vector< EsynetMessEvent > & eventQueue() { return m_event_queue; }
    void clearEventQueue() { m_event_queue.clear(); }
    
    void setTime( double time ) { m_current_time = time; }
	void addEvent(const EsynetMessEvent & event)
        { m_event_queue.push_back( event ); }
    
public:
	virtual void eventHandler(double time, const EsynetMessEvent & mess)
    {
        m_current_time = time;
        return;
    }
};

#endif // BASICSIMCOMPONENT_H
