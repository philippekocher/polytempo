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

#include "Polytempo_Score.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "../Misc/Polytempo_Alerts.h"
#include "../Scheduler/Polytempo_EventScheduler.h"


class Polytempo_EventComparator
{
public:
    static int compareElements(Polytempo_Event* e1, Polytempo_Event* e2) throw()
    {
        float t1 = float(e1->getTime());
        float t2 = float(e2->getTime());
        
        int result = 0;
        
        if(t1>t2) result = 1;
        if(t1<t2) result = -1;
        
        if(result == 0)
        {
            Polytempo_EventType type1 = e1->getType();
            Polytempo_EventType type2 = e2->getType();
            
            if(type1 == eventType_Beat && type2 != eventType_Beat) result = 1;
            if(type1 != eventType_Beat && type2 == eventType_Beat) result = -1;
        }
        
        return result;
    }
};

void Polytempo_Score_Section::sort() const
{
	Polytempo_EventComparator sorter;
	events.sort(sorter, true); // true = retain order of equal elements
}

Polytempo_Score::Polytempo_Score()
{
    nextEventIndex = 0;
    currentSectionIndex = -1;
    
    sectionMap = new StringArray();
    
    // add "init" section
    initSection = new Polytempo_Score_Section();
}

Polytempo_Score::~Polytempo_Score()
{
    sectionMap = nullptr;
    initSection = nullptr;
}

void Polytempo_Score::setDirty(bool flag)
{
    dirty = flag;
}

bool Polytempo_Score::isDirty()
{
    return dirty;
}

void Polytempo_Score::clear(bool clearInit)
{
    sections.clear();
    sectionMap = new StringArray();
    currentSectionIndex = -1;
    
    if(clearInit)
        initSection = new Polytempo_Score_Section();
}

void Polytempo_Score::addEvent(Polytempo_Event *event, bool addToInit)
{
    event->setOwned(true);
    if(addToInit)
    {
        initSection->events.add(event);
    }
    else
    {
        if(currentSectionIndex == -1)
        {
            sectionMap->add("default");
            sections.add(new Polytempo_Score_Section());
            currentSectionIndex = sectionMap->indexOf("default");
        }
        sections[currentSectionIndex]->events.add(event);
        sortSection(currentSectionIndex);
    }
}

void Polytempo_Score::addEvents(OwnedArray < Polytempo_Event >& events)
{
    if(currentSectionIndex == -1)
    {
        sectionMap->add("sequence");
        sections.add(new Polytempo_Score_Section());
        currentSectionIndex = 0;
    }
    
    sections[currentSectionIndex]->events.addCopiesOf(events);
    sortSection();
}

void Polytempo_Score::removeEvent(Polytempo_Event *event, bool removeFromInit)
{
    if(removeFromInit)
    {
        initSection->events.removeObject(event);
    }
    else
    {
        sections[currentSectionIndex]->events.removeObject(event);
    }
}

void Polytempo_Score::addSection(String sectionName)
{
    Polytempo_Score_Section *sect = new Polytempo_Score_Section();
    sections.add(sect);
    sectionMap->add(sectionName);
    
    currentSectionIndex = sectionMap->indexOf(sectionName);
}

void Polytempo_Score::selectSection(String sectionName)
{
    if(sectionName == String() && sections.size() > 0)
        currentSectionIndex = 0;
    else
        currentSectionIndex = sectionMap->indexOf(sectionName);
    
    nextEventIndex = 0;
}

void Polytempo_Score::sortSection(int sectionIndex)
{
    if(sectionIndex == -1)
        sections[currentSectionIndex]->sort();
    else
        sections[sectionIndex]->sort();
}

void Polytempo_Score::setTime(int time)
{
    Polytempo_Event *event;
    
    if(currentSectionIndex != -1)
    {
        for(int i=0;i<sections[currentSectionIndex]->events.size();i++)
        {
            event = sections[currentSectionIndex]->events[i];
            if(event->getTime() >= time)
            {
                nextEventIndex = i;
                return;
            }
        }
    }
}

