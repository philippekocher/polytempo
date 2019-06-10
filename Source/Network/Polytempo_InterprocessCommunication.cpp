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

Ipc::Ipc() : InterprocessConnection(false)
{
}

Ipc::~Ipc()
{
}

void Ipc::connectionMade()
{
	Logger::writeToLog("Connection to " + getConnectedHostName());
}

void Ipc::connectionLost()
{
	Logger::writeToLog("Connection lost: " + getConnectedHostName());
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
			if(xml->getTagName() == "Event")
			{
				String type = xml->getStringAttribute("Type");
				xml->removeAttribute("Type");
				Array<var> messages;
				for(int i = 0; i < xml->getNumAttributes(); i++)
				{
					String name = xml->getAttributeName(i);
					messages.add(name);
					double ddd = xml->getDoubleAttribute(name, std::numeric_limits<double>::infinity());
					if(juce_isfinite(ddd))
					{
						messages.add(ddd);
					}
					else
					{
						int iii = xml->getIntAttribute(name, std::numeric_limits<int>::infinity());
						if(juce_isfinite(iii))
						{
							messages.add(iii);
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
			Logger::writeToLog(String((int)Time::getMillisecondCounterHiRes()) + " - Message received: " + message.toString());
			MemoryBlock m;
			String str = "Answer";
			m.append(str.getCharPointer(), str.length() + 1);
			sendMessage(m);
			Logger::writeToLog(String((int)Time::getMillisecondCounterHiRes()) + " - Answer sent");
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
		serverConnections.clear();
	}
}

void Polytempo_InterprocessCommunication::cleanUpClient()
{
	stopTimer();
	if(client != nullptr)
	{
		client->disconnect();
		client = nullptr;
	}
}

void Polytempo_InterprocessCommunication::toggleMaster(bool master)
{
	cleanUpServer();
	cleanUpClient();

	if (master)
	{
		server = new IpcServer();
		server->beginWaitingForSocket(1234, String());// Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString());
	}
}

void Polytempo_InterprocessCommunication::connectToMaster(String ip)
{
	cleanUpClient();
	dataIndex = 0;
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
		startTimer(5000);
	}
	else
	{
		Logger::writeToLog("Error connecting to server " + ip);
	}
}

void Polytempo_InterprocessCommunication::registerNewServerConnection(Ipc* connection)
{
	serverConnections.addIfNotAlreadyThere(connection);
}

void Polytempo_InterprocessCommunication::notifyAllServerConnections(MemoryBlock m)
{
	for (Ipc* connection : serverConnections)
	{
		connection->sendMessage(m);
	}
}

void Polytempo_InterprocessCommunication::notifyAllServerConnections(XmlElement e)
{
	MemoryBlock m;
	MemoryOutputStream os;
	e.writeToStream(os, "");
	String s = os.toString();
	m.append(s.getCharPointer(), s.length());

	notifyAllServerConnections(m);
}

void Polytempo_InterprocessCommunication::timerCallback()
{
	if(client != nullptr)
	{
		MemoryBlock m;
		String str = String(dataIndex++);
		m.append(str.getCharPointer(), str.length() + 1);
		Logger::writeToLog(String((int)Time::getMillisecondCounterHiRes()) + " - Testdata sent");
		client->sendMessage(m);
	}
}

juce_ImplementSingleton(Polytempo_InterprocessCommunication);
