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

#ifndef __Polytempo_MidiInput__
#define __Polytempo_MidiInput__


#include "Polytempo_AudioDeviceManager.h"


class Polytempo_MidiInput : public MidiInputCallback

{
public:
    Polytempo_MidiInput()
//    :audioDeviceManager(Polytempo_AudioDeviceManager::getSharedAudioDeviceManager())
    {
//        const StringArray list (MidiInput::getDevices());
//        const String newInput (list[1]);
//        
//        if (! audioDeviceManager.isMidiInputEnabled (newInput))
//            audioDeviceManager.setMidiInputEnabled (newInput, true);
//        
//        audioDeviceManager.addMidiInputCallback (newInput, this);
    }
    ~Polytempo_MidiInput()
    {
//        audioDeviceManager.removeMidiInputCallback (MidiInput::getDevices()[1], this);
    }
    
    void handleIncomingMidiMessage(MidiInput*, const MidiMessage& message)
    {
        DBG(message.getDescription());
    }
    
private:
//    AudioDeviceManager& audioDeviceManager;
    
};



#endif  // __Polytempo_MidiInput__
