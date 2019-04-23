#ifndef EVENTTYPEDEFINE_H
#define EVENTTYPEDEFINE_H

#define ET_TYPE_EVG	0
#define ET_TYPE_ROUTER	1
#define ET_TYPE_WIRE	2
#define ET_TYPE_CREDIT	3

#define EVENT_TRACE_START	20
#define ITC_FLIT_RECEIVE	(EVENT_TRACE_START + 0)
#define ITC_FLIT_SWITCH		(EVENT_TRACE_START + 1)
#define ITC_FLIT_ACCEPT		(EVENT_TRACE_START + 2)
#define ITC_FLIT_SEND		(EVENT_TRACE_START + 3)

#define ITC_ROUTING		(EVENT_TRACE_START + 10)
#define ITC_ROUTING_END		(EVENT_TRACE_START + 11)
#define ITC_CROUTING		(EVENT_TRACE_START + 12)
#define ITC_CROUTING_END	(EVENT_TRACE_START + 13)
#define ITC_ASSIGN		(EVENT_TRACE_START + 14)
#define ITC_ASSIGN_END		(EVENT_TRACE_START + 15)

#define ITC_VC_STATE		(EVENT_TRACE_START + 20)
#define ITC_VC_ARBITRATION	(EVENT_TRACE_START + 21)
#define ITC_VC_SHARE		(EVENT_TRACE_START + 22)
#define ITC_VC_USAGE		(EVENT_TRACE_START + 23)

#define ITC_SW_ARBITRATION	(EVENT_TRACE_START + 30)

#define ET_COLUMN		16
#define ET_CYCLE		0
#define ET_TYPE			1
#define ET_SRC_X		2
#define ET_SRC_Y		3
#define ET_SRC_PHY		4
#define ET_SRC_VC		5
#define ET_DST_X		6
#define ET_DST_Y		7
#define ET_DST_PHY		8
#define ET_DST_VC		9
#define ET_PACKET_SIZE		10
#define ET_FLIT_ID		11
#define ET_FLIT_SIZE		12
#define ET_FLIT_TYPE		13
#define ET_FLIT_SRC_X		14
#define ET_FLIT_SRC_Y		15
#define ET_FLIT_DST_X		16
#define ET_FLIT_DST_Y		17
#define ET_FLIT_START_CYCLE	18
#define ET_FLIT_LATENCY		19

#endif // EVENTTYPEDEFINE_H