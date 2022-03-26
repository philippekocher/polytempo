#include "Polytempo_EventObserver.h"

#include "Polytempo_EventScheduler.h"


Polytempo_EventObserver::Polytempo_EventObserver()
{
    // observers register themselves with the subject
    Polytempo_EventScheduler::getInstance()->registerObserver(this);
}

Polytempo_EventObserver::~Polytempo_EventObserver()
{
    if(Polytempo_EventScheduler::getInstanceWithoutCreating()!= nullptr)
        Polytempo_EventScheduler::getInstance()->removeObserver(this);
}
