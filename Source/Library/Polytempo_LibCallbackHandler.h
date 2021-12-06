#pragma once
#include <string>

class EventCallbackHandler
{
public:
    virtual ~EventCallbackHandler() {}
    virtual void processEvent(std::string const& message) = 0;
};

class TickCallbackHandler
{
public:
    virtual ~TickCallbackHandler() {};
    virtual void processTick(double tick) = 0;
};

class StateCallbackHandler
{
public:
    virtual ~StateCallbackHandler() {};
    virtual void processState(int state, std::string message, std::vector<std::string> peers) = 0;
};
