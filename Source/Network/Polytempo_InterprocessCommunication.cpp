#include "Polytempo_InterprocessCommunication.h"
#include "Polytempo_TimeProvider.h"
#include "../Scheduler/Polytempo_EventScheduler.h"
#include "../Scheduler/Polytempo_ScoreScheduler.h"

#ifdef POLYTEMPO_NETWORK
#include "../Misc/Polytempo_Alerts.h"
#include "../Application/PolytempoNetwork/Polytempo_NetworkApplication.h"
#endif

#include "Polytempo_NetworkSupervisor.h"

Ipc::Ipc() : InterprocessConnection(false), lastHeartBeat(0)
{
}

Ipc::~Ipc()
{
}

void Ipc::connectionMade()
{
    NamedValueSet params;
    params.set("PeerName", Polytempo_NetworkSupervisor::getInstance()->getPeerName());
    params.set("ScoreName", Polytempo_NetworkSupervisor::getInstance()->getScoreName());
    XmlElement xml = XmlElement("Handshake");
    params.copyToXmlAttributes(xml);
    sendMessage(Polytempo_InterprocessCommunication::xmlToMemoryBlock(xml));
}

void Ipc::connectionLost()
{
    Logger::writeToLog("Connection lost to " + getRemoteScoreName() + "(" + getRemotePeerName() + ")");
}

void Ipc::messageReceived(const MemoryBlock& message)
{
    this->lastHeartBeat = Time::getMillisecondCounter();
    std::unique_ptr<XmlElement> xml = parseXML(message.toString());
    if (xml != nullptr)
    {
        if (xml->getTagName() == "Handshake")
        {
            // perform handshake
            remotePeerName = xml->getStringAttribute("PeerName");
            remoteScoreName = xml->getStringAttribute("ScoreName");
        }
        else if (xml->getTagName() == "TimeSyncRequest" || xml->getTagName() == "TimeSyncReply")
        {
            Polytempo_TimeProvider::getInstance()->handleMessage(*xml, this);
        }
        else if (xml->getTagName() == "Event")
        {
            Polytempo_Event* e = Polytempo_Event::makeEvent(xml.get());
            if (e->getType() == eventType_None)
            {
                DBG("--unknown");
                delete e;
                return;
            }
            
#ifdef POLYTEMPO_NETWORK
            else if(e->getType() == Polytempo_EventType::eventType_Open)
            {
                const MessageManagerLock mml(Thread::getCurrentThread());

                DBG("open");

                Polytempo_NetworkApplication* const app = dynamic_cast<Polytempo_NetworkApplication*>(JUCEApplication::getInstance());
                if (e->hasProperty(eventPropertyString_URL) || e->hasProperty(eventPropertyString_Value))
                {
                    String filePath(e->getProperty(eventPropertyString_URL).toString());
                    if(filePath.isEmpty()) filePath = e->getProperty(eventPropertyString_Value).toString();
                    File file(filePath);
                    if (file.existsAsFile())
                    {
                        // call on the message thread
                        MessageManager::callAsync([app, filePath]() { app->openScoreFilePath(filePath); });
                    }
                    return;
                }
            }
#endif
            
            // calculate syncTime
            uint32 syncTime;

            if (e->hasProperty(eventPropertyString_TimeTag))
                syncTime = uint32(int32(e->getProperty(eventPropertyString_TimeTag)));
            else
            {
                Polytempo_TimeProvider::getInstance()->getSyncTime(&syncTime);
            }

            if (e->hasProperty(eventPropertyString_Time))
            {
                Polytempo_ScoreScheduler* scoreScheduler = Polytempo_ScoreScheduler::getInstance();

                if (!scoreScheduler->isRunning()) return; // ignore an event that has a "time"
                // when the score scheduler isn't running

                syncTime += uint32(e->getTime() - scoreScheduler->getScoreTime());
            }

            if (e->hasProperty(eventPropertyString_Defer))
                syncTime += uint32(float(e->getProperty(eventPropertyString_Defer)) * 1000.0f);

            e->setSyncTime(syncTime);

            Polytempo_EventScheduler::getInstance()->scheduleEvent(e);
        }
    }
    else
    {
        // generic message
        Logger::writeToLog("Unknown Message received: " + message.toString());
    }
}

String Ipc::getRemotePeerName() const
{
    return remotePeerName;
}

String Ipc::getRemoteScoreName() const
{
    return remoteScoreName;
}

uint32 Ipc::getLastHeartBeat() const
{
    return lastHeartBeat;
}

void Ipc::setRemoteNames(String scoreName, String peerName)
{
    remoteScoreName = scoreName;
    remotePeerName = peerName;
}

InterprocessConnection* IpcServer::createConnectionObject()
{
    Ipc* newConnection = new Ipc();
    Polytempo_InterprocessCommunication::getInstance()->registerNewServerConnection(newConnection);
    return newConnection;
}

Polytempo_InterprocessCommunication::Polytempo_InterprocessCommunication(): dataIndex(0)
{
    server.reset(new IpcServer());
}

Polytempo_InterprocessCommunication::~Polytempo_InterprocessCommunication()
{
    cleanUpServer();
    cleanUpClient();
}

void Polytempo_InterprocessCommunication::cleanUpServerConnections()
{
    for (int i = serverConnections.size() - 1; i >= 0; i--)
    {
        serverConnections[i]->disconnect();
        serverConnections.remove(i);
    }
}

