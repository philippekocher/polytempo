/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

#include "Polytempo_EventDispatcher.h"


juce_ImplementSingleton(Polytempo_EventDispatcher);

void Polytempo_EventDispatcher::setBroadcastSender(Polytempo_OSCSender *sender)
{
    oscSender = sender;
}

void Polytempo_EventDispatcher::broadcastEvent(Polytempo_Event *event)
{
    // direct connection
    Polytempo_Scheduler::getInstance()->handleEvent(event, event->getTime());
    
    // network broadcast
    if(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("broadcastSchedulerCommands") &&
       oscSender != nullptr)
    {
        oscSender->broadcastEventAsMessage(event);
    }
}
