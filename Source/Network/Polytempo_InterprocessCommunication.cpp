/*
  ==============================================================================

    Polytempo_ InterprocessCommunication.cpp
    Created: 7 Jun 2019 4:51:30pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_InterprocessCommunication.h"
#include "Polytempo_TimeProvider.h"
#include "../Scheduler/Polytempo_ScoreScheduler.h"
#include "../Scheduler/Polytempo_EventScheduler.h"
#include "../Misc/Polytempo_Alerts.h"

Ipc::Ipc() : InterprocessConnection(false)
{
}

Ipc::~Ipc()
{
}

void Ipc::connectionMade()
{
	Logger::writeToLog("Connection to " + getConnectedHostName());
	lastConnectedHost = getConnectedHostName();
}

void Ipc::connectionLost()
{
	Logger::writeToLog("Connection lost: " + lastConnectedHost);
}

void Ipc::messageReceived(const MemoryBlock& message)
{
	if (message.toString().startsWith("Answer"))
	{
		Logger::writeToLog(String((int)Time::getMillisecondCounterHiRes()) + " - Answer received");
	}
	else
	{
		std::unique_ptr<XmlElement> xml = parseXML(message.toString());
		if(xml != nullptr)
		{
			if(xml->getTagName() == "TimeSyncRequest" || xml->getTagName() == "TimeSyncReply")
			{
				Polytempo_TimeProvider::getInstance()->handleMessage(*xml, this);
			}
			else if(xml->getTagName() == "Event")
			{
				String type = xml->getStringAttribute("Type");
				xml->removeAttribute("Type");
				Array<var> messages;
				for(int i = 0; i < xml->getNumAttributes(); i++)
				{
					String name = xml->getAttributeName(i);
					messages.add(name);
					int iii = xml->getIntAttribute(name, std::numeric_limits<int>::infinity());
					if (juce_isfinite(iii))
					{
						messages.add(iii);
					}
					else
					{
						double ddd = xml->getDoubleAttribute(name, std::numeric_limits<double>::infinity());
						if (juce_isfinite(ddd))
						{
							messages.add(ddd);
						}
						else
						{
							messages.add(xml->getStringAttribute(name));
						}
					}
				}
				Polytempo_Event* e = Polytempo_Event::makeEvent(type, messages);
				if (e->getType() == eventType_None)
				{
					DBG("--unknown");
					delete e;
					return;
				}

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
					Polytempo_ScoreScheduler *scoreScheduler = Polytempo_ScoreScheduler::getInstance();

					if (!scoreScheduler->isRunning()) return; // ignore an event that has a "time"
															 // when the score scheduler isn't running

					syncTime += e->getTime() - scoreScheduler->getScoreTime();
				}

				if (e->hasProperty(eventPropertyString_Defer))
					syncTime += int(float(e->getProperty(eventPropertyString_Defer)) * 1000.0f);

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
}

InterprocessConnection* IpcServer::createConnectionObject()
{
	Ipc* newConnection = new Ipc();
	Polytempo_InterprocessCommunication::getInstance()->registerNewServerConnection(newConnection);
	return newConnection;
}

Polytempo_InterprocessCommunication::Polytempo_InterprocessCommunication(): dataIndex(0)
{
	server = new IpcServer();
}

Polytempo_InterprocessCommunication::~Polytempo_InterprocessCommunication()
{
	cleanUpServer();
	cleanUpClient();
}

void Polytempo_InterprocessCommunication::cleanUpServerConnections()
{
	for(int i = serverConnections.size()-1; i >= 0; i--)
	{
		serverConnections[i]->disconnect();
		serverConnections.remove(i);
	}
}

void Polytempo_InterprocessCommunication::cleanUpServer()
{
	if(server != nullptr)
	{
		server->stop();
		server = nullptr;
	}
	cleanUpServerConnections();
}

void Polytempo_InterprocessCommunication::cleanUpClient()
{
	if(client != nullptr)
	{
		client->disconnect();
		client = nullptr;
	}
}

void Polytempo_InterprocessCommunication::reset(bool isMaster)
{
	cleanUpClient();
	cleanUpServerConnections();

	if(isMaster)
	{
		bool ok = server->beginWaitingForSocket(POLYTEMPO_IPC_PORT, String());
		if (!ok)
			Polytempo_Alert::show("TCP-Server", "Error starting TCP server");
	}
	else
	{
		server->stop();
	}
}

bool Polytempo_InterprocessCommunication::connectToMaster(String ip)
{
	cleanUpClient();
	dataIndex = 0;
	client = new Ipc();
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
	serverConnections.addIfNotAlreadyThere(connection);
}

void Polytempo_InterprocessCommunication::notifyAllClients(MemoryBlock m)
{
	for (Ipc* connection : serverConnections)
	{
		connection->sendMessage(m);
	}
}

MemoryBlock Polytempo_InterprocessCommunication::xmlToMemoryBlock(XmlElement e)
{
	MemoryBlock m;
	MemoryOutputStream os;
	e.writeToStream(os, "");
	String s = os.toString();
	m.append(s.getCharPointer(), s.length());
	return m;
}

void Polytempo_InterprocessCommunication::notifyAllClients(XmlElement e)
{
	notifyAllClients(xmlToMemoryBlock(e));
}

bool Polytempo_InterprocessCommunication::notifyServer(XmlElement e) const
{
	if(client == nullptr || !client->isConnected())
		return false;
	
	client->sendMessage(xmlToMemoryBlock(e));
	return true;
}

juce_ImplementSingleton(Polytempo_InterprocessCommunication);
