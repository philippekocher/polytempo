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
    syncTime = 0;
    position = 0;
    owned = false;
    
    properties = new NamedValueSet();
}

Polytempo_Event::Polytempo_Event(const Polytempo_Event& event)
{
    type = event.type;
    time = event.time;
    syncTime = event.syncTime;
    position = event.position;
    owned = event.owned;
    
    properties = new NamedValueSet(*event.properties);
}

Polytempo_Event::~Polytempo_Event()
{
    if(properties)    properties = nullptr;
}

// ----------------------------------------------------
#pragma mark -
#pragma mark factories

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

Polytempo_Event* Polytempo_Event::makeEvent(String oscAddress, Array<var> messages)
{
    Polytempo_Event *event = new Polytempo_Event();
    event->setType(oscAddress.substring(1));
    
    int i = 0;
    int num = messages.size();

    if(messages.contains("rect")) num++;
    if(num % 2 == 1)
        // if there is an odd number of messages, the first one is assigned to the property "value"
    {
        event->setProperty(eventPropertyString_Value, messages[i++]);
    }
    
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

// ----------------------------------------------------
#pragma mark -
#pragma mark event to OSC conversion

String Polytempo_Event::getOscAddressFromType()
{
    String address = getTypeString();
    address = "/" + address;
    return address;
}

Array<var> Polytempo_Event::getOscMessageFromProperties()
{
    Array<var> message = Array<var>();
    
    NamedValueSet* properties = getProperties();
    
    for(juce::NamedValueSet::NamedValue val : *properties)
    {
        Identifier key = val.name;
        if(key.toString()[0] != '~')
        {
            message.add(key.toString());
            message.add(val.value);
        }
    }

    return message;
}

// ----------------------------------------------------
#pragma mark -
#pragma mark accessors

void Polytempo_Event::setType(String typeString)
{
    if     (typeString == eventTypeString_Open)           type = eventType_Open;

    else if(typeString == eventTypeString_Start)          type = eventType_Start;
    else if(typeString == eventTypeString_Stop)           type = eventType_Stop;
    else if(typeString == eventTypeString_Pause)          type = eventType_Pause;
    
    else if(typeString == eventTypeString_GotoMarker)     type = eventType_GotoMarker;
    else if(typeString == eventTypeString_GotoTime)       type = eventType_GotoTime;
    else if(typeString == eventTypeString_TempoFactor)    type = eventType_TempoFactor;
    
    else if(typeString == eventTypeString_Tick)           type = eventType_Tick;
    else if(typeString == eventTypeString_Beat)           type = eventType_Beat;
    
    else if(typeString == eventTypeString_Marker)         type = eventType_Marker;
    
    else if(typeString == eventTypeString_LoadImage)      type = eventType_LoadImage;
    else if(typeString == eventTypeString_AddRegion)      type = eventType_AddRegion;
    else if(typeString == eventTypeString_AddSection)     type = eventType_AddSection;
    
    else if(typeString == eventTypeString_ClearAll)       type = eventType_ClearAll;
    
    else if(typeString == eventTypeString_Image)          type = eventType_Image;
    else if(typeString == eventTypeString_Text)           type = eventType_Text;
    else if(typeString == eventTypeString_Progressbar)    type = eventType_Progressbar;
    
    else if(typeString == eventTypeString_AddSender)      type = eventType_AddSender;
    else if(typeString == eventTypeString_Osc)            type = eventType_Osc;
    
    else if(typeString == eventTypeString_Settings)       type = eventType_Settings;
    
    else type = eventType_None;
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
            
        case eventType_GotoTime:
            return eventTypeString_GotoTime;
            
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
            
        case eventType_AddSection:
            return eventTypeString_AddSection;
            
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

void Polytempo_Event::setValue(var val)
{
    setProperty(eventPropertyString_Value, val);
}

var  Polytempo_Event::getValue()
{
    return getProperty(eventPropertyString_Value);
}

void Polytempo_Event::setTime(int t)
{
    time = t;
    setProperty(eventPropertyString_Time, t * 0.001);
}

int Polytempo_Event::getTime()
{
    return time;
}

//int Polytempo_Event::getMilisecondTime()
//{
//    return (int)(time * 1000.0f + 0.5f);
//}

void  Polytempo_Event::setSyncTime(int t)
{
    syncTime = t;
	setProperty(eventPropertyString_TimeTag, t);
}

int Polytempo_Event::getSyncTime()
{
    return syncTime;
}

void Polytempo_Event::setPosition(Rational pos)
{
    position = pos;
}

Rational Polytempo_Event::getPosition()
{
    return position;
}

void Polytempo_Event::setOwned(bool flag)
{
    owned = flag;
}

bool Polytempo_Event::isOwned()
{
    return owned;
}

void Polytempo_Event::setProperty(String key, var value)
{
    if(key == eventPropertyString_Time)
    {
        float t = float(value);
        
        if(t > 0.0f)      time = t * 1000.0f + 0.5f;
        else if(t < 0.0f) time = t * 1000.0f - 0.5f;
        else              time = 0;
    }
    else if(key == "position")
        position = value;
    
    if(!properties) properties = new NamedValueSet();
    
    properties->set(key,value);
}

var Polytempo_Event::getProperty(String key)
{
    if(properties)   return var((*properties)[key]);
    else             return var::null;
}

bool Polytempo_Event::hasProperty(String key)
{
    if(!properties) return false;
    return properties->contains(key);
}

void Polytempo_Event::removeProperty(String key)
{
    properties->remove(key);
}

NamedValueSet* Polytempo_Event::getProperties()
{
    return properties;
}
