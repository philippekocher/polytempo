#pragma once

#include "../Misc/Rational.h"

enum Polytempo_EventType
{
    eventType_None = 0,
        
    eventType_Comment,
    
    eventType_Open,
    
    eventType_Tick,
    eventType_Beat,
    eventType_Marker,
    
    eventType_Start,
    eventType_Stop,
    eventType_Pause,
    
    eventType_GotoMarker,
    eventType_GotoTime,
    eventType_TempoFactor,
    
    eventType_LoadImage,
    eventType_AddRegion,
    eventType_AddSection,
    
    eventType_Image,
    eventType_AppendImage,
    eventType_Text,
    eventType_Progressbar,
    
    eventType_LoadAudio,
    eventType_Audio,
    
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

#define eventTypeString_Comment         "#"

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
#define eventTypeString_AppendImage     "appendImage"
#define eventTypeString_Text            "text"
#define eventTypeString_Progressbar     "progressbar"

#define eventTypeString_LoadAudio       "loadAudio"
#define eventTypeString_Audio           "audio"

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
#define eventPropertyString_AudioID     "audioID"
#define eventPropertyString_ImageID     "imageID"
#define eventPropertyString_RegionID    "regionID"
#define eventPropertyString_MaxZoom     "maxZoom"
#define eventPropertyString_SectionID   "sectionID"
#define eventPropertyString_URL         "url"
#define eventPropertyString_Rect        "rect"
#define eventPropertyString_Layout      "layout"
#define eventPropertyString_XAlignment  "xAlignment"
#define eventPropertyString_YAlignment  "yAlignment"
#define eventPropertyString_Message     "message"
#define eventPropertyString_Pattern     "pattern"
#define eventPropertyDefault_Pattern    10
#define eventPropertyString_Linear      "linear"
#define eventPropertyString_Cue         "cue"
#define eventPropertyString_Gain        "gain"


class Polytempo_Event
{
public:
    Polytempo_Event(Polytempo_EventType ty = eventType_None);
    Polytempo_Event(const Polytempo_Event&);
    ~Polytempo_Event();
	XmlElement getXml();
    
    /* factories
     --------------------------------------- */
    static Polytempo_Event* makeEvent(String typeString);
    static Polytempo_Event* makeEvent(Polytempo_EventType type);
    static Polytempo_Event* makeEvent(Polytempo_EventType type, var value);
    static Polytempo_Event* makeEvent(String oscAddress, Array<var> values);
    static Polytempo_Event* makeEvent(XmlElement* xmlElement);

    static Array<var> defaultRectangle();

    /* event to OSC conversion
     --------------------------------------- */

    String getOscAddressFromType();
    Array<var> getOscMessageFromProperties() const;
    
    /* accessors
     --------------------------------------- */
    void setType(String);
    Polytempo_EventType getType() const;
    String getTypeString();
    
    void setValue(var val);
    var  getValue() const;
    
    void setTime(int t);
    int  getTime() const;
    bool hasDefinedTime() const;
    
    void   setSyncTime(uint32 t);
    uint32 getSyncTime() const;

    void setPosition(Rational pos);
    Rational getPosition() const;
    
    void setOwned(bool flag);
    bool isOwned() const;
    
    void setProperty(String key, var value);
    var  getProperty(String key) const;
    bool hasProperty(String key) const;
    void removeProperty(String key);
    NamedValueSet* getProperties() const;
    
    
private:
    static String getXmlTypedString(const var value);
    static var fromXmlTypedString(String string);

    Polytempo_EventType type;
    int                 time;      // in miliseconds (NB. the property "time" is in seconds and whenever a time is stored in the property "value" it's in seconds as well!)
    uint32              syncTime;  // in miliseconds
    Rational            position;
    
    bool                owned;     // indicate if this event is owned by a another data structure (e.g. a score) 

	std::unique_ptr<NamedValueSet> properties;
};


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
            
            // comments first
            if(type1 == eventType_Comment && type2 != eventType_Comment) return -1;
            if(type1 != eventType_Comment && type2 == eventType_Comment) return 1;

            // then markers
            if(type1 == eventType_Marker && type2 != eventType_Marker) return -1;
            if(type1 != eventType_Marker && type2 == eventType_Marker) return 1;

            // beats last
            if(type1 == eventType_Beat && type2 != eventType_Beat) return 1;
            if(type1 != eventType_Beat && type2 == eventType_Beat) return -1;

            if(type1 == eventType_AppendImage && type2 == eventType_Image) return 1;
            if(type1 == eventType_Image && type2 == eventType_AppendImage) return -1;
        }
        
        return result;
    }
};
