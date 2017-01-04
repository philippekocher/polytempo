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

#include "Polytempo_DefaultPreferences.h"


Polytempo_DefaultPreferences::Polytempo_DefaultPreferences()
{    
    // composer
    setValue("zoomX", 50);
    setValue("timeMapZoomY", 20);
    setValue("tempoMapZoomY", 20);
    setValue("mainWindowContent", "400 0 0");
    
    // network
    setValue("broadcastSchedulerCommands", 1);
    setValue("showVisualMetro", 1);
    setValue("stripWidth", 30);
    setValue("stripNormalColour", "ff000000");
    setValue("stripCueColour", "ffff0000");
    setValue("stripBackgroundColour", "ffeeeeee");
    setValue("stripFrameColour", "ff000000");    
    setValue("showAuxiliaryView", 1);
    setValue("zoom", 0.4);
    
    // common
    setValue("midiDownbeatPitch", 100);
    setValue("midiDownbeatVelocity", 127);
    setValue("midiBeatPitch", 95);
    setValue("midiBeatVelocity", 127);
    setValue("midiCuePitch", 90);
    setValue("midiCueVelocity", 127);
    setValue("midiChannel", 1);
    
    setValue("audioDownbeatPitch", 100);
    setValue("audioDownbeatVolume", 1.0);
    setValue("audioBeatPitch", 95);
    setValue("audioBeatVolume", 1.0);
    setValue("audioCuePitch", 90);
    setValue("audioCueVolume", 1.0);
}

Polytempo_DefaultPreferences::~Polytempo_DefaultPreferences()
{
    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_DefaultPreferences);