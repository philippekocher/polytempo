#include "Polytempo_LibEventHandler.h"

juce_ImplementSingleton(Polytempo_LibEventHandler)

void Polytempo_LibEventHandler::handleEvent(Polytempo_Event* e)
{
}

void Polytempo_LibEventHandler::registerObserver(Polytempo_EventObserver* observer)
{
    observers.add(observer);
}

void Polytempo_LibEventHandler::removeObserver(Polytempo_EventObserver* observer)
{
    observers.removeAllInstancesOf(observer);
}
