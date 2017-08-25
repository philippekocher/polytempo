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

#ifndef __Polytempo_MidiClick__
#define __Polytempo_MidiClick__

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Scheduler/Polytempo_EventObserver.h"

class Polytempo_MidiClick : public Polytempo_EventObserver
{
public:
    Polytempo_MidiClick();
    ~Polytempo_MidiClick();
    
    juce_DeclareSingleton (Polytempo_MidiClick, false);
    
    void eventNotification(Polytempo_Event *event);
    
    void setDownbeatPitch(int value);
    void setDownbeatVelocity(int value);
    void setBeatPitch(int value);
    void setBeatVelocity(int value);
    void setCuePitch(int value);
    void setCueVelocity(int value);
    void setChannel(int value);
    void setOutputDeviceIndex(int value);

private:
    int downbeatPitch, beatPitch, cuePitch;
    int downbeatVelocity, beatVelocity, cueVelocity;
    int channel, outputDeviceIndex;
    
    ScopedPointer <MidiOutput> midiOutput;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_MidiClick)
};


#endif /* defined(__Polytempo_MidiClick__) */
