#include "Polytempo_EventObserver.h"

#ifdef POLYTEMPO_LIB
#include "../Library/Polytempo_LibEventHandler.h"
#else
#include "Polytempo_EventScheduler.h"
#endif


Polytempo_EventObserver::Polytempo_EventObserver()
{
    // observers register themselves with the subject
#ifdef POLYTEMPO_LIB
    Polytempo_LibEventHandler::getInstance()->registerObserver(this);
#else
    Polytempo_EventScheduler::getInstance()->registerObserver(this);
#endif
}

Polytempo_EventObserver::~Polytempo_EventObserver()
{
#ifndef POLYTEMPO_LIB
    Polytempo_EventScheduler::getInstance()->removeObserver(this);
#endif
}
