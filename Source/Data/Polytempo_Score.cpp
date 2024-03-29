#include "Polytempo_Score.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "../Misc/Polytempo_Alerts.h"
#include "../Scheduler/Polytempo_EventScheduler.h"


void Polytempo_Score_Section::sort()
{
	Polytempo_EventComparator sorter;
	events.sort(sorter, true); // true = retain order of equal elements
}

Polytempo_Score::Polytempo_Score()
{
    nextEventIndex = 0;
    currentSectionIndex = -1;
    
    sectionMap.reset(new StringArray());
    
    // add "init" section
    initSection.reset(new Polytempo_Score_Section());
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

void Polytempo_Score::setReady(bool flag)
{
    ready = flag;
}

bool Polytempo_Score::isReady()
{
    return ready;
}

void Polytempo_Score::clear(bool clearInit)
{
    sections.clear();
    sectionMap.reset(new StringArray());
    currentSectionIndex = -1;
    
    if(clearInit)
        initSection.reset(new Polytempo_Score_Section());
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

void Polytempo_Score::addEvents(OwnedArray <Polytempo_Event>& events)
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
    Polytempo_Event *imageEvent = nullptr;

    bool done = false;
    
    // find next downbeat OR next cue-in OR next progressbar
    // OR next image that is not followed by one of the above
    for(i=0;i<sections[currentSectionIndex]->events.size();i++)
    {
        event = sections[currentSectionIndex]->events[i];
        if(event->getTime() < time) continue;
        
        Polytempo_EventType eventType = event->getType();

        if((eventType == eventType_Beat &&
            (int(event->getProperty(eventPropertyString_Pattern)) < 20 ||
             int(event->getProperty(eventPropertyString_Cue)) > 0))
            || eventType == eventType_Progressbar
           || (eventType == eventType_Image && imageEvent != nullptr))
        {
            tempTime = imageEvent == nullptr ? float(event->getTime()) : float(imageEvent->getTime());
            *waitBeforStart = tempTime - time;
            
            // find the first event that has the same time as the downbeat
            j=i;
            while(j>0 && sections[currentSectionIndex]->events[--j]->getTime() == (int)tempTime) i=j;
            nextEventIndex = i;
            
            done = true;
            break;
        }
        
        if(eventType == eventType_Image) imageEvent = event;
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
    
    Polytempo_Event *marker = nullptr;                    // the last marker is enough
    HashMap<var, Array<Polytempo_Event*>*> images;   // the last image (+ appended images) per region are enough
    
    for(i=0;i<sections[currentSectionIndex]->events.size();i++)
    {
        event = sections[currentSectionIndex]->events[i];
        if(event->getTime() > time + *waitBeforStart)
            break;
        
        Polytempo_EventType eventType = event->getType();
        
        if(eventType == eventType_Beat ||
           eventType == eventType_Osc ||
           eventType == eventType_Audio ||
           eventType == eventType_GotoTime || eventType == eventType_GotoMarker ||
           eventType == eventType_Start || eventType == eventType_Pause || eventType == eventType_Stop)
            continue;
        
        else if(eventType == eventType_Marker)
            marker = event;
        
        else if(eventType == eventType_Image)
            images.set(event->getProperty(eventPropertyString_RegionID), new Array<Polytempo_Event*>(event));

        else if(eventType == eventType_AppendImage)
        {
            if(!images.contains(event->getProperty(eventPropertyString_RegionID)))
                images.set(event->getProperty(eventPropertyString_RegionID), new Array<Polytempo_Event*>(event));
            else
                images[event->getProperty(eventPropertyString_RegionID)]->add(event);
        }
        else if(eventType == eventType_Progressbar)
        {
            if(event->getTime() + (float)event->getProperty(eventPropertyString_Duration)*1000 > time + *waitBeforStart)
                images.set(event->getProperty(eventPropertyString_RegionID), new Array<Polytempo_Event*>(event));
        }
        else
            events->add(event);

    }
    // add the necessary images and the last marker
	if (marker != nullptr)
		events->add(marker);
	
    for(Array<Polytempo_Event*>* regionImages : images) {
        for(Polytempo_Event *image : *regionImages)
            events->add(image);
    }
    
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
       nextEventIndex < sections[currentSectionIndex]->events.size())
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
    return initSection.get();
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
    String jsonString = "{\n";
    for(int i=-1;i<sections.size();i++)
    {
        String tempString, block1, block2;
        Polytempo_Score_Section *section;

        if(i == -1)
        {
            section = initSection.get();
            jsonString += "\"init\": [\n";
        }
        else
        {
            section = sections[i];
            jsonString += "\"sequence"+String(i+1)+"\": [\n";
        }

        for(auto event : section->events)
        {
            tempString = "\t{\""+event->getTypeString()+"\": {";

            for(auto property : *event->getProperties())
            {
                String key = property.name.toString();
                var value = property.value;
                if(key[0] != '~' &&
                   key != eventPropertyString_TimeTag)
                {
                    tempString += "\""+key+"\": ";
                    if(value.isDouble())
                    {
                        tempString += String(double(value), 3);
                        if(tempString.endsWith("0")) tempString = tempString.dropLastCharacters(1);
                        if(tempString.endsWith("0")) tempString = tempString.dropLastCharacters(1);
                    }
                    else if(value.isString())
                        tempString += "\""+value.toString()+"\"";
                    else if(value.isArray())
                    {
                        tempString += "[";
                        for(auto item : *value.getArray())
                        {
                            if(item.isDouble())
                            {
                                tempString += String(double(item), 3);
                                if(tempString.endsWith("0")) tempString = tempString.dropLastCharacters(1);
                                if(tempString.endsWith("0")) tempString = tempString.dropLastCharacters(1);
                            }
                            else if(item.isString())
                                tempString += "\""+item.toString()+"\"";
                            else
                                tempString += item.toString();
                            tempString += ", ";
                        }
                        tempString = tempString.trimCharactersAtEnd(", ");
                        tempString += "]";
                    }
                    else
                        tempString += value.toString();
                }
                tempString += ", ";
            }
            
            tempString = tempString.trimCharactersAtEnd(", ");
            tempString += "}},\n";

            if(jsonStringInTwoBlocks && i != -1 &&
               (event->getType() == eventType_Beat || event->getType() == eventType_Marker || event->getType() == eventType_Comment))
                block2 += tempString;
            else
                block1 += tempString;
        }
        jsonString += block1 + block2;
        jsonString = jsonString.trimCharactersAtEnd(",\n");
        jsonString += "\n],\n";
    }
    jsonString = jsonString.trimCharactersAtEnd(",\n");
    jsonString += "}\n";

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
                    if (properties.getValueAt(k).isString())
                    {
                        event->setProperty(eventPropertyString_Value, properties.getValueAt(k).toString());
                        addEvent(event, addToInit);
                    }
                    else if (properties.getValueAt(k).getDynamicObject() != nullptr)
                    {
                        NamedValueSet tempProps = properties.getValueAt(k).getDynamicObject()->getProperties();
                        for(int l=0;l < tempProps.size(); l++)
                        {
                            event->setProperty(tempProps.getName(l).toString(), tempProps.getValueAt(l));
                        }
                        addEvent(event, addToInit);
                    }
                }
            }
        }
        
        if(!addToInit)  sortSection();
    }
}

void Polytempo_Score::writeToFile(File& file)
{
    FileOutputStream stream(file);
    stream.writeText(getJsonString(), false, false, nullptr);
    
    dirty = false;
}
