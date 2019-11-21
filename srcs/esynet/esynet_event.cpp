
#include "esynet_event.h"

void
esynetEventFunClock( EsyEventQueue* queue,
                     void* payload )
{
    EsynetEventClock* clock_event = (EsynetEventClock*)payload;
    clock_event->esynet()->handleClockEvent( queue );
}

void
esynetEventFunPacketInject( EsyEventQueue* queue,
                            void* payload )
{
    EsynetEventPacketInject* pac_event = (EsynetEventPacketInject*)payload;
    pac_event->esynet()->handlePacketInjectEvent( queue,
        pac_event->srcNi(), pac_event->dstNi(), pac_event->length() );
}

