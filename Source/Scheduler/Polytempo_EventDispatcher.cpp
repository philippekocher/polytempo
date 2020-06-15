#include "Polytempo_EventDispatcher.h"
#include "../Network/Polytempo_TimeProvider.h"
#include "../Network/Polytempo_InterprocessCommunication.h"

juce_ImplementSingleton(Polytempo_EventDispatcher);

void Polytempo_EventDispatcher::broadcastEvent(Polytempo_Event* event)
{
#ifdef POLYTEMPO_NETWORK
    // network broadcast
    if (Polytempo_TimeProvider::getInstance()->isMaster())
    {
        // set sync time
        event->setSyncTime(Polytempo_TimeProvider::getInstance()->getDelaySafeTimestamp());
        Polytempo_InterprocessCommunication::getInstance()->notifyAllClients(event->getXml());
    }
#endif

    // direct connection
    Polytempo_EventScheduler::getInstance()->scheduleEvent(event); // the scheduler deletes the event
}
