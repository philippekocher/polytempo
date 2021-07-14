#include "Polytempo_Event.h"

Polytempo_Event::Polytempo_Event(Polytempo_EventType t)
{
    type = t;
    time = 0;
    syncTime = 0;
    position = 0;
    owned = false;

    properties.reset(new NamedValueSet());
}

Polytempo_Event::Polytempo_Event(const Polytempo_Event& event)
{
    type = event.type;
    time = event.time;
    syncTime = event.syncTime;
    position = event.position;
    owned = event.owned;

    properties.reset(new NamedValueSet(*event.properties));
}

Polytempo_Event::~Polytempo_Event()
{
    if (properties) properties = nullptr;
}

XmlElement Polytempo_Event::getXml()
{
    NamedValueSet tempSet;
    for (const auto& namedValue : *properties)
    {
        if(namedValue.value.isArray())
        {
            String serializedArray = "@array:";
            for(auto v : *namedValue.value.getArray())
            {
                serializedArray.append("\t" + getXmlTypedString(v), 10000);
            }
            
            tempSet.set(namedValue.name, serializedArray);
        }
        else 
        {
            tempSet.set(namedValue.name, getXmlTypedString(namedValue.value));
        }
    }
    XmlElement ret = XmlElement("Event");
    ret.setAttribute("Type", getOscAddressFromType());
    tempSet.copyToXmlAttributes(ret);
    return ret;
}

// ----------------------------------------------------
#pragma mark -
#pragma mark factories

Polytempo_Event* Polytempo_Event::makeEvent(String typeString)
{
    Polytempo_EventType type = eventType_None;

    if (typeString == eventTypeString_Beat) type = eventType_Beat;
    else if (typeString == eventTypeString_Marker) type = eventType_Marker;

    return new Polytempo_Event(type);
}

Polytempo_Event* Polytempo_Event::makeEvent(Polytempo_EventType type)
{
    Polytempo_Event* event = new Polytempo_Event();
    event->type = type;
    return event;
}

Polytempo_Event* Polytempo_Event::makeEvent(Polytempo_EventType type, var value)
{
    Polytempo_Event* event = new Polytempo_Event();
    event->type = type;
    event->setProperty(eventPropertyString_Value, value);
    return event;
}

