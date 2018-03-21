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

#ifdef POLYTEMPO_NETWORK
	pTimeSyncControl = nullptr;
#endif
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
	
	resetTimeSync();

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

void Polytempo_TimeProvider::handleTimeSyncMessage(Uuid senderId, uint32 masterTime, int timeIndex, int32 roundTrip)
{
	uint32 localLastSentTimestamp = lastSentTimestamp;
	int localLastSentTimeIndex = lastSentTimeIndex;

    if((sync && timeIndex != localLastSentTimeIndex) || senderId != lastMasterID)
	{
		displayMessage("Wrong time index or master ID", MessageType_Error);
		return;
	}

	int32 currentTimestamp = Time::getMillisecondCounter();
	int32 localTimeDiff = currentTimestamp - localLastSentTimestamp;
	int32 newTimeDiffToMaster = int32(int64(masterTime) - int64(currentTimestamp - localTimeDiff * 0.5));
	lastReceivedTimestamp = currentTimestamp;

	timeDiffHistory[timeDiffHistoryWritePosition] = newTimeDiffToMaster;
	timeDiffHistorySize = jmin(++timeDiffHistorySize, TIME_DIFF_HISTORY_SIZE);
	timeDiffHistoryWritePosition = (++timeDiffHistoryWritePosition) % TIME_DIFF_HISTORY_SIZE;

	// handle timing
	Array<uint32> valueArray;
	for (int i = 0; i < timeDiffHistorySize; i++)
		valueArray.add(timeDiffHistory[i]);
	valueArray.sort();
	if (valueArray.size() == 0)
		resetTimeSync();
	else if (valueArray.size() % 2)
		relativeMsToMaster = valueArray[valueArray.size() / 2];
	else
		relativeMsToMaster = valueArray[valueArray.size()/2] / 2 + valueArray[valueArray.size()/2-1] / 2;
	lastRoundTrip = localTimeDiff;
	maxRoundTrip = jmax(lastRoundTrip, roundTrip);

#if(JUCE_DEBUG)
	displayMessage(String(relativeMsToMaster) + "; RT " + String(localTimeDiff)+"ms; MRT " + String(maxRoundTrip), MessageType_Info);
#else
	displayMessage("Sync, MRT: " + String(maxRoundTrip)+"ms", MessageType_Info);
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
	sync = masterFlag || (lastReceivedTimestamp > lastSentTimestamp) || (lastSentTimestamp - lastReceivedTimestamp < SYNC_TIME_VALID_PERIOD_MS);

	if (masterFlag || !sync)
		relativeMsToMaster = 0;

	*pTime = Time::getMillisecondCounter() + relativeMsToMaster;
	
	return sync;
}

uint32 Polytempo_TimeProvider::getDelaySafeTimestamp()
{
	uint32 safeTime;
	getSyncTime(&safeTime);
	safeTime += maxRoundTrip;

	return safeTime;
}

bool Polytempo_TimeProvider::isMaster() const
{
	return masterFlag;
}

void Polytempo_TimeProvider::setRemoteMasterPeer(String ip, Uuid id, bool master)
{
	if (!master)
	{
		if (lastMasterID == id)
		{
			// means previous master discontinued to be master
			resetTimeSync();
		}
		return;
	}

	if(masterFlag)
	{
		displayMessage("Another master detected", MessageType_Error);
		resetTimeSync();
		return;
	}

	// master ID check
	if (lastMasterID != id)
	{
		resetTimeSync();
		displayMessage("Master changed", MessageType_Warning);
	}
	
	lastMasterID = id;
	lastMasterIp = ip;
}

#ifdef POLYTEMPO_NETWORK
void Polytempo_TimeProvider::registerUserInterface(Polytempo_TimeSyncControl* pControl)
{
	pTimeSyncControl = pControl;
}
#endif

void Polytempo_TimeProvider::oscMessageReceived(const OSCMessage& message)
{
	String addressPattern = message.getAddressPattern().toString();
	OSCArgument* argumentIterator = message.begin();

	if (addressPattern == "/timeSyncRequest")
	{
		String senderIp = (argumentIterator++)->getString();
		int timeIndex = (argumentIterator++)->getInt32();
		int32 lastRoundTripFromClient = (argumentIterator++)->getInt32();

		if (masterFlag)
		{
			uint32 ts = Time::getMillisecondCounter();
			bool ok = oscSender->sendToIPAddress(senderIp, oscPort, 
				OSCMessage(
					OSCAddressPattern("/timeSyncReply"), 
					OSCArgument(Polytempo_NetworkSupervisor::getInstance()->getUniqueId().toString()), 
					OSCArgument(int32(ts)), 
					OSCArgument(timeIndex),
					OSCArgument(maxRoundTrip)));
			displayMessage(ok ? "Mastertime sent" : "Fail", ok ? MessageType_Info : MessageType_Error);

			// handle round trip time
			roundTripTime[roundTripHistoryWritePosition] = lastRoundTripFromClient;
			roundTripHistorySize = jmin(++roundTripHistorySize, ROUND_TRIP_HISTORY_SIZE);
			roundTripHistoryWritePosition = (++roundTripHistoryWritePosition) % ROUND_TRIP_HISTORY_SIZE;

			int32 localMaxRoundTrip = 0;
			for (int i = 0; i < roundTripHistorySize; i++)
				localMaxRoundTrip = jmax(localMaxRoundTrip, roundTripTime[i]);
			maxRoundTrip = localMaxRoundTrip;
		}
		else
		{
			displayMessage("Wrong timeSyncRequest received", MessageType_Error);
		}
	}
	else if(addressPattern == "/timeSyncReply")
	{
		Uuid senderId = Uuid((argumentIterator++)->getString());
		uint32 argMasterTime = uint32((argumentIterator++)->getInt32());
		int timeIndex = (argumentIterator++)->getInt32();
		int32 maxRoundTripFromMaster = (argumentIterator++)->getInt32();

		handleTimeSyncMessage(senderId, argMasterTime, timeIndex, maxRoundTripFromMaster);
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
		bool ok = oscSender->sendToIPAddress(timeSyncMasterIp, oscPort, 
			OSCMessage(
				OSCAddressPattern("/timeSyncRequest"), 
				OSCArgument(Polytempo_NetworkInterfaceManager::getInstance()->getSelectedIpAddress().ipAddress.toString()), 
				OSCArgument(index),
				OSCArgument(lastRoundTrip)));
		if(!ok)
			displayMessage("send TS request failed", MessageType_Warning);
	}
    
    lastMasterIp = String::empty;
}

void Polytempo_TimeProvider::displayMessage(String message, MessageType messageType)
{
#ifdef POLYTEMPO_NETWORK
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
		return;
	}
#endif
	DBG(message);
}

void Polytempo_TimeProvider::resetTimeSync()
{
	sync = false;
	timeDiffHistorySize = 0;
	timeDiffHistoryWritePosition = 0;
	roundTripHistorySize = 0;
	roundTripHistoryWritePosition = 0;
	lastSentTimeIndex = 0;
	relativeMsToMaster = 0;
	maxRoundTrip = 0;
	lastMasterIp = String::empty;
	lastMasterID = Uuid::null();
}
