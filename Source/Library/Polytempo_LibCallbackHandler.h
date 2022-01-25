#pragma once
#include <string>
#include <vector>

class PolytempoEventCallbackHandler
{
public:
    virtual ~PolytempoEventCallbackHandler() {}
    virtual void processEvent(std::string const& message) = 0;
};

struct PolytempoEventCallbackOptions
{
public:
    bool ignoreTimeTag = true;
};

struct PolytempoEventArgument
{
    std::string name;
    std::string valueString;
    enum EValueType { Value_Int, Value_Double, Value_String, Value_Int64 } eValueType;
};

struct PolytempoDetailedEventCallbackObject
{
public:
    std::string command;
    std::vector<PolytempoEventArgument> arguments;
};

class PolytempoDetailedEventCallbackHandler
{
public:
    virtual ~PolytempoDetailedEventCallbackHandler() {}
    virtual void processEvent(PolytempoDetailedEventCallbackObject const& object) = 0;
};

class PolytempoTickCallbackHandler
{
public:
    virtual ~PolytempoTickCallbackHandler() {};
    virtual void processTick(double tick) = 0;
};

struct PolytempoTickCallbackOptions
{
public:
};

class PolytempoStateCallbackHandler
{
public:
    virtual ~PolytempoStateCallbackHandler() {};
    virtual void processState(int state, std::string message, std::vector<std::string> peers) = 0;
    virtual void processMasterChanged(bool isMaster) = 0;
};

struct PolytempoStateCallbackOptions
{
public:
    bool callOnChangeOnly = true;
};
