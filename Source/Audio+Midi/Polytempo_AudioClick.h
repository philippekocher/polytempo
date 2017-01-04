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

#ifndef __Polytempo_AudioClick__
#define __Polytempo_AudioClick__

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Scheduler/Polytempo_EventObserver.h"

class SynthAudioSource;

class Polytempo_AudioClick  : public Polytempo_EventObserver
{
public:
    Polytempo_AudioClick();
    ~Polytempo_AudioClick();
   
    juce_DeclareSingleton(Polytempo_AudioClick, false);

    void eventNotification(Polytempo_Event *event);
    
    int downbeatPitch, beatPitch, cuePitch;
    float downbeatVolume, beatVolume, cueVolume;

private:
    MidiKeyboardState keyboardState;
    AudioDeviceManager& audioDeviceManager;
    
    ScopedPointer <AudioSourcePlayer> audioSourcePlayer;
    ScopedPointer <SynthAudioSource> synthAudioSource;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_AudioClick)
};


#endif /* defined(__Polytempo_AudioClick__) */
