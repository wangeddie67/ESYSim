/*
 Copyright @ UESTC

 ESY-series Many-core System-on-chip Simulation Environment is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.

 ESY-series Many-core System-on-chip Simulation Environment is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/.
 */

#ifndef INTERFACE_EVENT_TYPE_H
#define INTERFACE_EVENT_TYPE_H

enum EsyEventType 
{
    EVENT_TRACE_START = 20,  /**< Start point of event for VisualNoC */

    ET_FLIT_RECEIVE = (EVENT_TRACE_START + 0), /**< Router receive a flit */
    ET_FLIT_SWITCH = (EVENT_TRACE_START + 1), /**< Router switch a flit */
    ET_FLIT_SEND = (EVENT_TRACE_START + 3), /**< Router send a flit out */
    ET_PACKET_INJECT = (EVENT_TRACE_START + 4), /**< A packet is injected */
    ET_PACKET_ACCEPT = (EVENT_TRACE_START + 5), /**< A packet is accepted */

    ET_ROUTING = (EVENT_TRACE_START + 10),
    ET_ROUTING_END = (EVENT_TRACE_START + 11),
    ET_CROUTING = (EVENT_TRACE_START + 12),
    ET_CROUTING_END = (EVENT_TRACE_START + 13),
    ET_ASSIGN = (EVENT_TRACE_START + 14),
    ET_ASSIGN_END = (EVENT_TRACE_START + 15),

    ET_VC_STATE = (EVENT_TRACE_START + 20),
    ET_VC_ARBITRATION = (EVENT_TRACE_START + 21),
    ET_VC_SHARE = (EVENT_TRACE_START + 22),
    ET_VC_USAGE = (EVENT_TRACE_START + 23),

    ET_SW_ARBITRATION = (EVENT_TRACE_START + 30),

    ET_FT_INJECTION = (EVENT_TRACE_START + 40), /**< fault injection */
    ET_FT_RECOVERY = (EVENT_TRACE_START + 41),  /**< fault recovery */
    ET_FT_ABANDON = (EVENT_TRACE_START + 42),   /**< abondon faulty component */
    ET_FT_ABANDON_END = (EVENT_TRACE_START + 43), /**< end of abondon faulty component */

    ET_PE_CHANGE = (EVENT_TRACE_START + 50), /**< Change PE status */
    ET_APP_REQUEST = (EVENT_TRACE_START + 60), /**< Application request */
    ET_APP_ENTER = (EVENT_TRACE_START + 61),   /**< Application enter */
    ET_APP_EXIT = (EVENT_TRACE_START + 62),    /**< Application exist */
};

#endif
