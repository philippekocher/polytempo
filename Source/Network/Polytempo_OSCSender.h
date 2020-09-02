#pragma once
#include "../Scheduler/Polytempo_EventObserver.h"

class Polytempo_OSCSender : public Polytempo_EventObserver
{
    struct Target
    {
        String hostName;
        int port;
        bool ticks;
    };

public:
    Polytempo_OSCSender();
    ~Polytempo_OSCSender();
    
    void eventNotification(Polytempo_Event *event) override;
    void addSender(const String& senderID, const String& hostName, int port, bool ticks = false);
    void deleteAll();
    void sendOscEventAsMessage(Polytempo_Event *event);
    void sendEventAsMessage(Polytempo_Event* event, Target target);
    void sendTick(Polytempo_Event *event);

private:
    HashMap < String, Target > targetMap;
    OSCSender oscSender;
};