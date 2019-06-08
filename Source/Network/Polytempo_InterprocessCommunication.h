/*
  ==============================================================================

    Polytempo_ InterprocessCommunication.h
    Created: 7 Jun 2019 4:51:30pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class Ipc : public InterprocessConnection
{
public:
	void connectionMade() override;
	void connectionLost() override;
	void messageReceived(const MemoryBlock& message) override;
};

class IpcServer : public InterprocessConnectionServer
{
protected:
	InterprocessConnection* createConnectionObject() override;
};

class Polytempo_InterprocessCommunication: public Timer
{
public:
	juce_DeclareSingleton(Polytempo_InterprocessCommunication, false);

	Polytempo_InterprocessCommunication();
	~Polytempo_InterprocessCommunication();
	void cleanUpServer();
	void cleanUpClient();
	void toggleMaster(bool master);
	void connectToMaster(String ip);
	void registerNewServerConnection(Ipc* connection);
	void notifyAllServerConnections(MemoryBlock m);

	void timerCallback() override;
private:
	ScopedPointer<IpcServer> server;
	ScopedPointer<Ipc> client;
	Array<Ipc*> serverConnections;
	int dataIndex;
};
