#pragma once

#include "../Scheduler/Polytempo_Event.h"

class Polytempo_Score_Section
{
public:
    void sort();
    OwnedArray<Polytempo_Event> events;
};

class Polytempo_Score
{
public:
    Polytempo_Score();
    ~Polytempo_Score();

    void setDirty(bool flag = true);
    bool isDirty();

    void clear(bool clearInit = false);

    void addEvent(Polytempo_Event*, bool addToInit = false);
    void addEvents(OwnedArray<Polytempo_Event>&);
    void removeEvent(Polytempo_Event*, bool removeFromInit = false);

    void addSection(String sectionName);
    void selectSection(String sectionName = String());
    void sortSection(int sectionIndex = -1);

    void setTime(int time);
    bool setTime(int time, Array<Polytempo_Event*>* events, float* waitBeforStart);

    Polytempo_Event* searchEvent(int referenceTime, Polytempo_EventType = eventType_None, bool searchBackwards = false);
    Polytempo_Event* getNextEvent();
    Polytempo_Event* getFirstEvent();
    bool getTimeForMarker(String marker, int* time);
    bool getMarkerForTime(int time, String* marker);

    Polytempo_Score_Section* getInitSection();
    OwnedArray<Polytempo_Event>* getInitEvents();
    OwnedArray<Polytempo_Event>* getEvents();
    Array<Polytempo_Event*> getEvents(Polytempo_EventType);

    String getJsonString();
    bool setJsonString(String);

    static void parse(File& file, Polytempo_Score** score);

    void writeToFile(File&);

private:
    static void parseJSON(File& JSONFile, Polytempo_Score** scoreFile);
    void parseVar(var);

    std::unique_ptr<StringArray> sectionMap;
    OwnedArray<Polytempo_Score_Section> sections;
    std::unique_ptr<Polytempo_Score_Section> initSection;

    int currentSectionIndex;
    uint32 nextEventIndex;

    bool dirty = false;
    bool jsonStringInTwoBlocks = true;
};
