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

#include "../Misc/Rational.h"


enum Polytempo_EventType
{
    eventType_None = 0,
        
    eventType_Open,
    
    eventType_Tick,
    eventType_Beat,
    eventType_Marker, // 4
    
    eventType_Start,
    eventType_Stop,
    eventType_Pause,
    
    eventType_GotoMarker,
    eventType_GotoTime,  // 9
    eventType_TempoFactor,
    
    eventType_LoadImage,
    eventType_AddRegion,
    eventType_AddSection,
    
    eventType_Image,  // 14
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
#define eventTypeString_GotoTime        "gotoTime"
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


#define eventPropertyString_Value       "value"
#define eventPropertyString_TimeTag     "timeTag"
#define eventPropertyString_Time        "time"
#define eventPropertyString_Defer       "defer"
#define eventPropertyString_Duration    "duration"
#define eventPropertyString_ImageID     "imageID"
#define eventPropertyString_RegionID    "regionID"
#define eventPropertyString_MaxZoom     "maxZoom"
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
    
    /* factories
     --------------------------------------- */
    static Polytempo_Event* makeEvent(String typeString);
    static Polytempo_Event* makeEvent(Polytempo_EventType type);
    static Polytempo_Event* makeEvent(Polytempo_EventType type, var value);
    static Polytempo_Event* makeEvent(String oscAddress, Array<var> values);

    /* event to OSC conversion
     --------------------------------------- */

    String getOscAddressFromType();
    Array<var> getOscMessageFromProperties();
    
    /* accessors
     --------------------------------------- */
    void setType(String);
    Polytempo_EventType getType();
    String getTypeString();
    
    void setValue(var val);
    var  getValue();
    
    void setTime(int t);
    int  getTime();
    
    void  setSyncTime(int t);
    int   getSyncTime();

    void setPosition(Rational pos);
    Rational getPosition();
    
    void setOwned(bool flag);
    bool isOwned();
    
    void setProperty(String key, var value);
    var  getProperty(String key);
    bool hasProperty(String key);
    void removeProperty(String key);
    NamedValueSet* getProperties();
    
    
private:
    Polytempo_EventType type;
    int                 time;      // in miliseconds (NB. the property "time" is in seconds and whenever a time is stored in the property "value" it's in seconds as well!)
    int                 syncTime;  // in miliseconds
    Rational            position;
    
    bool                owned;     // indicate if this event is owned by a another data structure (e.g. a score) 

    ScopedPointer < NamedValueSet > properties;
};



#endif // __Polytempo_Event__
