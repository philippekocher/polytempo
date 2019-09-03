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

#ifndef __Polytempo_OSCListener__
#define __Polytempo_OSCListener__

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Scheduler/Polytempo_Event.h"

class Polytempo_OSCListener : OSCReceiver, OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
    Polytempo_OSCListener(int port);
    ~Polytempo_OSCListener();

private:
	void oscMessageReceived(const OSCMessage& message) override;
	Polytempo_Event* oscToEvent(const OSCMessage& message, String addressPattern);
	int m_Port;
    
    ScopedPointer < OSCReceiver > oscReceiver;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_OSCListener);
};

#endif // __Polytempo_OSCListener__
