/*
  ==============================================================================

    Polytempo_ InterprocessCommunication.cpp
    Created: 7 Jun 2019 4:51:30pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_InterprocessCommunication.h"
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
	Logger::writeToLog(String((int)Time::getMillisecondCounterHiRes()) + " - Message received: " + message.toString());

	MemoryBlock m;
	String str = "Answer";
	m.append(str.getCharPointer(), str.length() + 1);
	sendMessage(m);
	Logger::writeToLog(String((int)Time::getMillisecondCounterHiRes()) + " - Answer sent");
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
	cleanUpClient();
}

void Polytempo_InterprocessCommunication::cleanUpServer()
{
	if(server != nullptr)
	{
		server->stop();
		server = nullptr;
	}
}

void Polytempo_InterprocessCommunication::cleanUpClient()
{
	if(client != nullptr)
	{
		client->disconnect();
		client = nullptr;
	}
}

void Polytempo_InterprocessCommunication::toggleMaster(bool master)
{
	cleanUpServer();

	if (master)
	{
		server = new IpcServer();
		server->beginWaitingForSocket(1234, String());// Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString());
	}
}

void Polytempo_InterprocessCommunication::connectToMaster(String ip)
{
	cleanUpClient();
	client = new Ipc();
	bool ok = client->connectToSocket(ip, 1234, 1000);
	if (ok)
	{
		Logger::writeToLog("Successfully connected to server " + ip);
		MemoryBlock m;
		String str = "Testdata";
		m.append(str.getCharPointer(), str.length() + 1);
		Logger::writeToLog(String((int)Time::getMillisecondCounterHiRes()) + " - Testdata sent");
		client->sendMessage(m);
	}
	else
	{
		Logger::writeToLog("Error connecting to server " + ip);
	}
}

juce_ImplementSingleton(Polytempo_InterprocessCommunication);
