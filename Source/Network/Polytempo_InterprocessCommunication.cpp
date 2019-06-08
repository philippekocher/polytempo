/*
  ==============================================================================

    Polytempo_ InterprocessCommunication.cpp
    Created: 7 Jun 2019 4:51:30pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_InterprocessCommunication.h"
#include "Polytempo_NetworkInterfaceManager.h"
#include "Polytempo_TimeProvider.h"

void Ipc::connectionMade()
{
	Logger::writeToLog("Connection to " + getConnectedHostName());
}

void Ipc::connectionLost()
{
	Logger::writeToLog("Connection lost");
}

void Ipc::messageReceived(const MemoryBlock& message)
{
	Logger::writeToLog("Message received");
}

InterprocessConnection* IpcServer::createConnectionObject()
{
	return new Ipc();
}

Polytempo_InterprocessCommunication::Polytempo_InterprocessCommunication()
{
}

Polytempo_InterprocessCommunication::~Polytempo_InterprocessCommunication()
{
	cleanUpServer();
}

void Polytempo_InterprocessCommunication::cleanUpServer()
{
	if(server != nullptr)
	{
		server->stop();
		server = nullptr;
	}
}

void Polytempo_InterprocessCommunication::toggleMaster(bool master)
{
	cleanUpServer();

	if(master)
	{
		server = new IpcServer();
		server->beginWaitingForSocket(1234, String());// Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString());
	}
	else
	{
		client = new Ipc();
		bool ok = client->connectToSocket(Polytempo_TimeProvider::getInstance()->getMasterIP(), 1234, 1000);
		if(ok)
		{
			Logger::writeToLog("Successfully connected to server " + Polytempo_TimeProvider::getInstance()->getMasterIP());
			MemoryBlock m;
			String str = "Testdata";
			m.append(str.getCharPointer(), str.length()+1);
			client->sendMessage(m);
		}
		else
		{
			Logger::writeToLog("Error connecting to server " + Polytempo_TimeProvider::getInstance()->getMasterIP());
		}
	}
}

juce_ImplementSingleton(Polytempo_InterprocessCommunication);
