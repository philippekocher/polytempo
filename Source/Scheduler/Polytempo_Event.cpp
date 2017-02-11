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

#include "Polytempo_Event.h"


Polytempo_Event::Polytempo_Event(Polytempo_EventType t)
{
    type = t;
    time = 0;
    position = 0;
    
    properties = new NamedValueSet();
}

Polytempo_Event::Polytempo_Event(const Polytempo_Event& event)
{
    type = event.type;
    time = event.time;
    position = event.position;
    
    properties = new NamedValueSet(*event.properties);
}

Polytempo_Event::~Polytempo_Event()
{
    if(properties)    properties = nullptr;
}

// ----------------
// factories

Polytempo_Event* Polytempo_Event::makeEvent(String typeString)
{
    Polytempo_EventType type = eventType_None;
    
    if(typeString == eventTypeString_Beat)        type = eventType_Beat;
    else if(typeString == eventTypeString_Marker) type = eventType_Marker;
    
    return new Polytempo_Event(type);
}

Polytempo_Event* Polytempo_Event::makeEvent(Polytempo_EventType type)
{
    Polytempo_Event *event = new Polytempo_Event();
    event->type = type;
    return event;
}

Polytempo_Event* Polytempo_Event::makeEvent(Polytempo_EventType type, var value)
{
    Polytempo_Event *event = new Polytempo_Event();
    event->type = type;
    event->setProperty("value", value);
    return event;
}


// ----------------
// accessors

void Polytempo_Event::setType(String typeString)
{
    if     (typeString == eventTypeString_Open)           type = eventType_Open;

    else if(typeString == eventTypeString_Start)          type = eventType_Start;
    else if(typeString == eventTypeString_Stop)           type = eventType_Stop;
    else if(typeString == eventTypeString_Pause)          type = eventType_Pause;
    
    else if(typeString == eventTypeString_GotoMarker)     type = eventType_GotoMarker;
    else if(typeString == eventTypeString_GotoLocator)    type = eventType_GotoLocator;
    else if(typeString == eventTypeString_TempoFactor)    type = eventType_TempoFactor;
    
    else if(typeString == eventTypeString_Tick)           type = eventType_Tick;
    else if(typeString == eventTypeString_Beat)           type = eventType_Beat;
    
    else if(typeString == eventTypeString_Marker)         type = eventType_Marker;
    
    else if(typeString == eventTypeString_LoadImage)      type = eventType_LoadImage;
    else if(typeString == eventTypeString_AddRegion)      type = eventType_AddRegion;
    else if(typeString == eventTypeString_AddSplitRegion) type = eventType_AddSplitRegion;
    
    else if(typeString == eventTypeString_ClearAll)       type = eventType_ClearAll;
    
    else if(typeString == eventTypeString_Image)          type = eventType_Image;
    else if(typeString == eventTypeString_Text)           type = eventType_Text;
    else if(typeString == eventTypeString_Progressbar)    type = eventType_Progressbar;
    
    else if(typeString == eventTypeString_AddSender)      type = eventType_AddSender;
    else if(typeString == eventTypeString_Osc)            type = eventType_Osc;
    
    else if(typeString == eventTypeString_Settings)       type = eventType_Settings;
    
}

Polytempo_EventType Polytempo_Event::getType()
{
    return type;
}

