/*
  ==============================================================================

    Polytempo_LibEventObserver.cpp
    Created: 15 Sep 2021 1:14:05pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_LibEventObserver.h"

void Polytempo_LibEventObserver::eventNotification(Polytempo_Event* event)
{
    String eventString = event->getTypeString(); // TODO!!! serialize
    sendActionMessage(eventString);
}
