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

#ifndef __Polytempo_AudioDeviceManager__
#define __Polytempo_AudioDeviceManager__


#include "../Preferences/Polytempo_StoredPreferences.h"


static AudioDeviceManager* sharedAudioDeviceManager;

class Polytempo_AudioDeviceManager
{
public:
    static AudioDeviceManager& getSharedAudioDeviceManager()
    {
        if(sharedAudioDeviceManager == nullptr)
        {
            sharedAudioDeviceManager = new AudioDeviceManager();
            XmlElement *audioDeviceManagerState = Polytempo_StoredPreferences::getInstance()->getProps().getXmlValue("audioDevice");
            sharedAudioDeviceManager->initialise(0, 2,
                                                 nullptr,
                                                 true, String::empty, 0);
            delete audioDeviceManagerState;
        }
    
        return *sharedAudioDeviceManager;
    }

    static void clearSharedAudioDeviceManager()
    {
        delete sharedAudioDeviceManager;
    }
};


#endif  // __Polytempo_AudioDeviceManager__
