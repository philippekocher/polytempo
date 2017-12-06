/*
  ==============================================================================

    Polytempo_TimeProvider.cpp
    Created: 30 Nov 2017 7:58:57pm
    Author:  chris

  ==============================================================================
*/

#include "Polytempo_TimeProvider.h"
#include "Polytempo_NetworkSupervisor.h"
#include "Polytempo_NetworkInterfaceManager.h"


Polytempo_TimeProvider::Polytempo_TimeProvider(): relativeMsToMaster(0), timeDiffHistorySize(0), timeDiffHistoryWritePosition(0), masterFlag(false), sync(false), lastSentTimeIndex(0), lastSentTimestamp(0)
{
	oscSender = new OSCSender();
	oscReceiver = new OSCReceiver();

	pTimeSyncControl = nullptr;
}

Polytempo_TimeProvider::~Polytempo_TimeProvider()
{
	oscSender->disconnect();
	oscReceiver->disconnect();
	
	clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_TimeProvider)

void Polytempo_TimeProvider::initialize(bool master, int port)
{
	stopTimer();
	oscSender->disconnect();
	oscReceiver->disconnect();

	oscPort = port;
	masterFlag = master;
	sync = masterFlag;
	timeDiffHistorySize = 0;
	timeDiffHistoryWritePosition = 0;
	lastSentTimeIndex = 0;

	bool ok = oscSender->connect(Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString(), 0);
	if (!ok)
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Time Sync", "Error setting up time sync OSC sender");

	ok = oscReceiver->connect(oscPort);
	if (!ok)
		AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Time Sync", "Error setting up time sync OSC receiver");

	oscReceiver->addListener(this);

	if (!masterFlag)
		startTimer(TIME_SYNC_INTERVAL_MS);
}

void Polytempo_TimeProvider::handleTimeSyncMessage(Uuid senderId, int32 masterTime, int timeIndex)
{
	uint32 localLastSentTimestamp = lastSentTimestamp;
	int localLastSentTimeIndex = lastSentTimeIndex;

	if(sync && timeIndex != localLastSentTimeIndex || senderId != lastMasterID)
	{
		displayMessage("Wrong time index or master ID", MessageType_Error);
		return;
	}

	int32 currentTimestamp = Time::getMillisecondCounter();
	int32 localTimeDiff = currentTimestamp - localLastSentTimestamp;
	int32 newTimeDiffToMaster = int32(masterTime - (currentTimestamp - localTimeDiff * 0.5));

	lastReceivedTimestamp = localLastSentTimeIndex;
	timeDiffHistory[timeDiffHistoryWritePosition] = newTimeDiffToMaster;
	timeDiffHistorySize = jmin(++timeDiffHistorySize, TIME_DIFF_HISTORY_SIZE);
	timeDiffHistoryWritePosition = (++timeDiffHistoryWritePosition) % TIME_DIFF_HISTORY_SIZE;

	int64 sum = 0;
	for (int i = 0; i < timeDiffHistorySize; i++)
		sum += timeDiffHistory[i];
	
	relativeMsToMaster = int32(sum / timeDiffHistorySize);
#if(JUCE_DEBUG)
	displayMessage(String(relativeMsToMaster) + "; RT " + String(localTimeDiff)+"ms", MessageType_Info);
#else
	displayMessage("Sync! Roundtrip " + String(localTimeDiff)+"ms", MessageType_Info);
#endif
	
	sync = true;
}

void Polytempo_TimeProvider::createTimeIndex(int* pIndex, uint32* pTimestamp)
{
	lastSentTimestamp = Time::getMillisecondCounter();
	lastSentTimeIndex++;

	*pIndex = lastSentTimeIndex;
	*pTimestamp = lastSentTimestamp;
}

bool Polytempo_TimeProvider::getSyncTime(uint32* pTime)
{
	sync = (lastSentTimestamp - lastReceivedTimestamp < SYNC_TIME_VALID_PERIOD_MS);

	if (!sync)
		return false;

	*pTime = Time::getMillisecondCounter() + relativeMsToMaster;
	return true;
}

bool Polytempo_TimeProvider::isMaster() const
{
	return masterFlag;
}

void Polytempo_TimeProvider::setRemoteMasterPeer(String ip, Uuid id)
{
	if(masterFlag)
	{
		displayMessage("Another master detected", MessageType_Error);
	}
	// master ID check
	if (lastMasterID != id)
	{
		displayMessage("Master changed", MessageType_Warning);
	}

	lastMasterID = id;
	lastMasterIp = ip;
}

void Polytempo_TimeProvider::registerUserInterface(Polytempo_TimeSyncControl* pControl)
{
	pTimeSyncControl = pControl;
}

void Polytempo_TimeProvider::oscMessageReceived(const OSCMessage& message)
{
	String addressPattern = message.getAddressPattern().toString();
	OSCArgument* argumentIterator = message.begin();

	if (addressPattern == "/timeSyncRequest")
	{
		String senderIp = (argumentIterator++)->getString();
		int timeIndex = (argumentIterator++)->getInt32();
		
		if (masterFlag)
		{
			oscSender->sendToIPAddress(senderIp, oscPort, 
				OSCMessage(
					OSCAddressPattern("/timeSyncReply"), 
					OSCArgument(Polytempo_NetworkSupervisor::getInstance()->getUniqueId().toString()), 
					OSCArgument(int32(Time::getMillisecondCounter())), 
					OSCArgument(timeIndex)));
		}
		else
		{
			displayMessage("Wrong timeSyncRequest received", MessageType_Error);
		}
	}
	else if(addressPattern == "/timeSyncReply")
	{
		Uuid senderId = Uuid((argumentIterator++)->getString());
		int32 argMasterTime = (argumentIterator++)->getInt32();
		int timeIndex = (argumentIterator++)->getInt32();

		handleTimeSyncMessage(senderId, argMasterTime, timeIndex);
	}
}

void Polytempo_TimeProvider::timerCallback()
{
	String timeSyncMasterIp = lastMasterIp;
	if(timeSyncMasterIp.isEmpty())
	{
		displayMessage("No master detected", MessageType_Error);
	}
	else
	{
		int index;
		uint32 timestamp;
		createTimeIndex(&index, &timestamp);
		oscSender->sendToIPAddress(timeSyncMasterIp, oscPort, 
			OSCMessage(
				OSCAddressPattern("/timeSyncRequest"), 
				OSCArgument(Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString()), 
				OSCArgument(index)));
	}
}

void Polytempo_TimeProvider::displayMessage(String message, MessageType messageType)
{
	if (pTimeSyncControl != nullptr)
	{
		Colour c;
		switch(messageType)
		{
		case MessageType_Info: c = Colours::lightgreen; break;
		case MessageType_Warning: c = Colours::yellow; break;
		case MessageType_Error: c = Colours::orangered; break;
		default: c = Colours::grey;
		}
		pTimeSyncControl->showInfoMessage(message, c);
	}
	else
	{
		DBG(message);
	}
}
