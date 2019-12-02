#ifndef __Polytempo_EventObserver__
#define __Polytempo_EventObserver__

#include "Polytempo_Event.h"

class Polytempo_EventObserver
{
public:
    Polytempo_EventObserver();
    virtual ~Polytempo_EventObserver();
    virtual void eventNotification(Polytempo_Event *event) = 0;
};


#endif // __Polytempo_EventObserver__ 