void Polytempo_InterprocessCommunication::cleanUpServer()
{
    if (server != nullptr)
    {
        server->stop();
        server = nullptr;
    }
    cleanUpServerConnections();
}

void Polytempo_InterprocessCommunication::cleanUpClient()
{
    if (client != nullptr)
    {
        client->disconnect();
        client = nullptr;
    }
}

bool Polytempo_InterprocessCommunication::reset(bool isMaster)
{
    // returns true if successful

    cleanUpClient();
    cleanUpServerConnections();

    if (isMaster)
    {
        bool ok = server->beginWaitingForSocket(POLYTEMPO_IPC_PORT, String());
        if (!ok)
        {
#ifdef POLYTEMPO_NETWORK
            Polytempo_Alert::show("TCP-Server", "Error starting TCP server");
#endif
            return false;
        }
    }
    else
    {
        server->stop();
    }

    return true;
}

bool Polytempo_InterprocessCommunication::connectToMaster(String ip)
{
    cleanUpClient();
    dataIndex = 0;
    client.reset(new Ipc());
    bool ok = client->connectToSocket(ip, POLYTEMPO_IPC_PORT, 1000);
    if (ok)
    {
        Logger::writeToLog("Successfully connected to server " + ip);
    }
    else
    {
        Logger::writeToLog("Error connecting to server " + ip);
    }

    return ok;
}

void Polytempo_InterprocessCommunication::registerNewServerConnection(Ipc* connection)
{
    if (!serverConnections.contains(connection))
        serverConnections.add(connection);
}

void Polytempo_InterprocessCommunication::notifyAllClients(MemoryBlock m, String namePattern)
{
    for (Ipc* connection : serverConnections)
    {
        if (namePattern == String() || connection->getRemoteScoreName().matchesWildcard(namePattern, true) || connection->getRemotePeerName().matchesWildcard(namePattern, true))
            connection->sendMessage(m);
    }
}

MemoryBlock Polytempo_InterprocessCommunication::xmlToMemoryBlock(XmlElement e)
{
    MemoryBlock m;
    MemoryOutputStream os;
    e.writeTo(os);
    String s = os.toString();
    m.append(s.getCharPointer(), (size_t)s.length());
    return m;
}

void Polytempo_InterprocessCommunication::notifyAllClients(XmlElement e, String namePattern)
{
    notifyAllClients(xmlToMemoryBlock(e), namePattern);
}

bool Polytempo_InterprocessCommunication::notifyServer(XmlElement e) const
{
    if (client == nullptr || !client->isConnected())
        return false;

    client->sendMessage(xmlToMemoryBlock(e));
    return true;
}

Polytempo_PeerInfo* Polytempo_InterprocessCommunication::getMasterInfo() const
{
    if (client == nullptr || !client->isConnected())
        return nullptr;

    return getPeerInfoFromIpc(client.get(), Time::getMillisecondCounter());
}

bool Polytempo_InterprocessCommunication::isClientConnected() const
{
    return client != nullptr && client->isConnected();
}

void Polytempo_InterprocessCommunication::getClientsInfo(OwnedArray<Polytempo_PeerInfo>* pPeers)
{
    uint32 currentTime = Time::getMillisecondCounter();

    // clean up inactive connections
    for (int i = 0; i < serverConnections.size(); i++)
    {
        if (currentTime - serverConnections[i]->getLastHeartBeat() > REMOVE_INVALID_CONNECTIONS_TIMEOUT)
        {
            serverConnections[i]->disconnect();
            serverConnections.remove(i);
            i--;
        }
    }

    for (Ipc* serverConnection : serverConnections)
    {
        pPeers->add(getPeerInfoFromIpc(serverConnection, currentTime));
    }
}

void Polytempo_InterprocessCommunication::distributeEvent(Polytempo_Event* pEvent, String namePattern)
{
    pEvent->setSyncTime(Polytempo_TimeProvider::getInstance()->getDelaySafeTimestamp());
    String localScoreName = Polytempo_NetworkSupervisor::getInstance()->getScoreName();
    String localInstanceName = Polytempo_NetworkSupervisor::getInstance()->getPeerName();

    XmlElement eventAsXml = pEvent->getXml();

#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
    if (localScoreName.matchesWildcard(namePattern, true) || localInstanceName.matchesWildcard(namePattern, true))
    {
        if (pEvent->hasProperty(eventPropertyString_Defer))
            pEvent->setSyncTime(pEvent->getSyncTime() + uint32(float(pEvent->getProperty(eventPropertyString_Defer)) * 1000.0f));

        Polytempo_EventScheduler::getInstance()->scheduleEvent(pEvent);
    }
#endif

    notifyAllClients(eventAsXml, namePattern);
}

Polytempo_PeerInfo* Polytempo_InterprocessCommunication::getPeerInfoFromIpc(Ipc* pIpc, uint32 referenceTime)
{
    Polytempo_PeerInfo* info = new Polytempo_PeerInfo();
    info->peerName = pIpc->getRemotePeerName();
    info->scoreName = pIpc->getRemoteScoreName();
    info->connectionOk = pIpc->isConnected() && (referenceTime - pIpc->getLastHeartBeat() < 2 * TIME_SYNC_INTERVAL_MS);

    return info;
}

juce_ImplementSingleton(Polytempo_InterprocessCommunication)