bool Polytempo_Score::setTime(int time, Array<Polytempo_Event*> *events, float *waitBeforStart)
{
    if(currentSectionIndex == -1) return false;
    
    int i,j;
    float tempTime;
    Polytempo_Event *event = nullptr;
    bool done = false;
    
    // find next downbeat OR next cue-in OR next progressbar
    for(i=0;i<sections[currentSectionIndex]->events.size();i++)
    {
        event = sections[currentSectionIndex]->events[i];
        if(event->getTime() >= time &&
           ((event->getType() == eventType_Beat &&
            (int(event->getProperty(eventPropertyString_Pattern)) < 20 ||
             int(event->getProperty(eventPropertyString_Cue)) > 0))
            || event->getType() == eventType_Progressbar))
        {
            tempTime = float(event->getTime());
            *waitBeforStart = tempTime - time;
            
            // find the first event that has the same time as the downbeat
            j=i;
            while(j>0 && sections[currentSectionIndex]->events[--j]->getTime() == tempTime) i=j;
            nextEventIndex = i;
            
            done = true;
            break;
        }
    }
    
    // if no downbeat, cue-in or progressbar has been found: find any event
    if(!done)
    {
        for(i=0;i<sections[currentSectionIndex]->events.size();i++)
        {
            event = sections[currentSectionIndex]->events[i];
            if(event->getTime() >= time)
            {
                tempTime = float(event->getTime());
                *waitBeforStart = tempTime - time;
                
                nextEventIndex = ++i;
                
                break;
            }
        }
    }
    
    // events to be executed immediately to update the internal state
    
    Polytempo_Event *marker = nullptr;               // the last marker is enough;
    HashMap<var, Polytempo_Event*> images; // one image per region is enough;
    
    for(i=0;i<sections[currentSectionIndex]->events.size();i++)
    {
        event = sections[currentSectionIndex]->events[i];
        if(event->getTime() > time + *waitBeforStart)
            break;
        
        if(event->getType() == eventType_Beat ||
           event->getType() == eventType_Osc)   continue;
        
        else if(event->getType() == eventType_Marker)
            marker = event;
        
        else if(event->getType() == eventType_Image)
            images.set(event->getProperty(eventPropertyString_RegionID), event);
        
        else
            events->add(event);

    }
    // add the necessary images and the last marker
	if (marker != nullptr)
		events->add(marker);
	
	for(Polytempo_Event *image : images)
        events->add(image);
    
    return true;
}

Polytempo_Event* Polytempo_Score::searchEvent(int referenceTime, Polytempo_EventType type, bool searchBackwards)
{
    if(currentSectionIndex == -1 || currentSectionIndex >= sections.size())
        return nullptr;
    
    /* search a event of a certain type that does not have the same time as the current event. We assume that the events have been sorted.
     */
    
    if(searchBackwards)
    {
        for(int i=nextEventIndex-1;i>=0;i--)
        {
            Polytempo_Event* event = sections[currentSectionIndex]->events[i];
            if(type == eventType_None ||
               (type == event->getType() && referenceTime > event->getTime()))
                return event;
        }
    }
    else
    {
        for(int i=nextEventIndex;i<sections[currentSectionIndex]->events.size();i++)
        {
            Polytempo_Event* event = sections[currentSectionIndex]->events[i];
            if(type == eventType_None ||
               (type == event->getType() && referenceTime < event->getTime()))
                return event;
        }
    }
    
    return nullptr;
}

Polytempo_Event* Polytempo_Score::getNextEvent()
{
    // we assume that the events have been sorted
    
    if(currentSectionIndex > -1 && currentSectionIndex < sections.size() &&
       nextEventIndex < uint32(sections[currentSectionIndex]->events.size()))
        return sections[currentSectionIndex]->events[nextEventIndex++];
    else
        return nullptr;
}

Polytempo_Event* Polytempo_Score::getFirstEvent()
{
    if(currentSectionIndex != -1)
        return sections[currentSectionIndex]->events[0];
    else
        return nullptr;
}

bool Polytempo_Score::getTimeForMarker(String marker, int *time)
{
    if(currentSectionIndex < 0) return false;
    
    for(int i=0;i<sections[currentSectionIndex]->events.size();i++)
    {
        Polytempo_Event *event = sections[currentSectionIndex]->events[i];
        if(event->getType() == eventType_Marker && event->getProperty(eventPropertyString_Value).toString() == marker)
        {
            *time = event->getTime();
            return true;
        }
    }
    
    return false;
}

bool Polytempo_Score::getMarkerForTime(int time, String* marker)
{
    for(int i=0;i<sections[currentSectionIndex]->events.size();i++)
    {
        Polytempo_Event *event = sections[currentSectionIndex]->events[i];
        if(event->getType() == eventType_Marker && event->getTime() == time)
        {
            *marker = event->getProperty(eventPropertyString_Value).toString();
            return true;
        }
    }
    
    return false;
}

Polytempo_Score_Section* Polytempo_Score::getInitSection()
{
    return initSection;
}

OwnedArray < Polytempo_Event >* Polytempo_Score::getInitEvents()
{
    return &initSection->events;
}

OwnedArray < Polytempo_Event >* Polytempo_Score::getEvents()
{
    return &sections[currentSectionIndex]->events;
}

Array<Polytempo_Event*> Polytempo_Score::getEvents(Polytempo_EventType type)
{
    Array<Polytempo_Event*> result;
    Polytempo_Event *event;
    
    for(int i=0;i<initSection->events.size();i++)
    {
        event = initSection->events[i];
        if(event->getType() == type)
        {
            result.add(event);
        }
    }
    if(currentSectionIndex != -1)
    {
        for(int i=0;i<sections[currentSectionIndex]->events.size();i++)
        {
            event = sections[currentSectionIndex]->events[i];
            if(event->getType() == type)
            {
                result.add(event);
            }
        }
    }
    return result;
}

