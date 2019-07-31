/*
  ==============================================================================

    Polytempo_ InterprocessCommunication.h
    Created: 7 Jun 2019 4:51:30pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Polytempo_PeerInfo.h"
#define POLYTEMPO_IPC_PORT	47524
#define REMOVE_INVALID_CONNECTIONS_TIMEOUT	10000

class Ipc : public InterprocessConnection
{
public:
	Ipc();
	~Ipc();
	void connectionMade() override;
	void connectionLost() override;
	void messageReceived(const MemoryBlock& message) override;
	String getRemotePeerName() const;
	String getRemoteScoreName() const;
	uint32 getLastHeartBeat() const;

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
	juce_DeclareSingleton(Polytempo_InterprocessCommunication, false);

	Polytempo_InterprocessCommunication();
	~Polytempo_InterprocessCommunication();
	void cleanUpServerConnections();
	void cleanUpServer();
	void cleanUpClient();
	void reset(bool isMaster);
	bool connectToMaster(String ip);
	void registerNewServerConnection(Ipc* connection);
	void notifyAllClients(MemoryBlock m);
	static MemoryBlock xmlToMemoryBlock(XmlElement e);
	void notifyAllClients(XmlElement e);
	bool notifyServer(XmlElement e) const;
	Polytempo_PeerInfo* getMasterInfo() const;
	bool isClientConnected() const;
	void getClientsInfo(OwnedArray<Polytempo_PeerInfo>* pPeers);

private:
	static Polytempo_PeerInfo* getPeerInfoFromIpc(Ipc* pIpc, uint32 referenceTime);

private:
	ScopedPointer<IpcServer> server;
	ScopedPointer<Ipc> client;
	OwnedArray<Ipc> serverConnections;
	int dataIndex;
};
