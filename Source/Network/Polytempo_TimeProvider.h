/*
  ==============================================================================

    Polytempo_TimeProvider.h
    Created: 30 Nov 2017 7:58:57pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "../Views/PolytempoNetwork/Polytempo_TimeSyncControl.h"

#define TIME_DIFF_HISTORY_SIZE		10
#define SYNC_TIME_VALID_PERIOD_MS	10000
#define TIME_SYNC_INTERVAL_MS			4000
#define TIME_SYNC_OSC_PORT			9999

class Polytempo_TimeProvider : private OSCReceiver::Listener<OSCReceiver::RealtimeCallback>, Timer
{
public:
	juce_DeclareSingleton(Polytempo_TimeProvider, true);

	Polytempo_TimeProvider();
	~Polytempo_TimeProvider();

	void initialize(bool master, int oscPort);
	bool getSyncTime(uint32* pTime);
	bool isMaster() const;
	void setRemoteMasterPeer(String ip, Uuid id);
	void registerUserInterface(Polytempo_TimeSyncControl* pControl);

	enum MessageType { MessageType_Info, MessageType_Warning, MessageType_Error };

private:
	void handleTimeSyncMessage(Uuid senderId, int32 masterTime, int timeIndex);
	void createTimeIndex(int* pIndex, uint32* pTimestamp);
	void oscMessageReceived(const OSCMessage& message) override;
	void timerCallback() override;
	void displayMessage(String message, MessageType messageType);
	void resetTimeSync();

private:
	Polytempo_TimeSyncControl* pTimeSyncControl;
	ScopedPointer<OSCSender> oscSender;
	ScopedPointer<OSCReceiver> oscReceiver;
	int oscPort;

	int32 relativeMsToMaster;

	int32 timeDiffHistory[TIME_DIFF_HISTORY_SIZE];
	int timeDiffHistorySize;
	int timeDiffHistoryWritePosition;
	
	bool masterFlag;
	bool sync;
	
	int lastSentTimeIndex;
	uint32 lastSentTimestamp;
	uint32 lastReceivedTimestamp;
	
	Uuid lastMasterID;
	String lastMasterIp;
};