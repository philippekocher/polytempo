#pragma once

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