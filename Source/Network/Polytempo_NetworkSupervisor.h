#pragma once
#include "../Scheduler/Polytempo_EventObserver.h"
#include "Polytempo_PeerInfo.h"
#include "Polytempo_IPAddress.h"

#define NETWORK_SUPERVISOR_PING_INTERVAL	1000

class Polytempo_NetworkSupervisor : public Timer,
                                    public Polytempo_EventObserver
{
public:
    juce_DeclareSingleton(Polytempo_NetworkSupervisor, false)

    Polytempo_NetworkSupervisor();
    ~Polytempo_NetworkSupervisor() override;

    void timerCallback() override;

    String getDescription() const;
    String getScoreName() const;
    String getPeerName() const;
    void setManualPeerName(String name);

    void createSender(int portCount, const int* ports);

#ifndef POLYTEMPO_LIB
    void setComponent(Component* aComponent);
#endif

    void eventNotification(Polytempo_Event* event) override;
    Uuid getUniqueId();
    bool unicastFlood(Polytempo_IPAddress ownIp);

private:
    std::unique_ptr<OSCMessage> createAdvertiseMessage(String ownIp);

    Uuid uniqueId = nullptr;
    std::unique_ptr<OSCSender> oscSender;

#ifndef POLYTEMPO_LIB
    Component* component;
#endif

    std::unique_ptr<String> localName;
    std::unique_ptr<String> nodeName;
    std::unique_ptr<String> manualPeerName;

    Array<int> currentPorts;
};