String Polytempo_Score::getJsonString()
{
    DynamicObject* jsonSections = new DynamicObject();
    
    for(int i=-1;i<sections.size();i++)
    {
        var jsonSection1, jsonSection2;
        Polytempo_Score_Section *section;
        
        if(i == -1) section = initSection;
        else        section = sections[i];
        
        for(int j=0;j<section->events.size();j++)
        {
            DynamicObject* jsonEvent = new DynamicObject();
            DynamicObject* jsonEventProperties = new DynamicObject();
            
            Polytempo_Event* event = section->events[j];
            NamedValueSet* properties = event->getProperties();
            
            for(int k=0;k<properties->size();k++)
            {
                Identifier key = properties->getName(k);
                if(key.toString()[0] != '~' &&
                   key.toString() != eventPropertyString_TimeTag)
                    jsonEventProperties->setProperty(key, properties->getValueAt(k));
            }
            
            jsonEvent->setProperty(event->getTypeString(), jsonEventProperties);
            
            if(jsonStringInTwoBlocks &&
               (event->getType() == eventType_Beat || event->getType() == eventType_Marker))
                jsonSection2.append(jsonEvent);
            else
                jsonSection1.append(jsonEvent);
        }
        
        for(int j=0;j<jsonSection2.size();j++)
        {
            jsonSection1.append(jsonSection2[j]);
        }
        
        if(i == -1) jsonSections->setProperty("init", jsonSection1);
        else jsonSections->setProperty(sectionMap->getReference(i), jsonSection1);
    }
    var json(jsonSections); // store the outer object in a var
    
    
    String jsonString = JSON::toString(json, true, 4);
    
    /* a semiprofessional formatter:
     not as tight as the "all on one line" option
     not as lose as the standard layout
     */
    
    jsonString = jsonString.replaceSection(0,1,"{\n");    // beginning of file
    
    jsonString = jsonString.replace("null, ","[],\n");    // empty section
    jsonString = jsonString.replace("null","[]\n");       // empty section
    
    jsonString = jsonString.replace("[{","[\n\t{");       // new section
    jsonString = jsonString.replace("}], ","}\n\t],\r");  // between sections
    
    jsonString = jsonString.replace("}, ","},\n\t");      // indent events
    
    jsonString = jsonString.replace("}]}","}\n\t]\n}");   // end of file
    
    

    return jsonString;
}

bool Polytempo_Score::setJsonString(String jsonString)
{
    var jsonVar = JSON::parse(jsonString);
    
    if(jsonVar == var()) return false;
    
    parseVar(jsonVar);
    setDirty();
    return true;
}


void Polytempo_Score::parse(File& file, Polytempo_Score **scoreFile)
{
    if(file.getFileExtension() == ".json" || file.getFileExtension() == ".ptsco") Polytempo_Score::parseJSON(file, scoreFile);
    else Polytempo_Alert::show("Error", "No valid file extension:\n" + file.getFileName());
}

void Polytempo_Score::parseJSON(File& JSONFile, Polytempo_Score** score)
{
    //DBG("parse json");
    var jsonVar = JSON::parse(JSONFile);
    
    if(jsonVar == var())
    {
        Polytempo_Alert::show("Error", "Not a valid JSON file:\n" + JSONFile.getFileName());
        return;
    }
    
    *score = new Polytempo_Score();
    (*score)->parseVar(jsonVar);
}

void Polytempo_Score::parseVar(var jsonVar)
{
    NamedValueSet jsonSections = jsonVar.getDynamicObject()->getProperties();
    var jsonSection, jsonEvent;
    DynamicObject* jsonObject;
    bool addToInit;
    
    clear(true);
    
    for(int i=0; i < jsonSections.size(); i++)
    {
        Identifier id = jsonSections.getName(i);
        
        if(id.toString() == "init")
        {
            addToInit = true;
        }
        else
        {
            addSection(id.toString());
            addToInit = false;
        }
        
        jsonSection = jsonVar[id];
        
        for(int j=0; j < jsonSection.size(); j++)
        {
            jsonObject = jsonSection[j].getDynamicObject();
            if(jsonObject == nullptr) continue;

            Polytempo_Event* event = new Polytempo_Event();
            
            // build event from Dynamic Object
            
            NamedValueSet properties = jsonObject->getProperties();
            for(int k=0;k < properties.size();k++)
            {
                event->setType(properties.getName(k).toString());
                
                if(event->getType() != eventType_None)
                {
                    if(properties.getValueAt(k).getDynamicObject() == nullptr) continue;
                    
                    NamedValueSet tempProps = properties.getValueAt(k).getDynamicObject()->getProperties();
                    for(int l=0;l < tempProps.size(); l++)
                    {
                        event->setProperty(tempProps.getName(l).toString(), tempProps.getValueAt(l));
                    }
                    
                    addEvent(event, addToInit);
                }
            }
        }
        
        if(!addToInit)  sortSection();
    }
}

void Polytempo_Score::writeToFile(File& file)
{
    FileOutputStream stream(file);
    stream.writeString(getJsonString());
    
    dirty = false;
}

