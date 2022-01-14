#pragma once
#include <string>
#include <vector>

class EventCallbackHandler
{
public:
    virtual ~EventCallbackHandler() {}
    virtual void processEvent(std::string const& message) = 0;
};

struct EventCallbackOptions
{
public:
    bool ignoreTimeTag = true;
};

class TickCallbackHandler
{
public:
    virtual ~TickCallbackHandler() {};
    virtual void processTick(double tick) = 0;
};

struct TickCallbackOptions
{
public:
};

class StateCallbackHandler
{
public:
    virtual ~StateCallbackHandler() {};
    virtual void processState(int state, std::string message, std::vector<std::string> peers) = 0;
};

struct StateCallbackOptions
{
public:
    bool callOnChangeOnly = true;
};
