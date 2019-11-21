
#include <cstdint>
#include "esynet_sim.h"

#ifndef ESYNET_ESYNET_EVENT_H_
#define ESYNET_ESYNET_EVENT_H_

class EsynetEventClock
{
private:
    Esynet* mp_esynet;
public:
    EsynetEventClock( Esynet* esynet )
        : mp_esynet( esynet )
    {}

    Esynet* esynet() const { return mp_esynet; }
};

class EsynetEventPacketInject
{
private:
    Esynet* mp_esynet;
    uint32_t m_src_ni;
    uint32_t m_dst_ni;
    uint32_t m_length;

public:
    EsynetEventPacketInject( Esynet* esynet,
                             uint32_t src_ni,
                             uint32_t dst_ni,
                             uint32_t length
                           )
        : mp_esynet( esynet )
        , m_src_ni( src_ni )
        , m_dst_ni( dst_ni )
        , m_length( length )
    {}

    Esynet* esynet() const { return mp_esynet; }
    uint32_t srcNi() const { return m_src_ni; }
    uint32_t dstNi() const { return m_dst_ni; }
    uint32_t length() const { return m_length; }
};

class EsynetEvent : public EsyEvent
{
public:
    /**
     * @brief Construct an empty instance.
     */
    EsynetEvent()
        : EsyEvent()
    {}
    /**
     * @brief Construct an instance with all field specified.
     * @param time event time.
     * @param type event type.
     * @param payload pointer to event payload.
     */
    EsynetEvent( uint64_t time, uint16_t type, void* payload )
        : EsyEvent( time, type, payload )
    {}

    ~EsynetEvent()
    {
        if ( mp_payload != NULL )
        {
            if ( m_type == Esynet::EVENT_CYCLE )
            {
                delete (EsynetEventClock*)mp_payload;
            }
            else if ( m_type == Esynet::EVENT_PAC_INJECT )
            {
                delete (EsynetEventPacketInject*)mp_payload;
            }
        }
    }
};


void esynetEventFunClock( EsyEventQueue* queue, void* payload );
void esynetEventFunPacketInject( EsyEventQueue* queue, void* payload );

#endif
