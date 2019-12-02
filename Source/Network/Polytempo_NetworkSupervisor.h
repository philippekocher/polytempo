#ifndef __Polytempo_NetworkSupervisor__
#define __Polytempo_NetworkSupervisor__

#include "../Scheduler/Polytempo_EventObserver.h"
#include "Polytempo_PeerInfo.h"
#include "Polytempo_IPAddress.h"

#define NETWORK_SUPERVISOR_PING_INTERVAL	1000

class Polytempo_NetworkSupervisor : public Timer,
                                    public Polytempo_EventObserver
{
public:
	juce_DeclareSingleton(Polytempo_NetworkSupervisor, false);

	Polytempo_NetworkSupervisor();
    ~Polytempo_NetworkSupervisor();

	void timerCallback() override;

	String getDescription() const;
	String getScoreName() const; 
	String getPeerName() const;

    void createSender(int port);
    void setComponent(Component *aComponent);
	
    void eventNotification(Polytempo_Event *event) override;
	Uuid getUniqueId();
	void unicastFlood(Polytempo_IPAddress ownIp);

private:
	std::unique_ptr<OSCMessage> createAdvertiseMessage(String ownIp);

	Uuid uniqueId = nullptr;
	std::unique_ptr<OSCSender> oscSender;
    Component *component;
    
	std::unique_ptr < String > localName;
	std::unique_ptr < String > nodeName;

    int currentPort;
};


#endif  // __Polytempo_NetworkSupervisor__
