/*
  ==============================================================================

    Polytempo_ InterprocessCommunication.h
    Created: 7 Jun 2019 4:51:30pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#define POLYTEMPO_IPC_PORT	47524
class Ipc : public InterprocessConnection
{
public:
	Ipc();
	~Ipc();
	void connectionMade() override;
	void connectionLost() override;
	void messageReceived(const MemoryBlock& message) override;
private:
	String lastConnectedHost;
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

private:
	ScopedPointer<IpcServer> server;
	ScopedPointer<Ipc> client;
	OwnedArray<Ipc> serverConnections;
	int dataIndex;
};