String Polytempo_Event::getTypeString()
{
    switch(getType())
    {
        case eventType_None:
        case eventType_Ready:
        case eventType_DeleteAll:
            return String::empty;
        // these events have no string equivalent
        
        case eventType_Open:
            return eventTypeString_Open;
        
        case eventType_Start:
            return eventTypeString_Start;
            
        case eventType_Stop:
            return eventTypeString_Stop;
            
        case eventType_Pause:
            return eventTypeString_Pause;
            
        case eventType_GotoMarker:
            return eventTypeString_GotoMarker;
            
        case eventType_GotoLocator:
            return eventTypeString_GotoLocator;
            
        case eventType_TempoFactor:
            return eventTypeString_TempoFactor;
            
        case eventType_Tick:
            return eventTypeString_Tick;
            
        case eventType_Beat:
            return eventTypeString_Beat;
            
        case eventType_Marker:
            return eventTypeString_Marker;
            
        case eventType_LoadImage:
            return eventTypeString_LoadImage;
            
        case eventType_AddRegion:
            return eventTypeString_AddRegion;
            
        case eventType_AddSplitRegion:
            return eventTypeString_AddRegion;
            
        case eventType_ClearAll:
            return eventTypeString_ClearAll;
            
        case eventType_Image:
            return eventTypeString_Image;
            
        case eventType_Text:
            return eventTypeString_Text;
            
        case eventType_Progressbar:
            return eventTypeString_Progressbar;
            
        case eventType_AddSender:
            return eventTypeString_AddSender;
            
        case eventType_Osc:
            return eventTypeString_Osc;
            
        case eventType_Settings:
            return eventTypeString_Settings;
    }
    
    return String::empty;
}

void Polytempo_Event::setTime(float t)
{
    time = t;
    properties->set("time",t);
}

float Polytempo_Event::getTime()
{
    return time;
}

int Polytempo_Event::getMilisecondTime()
{
    return time * 1000 + 0.5;
}

void Polytempo_Event::setPosition(Rational pos)
{
    position = pos;
}

Rational Polytempo_Event::getPosition()
{
    return position;
}

void Polytempo_Event::setProperty(String key, var value)
{
    if     (key == "time")       time = float(value);
    else if(key == "position")   position = value;
    
    properties->set(key,value);
}

var Polytempo_Event::getProperty(String key)
{
    if(properties)   return (*properties)[key];
    else             return var::null;
}

bool Polytempo_Event::hasProperty(String key)
{
    return properties->contains(key);
}

NamedValueSet* Polytempo_Event::getProperties()
{
    return properties;
}


// ----------------------------------------------------
#pragma mark -
#pragma mark event to message conversion

String Polytempo_Event::getOscAddressFromType()
{
    String address = getTypeString();
    address = "/" + address;
    return address;
}

Array<var> Polytempo_Event::getOscMessageFromParameters()
{
    Array<var> *messages = new Array<var>();
    
    messages->add(time);
    messages->add(getProperty("value"));
    
    /*
     ok for the moment...
     
     else if(type == eventType_Return)         {}
     
     else if(type == eventType_GotoMarker)     {}
     else if(type == eventType_GotoLocator)    {}
     else if(type == eventType_TempoFactor)    {}
     
     else if(type == eventType_Tick)           {}
     else if(type == eventType_Beat)           {}
     
     else if(type == eventType_Marker)         {}
     
     else if(type == eventType_LoadImage)      {}
     else if(type == eventType_ShowImage)      {}
     else if(type == eventType_AddRegion)      {}
     
     */
    
    return *messages;
}

Polytempo_Event* Polytempo_Event::makeEvent(String address, Array<var> messages)
{
    Polytempo_Event *event = new Polytempo_Event();
    event->setType(address.substring(1));
    
    if(messages.size()>0 && !messages[0].isString())
    {
        event->setProperty("time",messages[0]);
        if(messages.size()>1 && !messages[1].isString())
            event->setProperty("value",messages[1]);
    }
    
    int i = 0;
    while(messages.size() > i+1)
    {
        if(messages[i].isString())
        {
            // "rect" is followed by 4 numbers
            if(messages[i] == eventPropertyString_Rect)
            {
                Array < var > r;
                r.set(0,0); r.set(1,0); r.set(2,1); r.set(3,1);
                int j = 0;
                i++;
                while(messages[i].isDouble() || messages[i].isInt())
                {
                    r.set(j++, messages[i++]);
                }
                event->setProperty(eventPropertyString_Rect, r);
            }
            // other parameters are followed by one value
            else
            {
                event->setProperty(messages[i], messages[i+1]);
                i = i + 2;
            }
        }
        else
        {
            i++;
        }
    }
    
    return event;
}



