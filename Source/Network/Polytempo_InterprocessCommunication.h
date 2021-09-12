#pragma once

#include "JuceHeader.h"
#include "Polytempo_PeerInfo.h"
#include "../Scheduler/Polytempo_Event.h"

#define POLYTEMPO_IPC_PORT	47524
#define REMOVE_INVALID_CONNECTIONS_TIMEOUT	10000

class Ipc : public InterprocessConnection
{
public:
    Ipc();
    ~Ipc() override;
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const MemoryBlock& message) override;
    String getRemotePeerName() const;
    String getRemoteScoreName() const;
    uint32 getLastHeartBeat() const;
    void setRemoteNames(String scoreName, String peerName);

private:
    String lastConnectedHost;
    String remotePeerName;
    String remoteScoreName;
    uint32 lastHeartBeat;
};

class IpcServer : public InterprocessConnectionServer
{
protected:
    InterprocessConnection* createConnectionObject() override;
};

class Polytempo_InterprocessCommunication
{
public:
    juce_DeclareSingleton(Polytempo_InterprocessCommunication, false)

    Polytempo_InterprocessCommunication();
    ~Polytempo_InterprocessCommunication();
    bool reset(bool isMaster);
    bool connectToMaster(String ip);
    void registerNewServerConnection(Ipc* connection);
    void notifyAllClients(MemoryBlock m, String namePattern = String());
    static MemoryBlock xmlToMemoryBlock(XmlElement e);
    void notifyAllClients(XmlElement e, String namePattern = String());
    bool notifyServer(XmlElement e) const;
    Polytempo_PeerInfo* getMasterInfo() const;
    bool isClientConnected() const;
    void getClientsInfo(OwnedArray<Polytempo_PeerInfo>* pPeers);
    void distributeEvent(Polytempo_Event* pEvent, String namePattern);

private:
    static Polytempo_PeerInfo* getPeerInfoFromIpc(Ipc* pIpc, uint32 referenceTime);
    void cleanUpServerConnections();
    void cleanUpServer();
    void cleanUpClient();

private:
    std::unique_ptr<IpcServer> server;
    std::unique_ptr<Ipc> client;
    OwnedArray<Ipc> serverConnections;
    int dataIndex;
};
