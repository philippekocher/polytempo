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


#ifndef __Polytempo_Event__
#define __Polytempo_Event__

//#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Misc/Rational.h"

enum Polytempo_EventType
{
    eventType_None = 0,
        
    eventType_Open,
    
    eventType_Tick,
    eventType_Beat,
    eventType_Marker,
    
    eventType_Start,
    eventType_Stop,
    eventType_Pause,
    
    eventType_GotoMarker,
    eventType_GotoLocator,
    eventType_TempoFactor,
    
    eventType_LoadImage,
    eventType_AddRegion,
    eventType_AddSection,
    
    eventType_Image,
    eventType_Text,
    eventType_Progressbar,
    
    eventType_AddSender,
    eventType_Osc,
    
    eventType_Settings,

    eventType_DeleteAll,  // delete data
    eventType_ClearAll,   // clear visuals
    eventType_Ready,
};

/* ------------------------------------------------------
   type strings as used in human readable scores
   and in OSC communication */

#define eventTypeString_Open            "open"

#define eventTypeString_Tick            "tick"
#define eventTypeString_Beat            "beat"
#define eventTypeString_Marker          "marker"

#define eventTypeString_Start           "start"
#define eventTypeString_Stop            "stop"
#define eventTypeString_Pause           "pause"

#define eventTypeString_GotoMarker      "gotoMarker"
#define eventTypeString_GotoLocator     "gotoLocator"
#define eventTypeString_TempoFactor     "tempoFactor"

#define eventTypeString_LoadImage       "loadImage"
#define eventTypeString_AddRegion       "addRegion"
#define eventTypeString_AddSection      "addSection"

#define eventTypeString_Image           "image"
#define eventTypeString_Text            "text"
#define eventTypeString_Progressbar     "progressbar"

#define eventTypeString_AddSender       "addSender"
#define eventTypeString_Osc             "osc"

#define eventTypeString_Settings        "settings"

#define eventTypeString_ClearAll        "clearAll"
//#define eventTypeString_DeleteAll       "deleteAll"  // only local
//#define eventTypeString_Ready           "ready"  // only local


/* ------------------------------------------------------
   property strings as used in human readable scores
   and in OSC communication */


#define eventPropertyString_Duration    "duration"
#define eventPropertyString_ImageID     "imageID"
#define eventPropertyString_RegionID    "regionID"
#define eventPropertyString_SectionID   "sectionID"
#define eventPropertyString_URL         "url"
#define eventPropertyString_Rect        "rect"
#define eventPropertyString_Pattern     "pattern"
#define eventPropertyString_Cue         "cue"


class Polytempo_Event
{
public:
    Polytempo_Event(Polytempo_EventType ty = eventType_None);
    Polytempo_Event(const Polytempo_Event&);
    ~Polytempo_Event();
    
    // factories
    static Polytempo_Event* makeEvent(String typeString);   // initialize from osc message
    static Polytempo_Event* makeEvent(Polytempo_EventType type);
    static Polytempo_Event* makeEvent(Polytempo_EventType type, var value);
    

    // accessors
    void setType(String);
    Polytempo_EventType getType();
    String getTypeString();
    
    void  setTime(float t);
    float getTime();
    int   getMilisecondTime();
    
    void setPosition(Rational pos);
    Rational getPosition();
    
    void setProperty(String key, var value);
    var  getProperty(String key);
    bool hasProperty(String key);
    NamedValueSet* getProperties();
    
    // event to message conversion
	String getOscAddressFromType();
    Array<var> getOscMessageFromParameters();
    static Polytempo_Event* makeEvent(String address, Array<var> values);


private:
    Polytempo_EventType type;
    float               time;
    Rational            position;

    NamedValueSet *properties;
};



#endif // __Polytempo_Event__
