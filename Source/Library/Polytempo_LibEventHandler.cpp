#include "Polytempo_LibEventHandler.h"

juce_ImplementSingleton(Polytempo_LibEventHandler)

Polytempo_LibEventHandler::Polytempo_LibEventHandler()
{
}

Polytempo_LibEventHandler::~Polytempo_LibEventHandler()
{
    clearSingletonInstance();
}

void Polytempo_LibEventHandler::handleEvent(Polytempo_Event* e)
{
    for (auto o : observers)
        o->eventNotification(e);

    if (!e->isOwned())
        delete e;
}

void Polytempo_LibEventHandler::registerObserver(Polytempo_EventObserver* observer)
{
    observers.add(observer);
}

void Polytempo_LibEventHandler::removeObserver(Polytempo_EventObserver* observer)
{
    observers.removeAllInstancesOf(observer);
}
