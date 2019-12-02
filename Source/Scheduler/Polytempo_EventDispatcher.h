#pragma once

#include "Polytempo_EventScheduler.h"
#include "../Preferences/Polytempo_StoredPreferences.h"

class Polytempo_EventDispatcher
{
public:
	juce_DeclareSingleton(Polytempo_EventDispatcher, false)
	void broadcastEvent(Polytempo_Event *event);
};