Polytempo_Event* Polytempo_Event::makeEvent(String oscAddress, Array<var> messages)
{
    Polytempo_Event* event = new Polytempo_Event();
    event->setType(oscAddress.substring(1));

    int i = 0;
    int num = messages.size();

    if (messages.contains(eventPropertyString_Rect)) num++;
    if (event->getType() != eventType_Osc && num % 2 == 1)
    // if there is an odd number of messages, the first one is assigned to the property "value"
    // only for messages other than OSC (OSC message array can have any number of elements)
    {
        event->setProperty(eventPropertyString_Value, messages[i++]);
    }

    while (messages.size() > i + 1)
    {
        if (messages[i].isString())
        {
            // "rect" is followed by 4 numbers
            if (messages[i] == eventPropertyString_Rect)
            {
                Array<var> r = defaultRectangle();
                int j = 0;
                i++;
                while (messages[i].isDouble() || messages[i].isInt())
                {
                    r.set(j++, messages[i++]);
                }
                event->setProperty(eventPropertyString_Rect, r);
            }
            // for type OSC, all elements following a "message" key are considered as array element of the "message" property
            else if(event->getType() == eventType_Osc && messages[i] == eventPropertyString_Message)
            {
                Array<var> arr;
                i++;
                while(i < messages.size())
                {
                    arr.add(messages[i++]);
                }
                event->setProperty(eventPropertyString_Message, arr);
            }
            // other parameters are followed by one value
            else
            {
                event->setProperty(messages[i], messages[i + 1]);
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

Polytempo_Event* Polytempo_Event::makeEvent(XmlElement* xmlElement)
{
    Polytempo_Event* event = new Polytempo_Event();

    String type = xmlElement->getStringAttribute("Type");
    xmlElement->removeAttribute("Type");
    event->setType(type.substring(1));

    for (int i = 0; i < xmlElement->getNumAttributes(); i++)
    {
        String name = xmlElement->getAttributeName(i);
        String valueString = xmlElement->getStringAttribute(name);
        if(valueString.startsWith("@array:"))
        {
            StringArray tokens;
            Array<var> varArray;
            tokens.addTokens(valueString, "\t", "");
            for (int iToken = 1; iToken < tokens.size(); iToken++)
            {
                var v = fromXmlTypedString(tokens[iToken]);
                varArray.add(v);
            }

            event->setProperty(name, varArray);
        }
        else
        {
            var v = fromXmlTypedString(valueString);
            event->setProperty(name, v);
        }
    }

    return event;
}

Array<var> Polytempo_Event::defaultRectangle()
{
    // default rectangle [0,0,1,1]
    Array<var> r;
    r.set(0, 0);
    r.set(1, 0);
    r.set(2, 1);
    r.set(3, 1);
    return r;
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

Array<var> Polytempo_Event::getOscMessageFromProperties() const
{
    Array<var> message = Array<var>();

    NamedValueSet* props = getProperties();

    for (NamedValueSet::NamedValue val : *props)
    {
        Identifier key = val.name;
        if (key.toString()[0] != '~')
        {
            if (key.toString() == eventPropertyString_Rect && val.value.isArray() && val.value.getArray()->size() >= 4)
            {
                message.add(eventPropertyString_Rect);
                for (int i = 0; i < 4; i++)
                    message.add(val.value.getArray()[i]);
            }
            else
            {
                message.add(key.toString());
                message.add(val.value);
            }
        }
    }

    return message;
}

// ----------------------------------------------------
#pragma mark -
#pragma mark accessors

void Polytempo_Event::setType(String typeString)
{
    if (typeString == eventTypeString_Open) type = eventType_Open;

    else if (typeString == eventTypeString_Start) type = eventType_Start;
    else if (typeString == eventTypeString_Stop) type = eventType_Stop;
    else if (typeString == eventTypeString_Pause) type = eventType_Pause;

    else if (typeString == eventTypeString_GotoMarker) type = eventType_GotoMarker;
    else if (typeString == eventTypeString_GotoTime) type = eventType_GotoTime;
    else if (typeString == eventTypeString_TempoFactor) type = eventType_TempoFactor;

    else if (typeString == eventTypeString_Tick) type = eventType_Tick;
    else if (typeString == eventTypeString_Beat) type = eventType_Beat;

    else if (typeString == eventTypeString_Marker) type = eventType_Marker;

    else if (typeString == eventTypeString_LoadImage) type = eventType_LoadImage;
    else if (typeString == eventTypeString_AddRegion) type = eventType_AddRegion;
    else if (typeString == eventTypeString_AddSection) type = eventType_AddSection;

    else if (typeString == eventTypeString_ClearAll) type = eventType_ClearAll;

    else if (typeString == eventTypeString_Image) type = eventType_Image;
    else if (typeString == eventTypeString_AppendImage) type = eventType_AppendImage;
    else if (typeString == eventTypeString_Text) type = eventType_Text;
    else if (typeString == eventTypeString_Progressbar) type = eventType_Progressbar;

    else if (typeString == eventTypeString_AddSender) type = eventType_AddSender;
    else if (typeString == eventTypeString_Osc) type = eventType_Osc;

    else if (typeString == eventTypeString_Settings) type = eventType_Settings;

    else type = eventType_None;
}

Polytempo_EventType Polytempo_Event::getType() const
{
    return type;
}

String Polytempo_Event::getTypeString()
{
    switch (getType())
    {
    case eventType_None:
    case eventType_Ready:
    case eventType_DeleteAll:
        return String();
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

    case eventType_AppendImage:
        return eventTypeString_AppendImage;

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

    return String();
}

void Polytempo_Event::setValue(var val)
{
    setProperty(eventPropertyString_Value, val);
}

var Polytempo_Event::getValue() const
{
    return getProperty(eventPropertyString_Value);
}

void Polytempo_Event::setTime(int t)
{
    time = t;
    setProperty(eventPropertyString_Time, t * 0.001);
}

int Polytempo_Event::getTime() const
{
    return time;
}

bool Polytempo_Event::hasDefinedTime() const
{
    return !getProperty(eventPropertyString_Time).isVoid() &&
           getProperty(eventPropertyString_Time).toString() != "nan";
}

void Polytempo_Event::setSyncTime(uint32 t)
{
    syncTime = t;
    setProperty(eventPropertyString_TimeTag, int32(t));
}

uint32 Polytempo_Event::getSyncTime() const
{
    return syncTime;
}

void Polytempo_Event::setPosition(Rational pos)
{
    position = pos;
}

Rational Polytempo_Event::getPosition() const
{
    return position;
}

void Polytempo_Event::setOwned(bool flag)
{
    owned = flag;
}

bool Polytempo_Event::isOwned() const
{
    return owned;
}

void Polytempo_Event::setProperty(String key, var value)
{
    if (key == eventPropertyString_Time)
    {
        float t = float(value);

        if (t > 0.0f) time = int(t * 1000.0f + 0.5f);
        else if (t < 0.0f) time = int(t * 1000.0f - 0.5f);
        else time = 0;
    }
    else if (key == "position")
        position = value;

    if (!properties) properties.reset(new NamedValueSet());

    properties->set(key, value);
}

var Polytempo_Event::getProperty(String key) const
{
    if (properties) return var((*properties)[key]);
    else return var();
}

bool Polytempo_Event::hasProperty(String key) const
{
    if (!properties) return false;
    return properties->contains(key);
}

void Polytempo_Event::removeProperty(String key)
{
    properties->remove(key);
}

NamedValueSet* Polytempo_Event::getProperties() const
{
    return properties.get();
}

String Polytempo_Event::getXmlTypedString(const var value)
{
    if (value.isBool())
        return "b:" + value.toString();
    if (value.isDouble())
        return "d:" + value.toString();
    if (value.isInt())
        return "i:" + value.toString();
    if (value.isInt64())
        return "i64:" + value.toString();
    
    return "s:" + value.toString();
}

var Polytempo_Event::fromXmlTypedString(String string)
{
    if (string.startsWith("b:"))
        return bool(string.substring(2).getIntValue());
    if (string.startsWith("d:"))
        return string.substring(2).getDoubleValue();
    if (string.startsWith("i:"))
        return string.substring(2).getIntValue();
    if (string.startsWith("i64:"))
        return string.substring(4).getLargeIntValue();
    if (string.startsWith("s:"))
        return string.substring(2);

    DBG("Invalid typed string received: " + string);
    return 0;
}
