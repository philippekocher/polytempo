#pragma once

class EventCallbackHandler
{
public:
    virtual ~EventCallbackHandler() {}
    virtual void processEvent(std::string const& message) = 0;
};
