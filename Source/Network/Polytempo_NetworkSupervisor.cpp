#include "Polytempo_NetworkSupervisor.h"
#include "Polytempo_IPAddress.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "Polytempo_NetworkInterfaceManager.h"
#include "Polytempo_TimeProvider.h"
#include "../Misc/Polytempo_Alerts.h"
#include "../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"

Polytempo_NetworkSupervisor::Polytempo_NetworkSupervisor()
{
    oscSender = nullptr;
    component = nullptr;

    nodeName.reset(new String(SystemStats::getFullUserName()));

    startTimer(NETWORK_SUPERVISOR_PING_INTERVAL);
}

Polytempo_NetworkSupervisor::~Polytempo_NetworkSupervisor()
{
    stopTimer();

    localName = nullptr;
    nodeName = nullptr;
    oscSender = nullptr;

    clearSingletonInstance();

    Polytempo_NetworkInterfaceManager::deleteInstance();
}

juce_ImplementSingleton(Polytempo_NetworkSupervisor)

std::unique_ptr<OSCMessage> Polytempo_NetworkSupervisor::createAdvertiseMessage(String ownIp)
{
    return std::make_unique<OSCMessage>(
        OSCAddressPattern("/masteradvertise"),
        OSCArgument(getUniqueId().toString()),
        OSCArgument(ownIp));
}

void Polytempo_NetworkSupervisor::timerCallback()
{
    if (oscSender == nullptr || !Polytempo_TimeProvider::getInstance()->isMaster()) return;
    // nothing to do, if there is no socket or client not in Master mode

    Array<Polytempo_IPAddress> localIpAddresses;
    Polytempo_NetworkInterfaceManager::getInstance()->getAvailableIpAddresses(localIpAddresses);

    // broadcast a heartbeat
    for (Polytempo_IPAddress localIpAddress : localIpAddresses)
    {
        std::unique_ptr<OSCMessage> msg = createAdvertiseMessage(localIpAddress.m_ipAddress.toString());
        oscSender->sendToIPAddress(localIpAddress.getBroadcastAddress().toString(), currentPort, *msg);
    }

    if (component) component->repaint();
}

Uuid Polytempo_NetworkSupervisor::getUniqueId()
{
    if (uniqueId == nullptr)
        uniqueId = Uuid();

    return uniqueId;
}

void Polytempo_NetworkSupervisor::unicastFlood(Polytempo_IPAddress ownIp)
{
    OSCSender localSender;
    localSender.connect(ownIp.m_ipAddress.toString(), 0);
    std::unique_ptr<OSCMessage> msg = this->createAdvertiseMessage(ownIp.m_ipAddress.toString());
    IPAddress currentIp = ownIp.getFirstNetworkAddress();
    IPAddress lastIp = ownIp.getLastNetworkAddress();

    while (currentIp <= lastIp)
    {
        Logger::writeToLog("Sending node information to " + currentIp.toString());
        bool ok = localSender.sendToIPAddress(currentIp.toString(), currentPort, *msg);
        if (!ok)
        {
            Polytempo_Alert::show("Error", "Error sending node information to " + currentIp.toString());
            return;
        }
        // proceed to next address
        if (currentIp.address[3] == 255)
        {
            currentIp.address[3] = 0;
            if (currentIp.address[2] == 255)
            {
                currentIp.address[2] = 0;
                if (currentIp.address[1] == 255)
                {
                    currentIp.address[1] = 0;
                    currentIp.address[0]++;
                }
                else
                    currentIp.address[1]++;
            }
            else
                currentIp.address[2]++;
        }
        else
            currentIp.address[3]++;
    }
}

String Polytempo_NetworkSupervisor::getDescription() const
{
    String peerName = getPeerName();
    return getScoreName() + " (" + (peerName.isNotEmpty() ? peerName : "-") + ")";
}

String Polytempo_NetworkSupervisor::getScoreName() const
{
    return String(localName == nullptr ? "Untitled" : *localName);
}

String Polytempo_NetworkSupervisor::getPeerName() const
{
    return String(Polytempo_StoredPreferences::getInstance()->getProps().getValue("instanceName"));
}

void Polytempo_NetworkSupervisor::createSender(int port)
{
    currentPort = port;
    oscSender.reset(new OSCSender());
    oscSender->connect("255.255.255.255", currentPort);
}

void Polytempo_NetworkSupervisor::setComponent(Component* aComponent)
{
    component = aComponent;
}

void Polytempo_NetworkSupervisor::eventNotification(Polytempo_Event* event)
{
    if (event->getType() == eventType_Open)
    {
        const MessageManagerLock mml(Thread::getCurrentThread());

        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
        if (event->hasProperty(eventPropertyString_URL) || event->hasProperty(eventPropertyString_Value))
        {
            String filePath(event->getProperty(eventPropertyString_URL).toString());
            if(filePath.isEmpty()) filePath = event->getProperty(eventPropertyString_Value).toString();
            File file(filePath);
            if (file.existsAsFile())
            {
                // call on the message thread
                MessageManager::callAsync([app, filePath]() { app->openScoreFilePath(filePath); });
            }
        }
    }
    else if (event->getType() == eventType_Settings)
    {
        if (event->hasProperty("name"))
            localName.reset(new String(event->getProperty("name").toString()));
        if (event->hasProperty("brightness"))
        {
            Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
            Polytempo_NetworkWindow* window = app->getMainWindow();
            window->setBrightness(float(event->getProperty("brightness")));
        }
        if (event->hasProperty("fullScreen"))
        {
            bool fullScreen = int(event->getProperty("fullScreen")) != 0;
            
            // call on the message thread
            MessageManager::callAsync ([fullScreen]() {
                
                Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
                Polytempo_NetworkWindow* window = app->getMainWindow();

                window->setFullScreen(fullScreen);
            });
        }
    }
    else if (event->getType() == eventType_DeleteAll)
    {
        // reset name
        localName.reset(nullptr);

        // reset brightness
        Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
        Polytempo_NetworkWindow* window = app->getMainWindow();
        window->setBrightness(1.0f);
    }
}
