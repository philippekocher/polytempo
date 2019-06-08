/*
  ==============================================================================

    Polytempo_TimeProvider.h
    Created: 30 Nov 2017 7:58:57pm
    Author:  christian.schweizer

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#ifdef POLYTEMPO_NETWORK
#include "../Views/PolytempoNetwork/Polytempo_TimeSyncControl.h"
#endif

#define TIME_DIFF_HISTORY_SIZE		10
#define	ROUND_TRIP_HISTORY_SIZE		20
#define SYNC_TIME_VALID_PERIOD_MS	10000
#define TIME_SYNC_INTERVAL_MS		4000

class Polytempo_TimeProvider : OSCReceiver::Listener<OSCReceiver::RealtimeCallback>, Timer
{
public:
	juce_DeclareSingleton(Polytempo_TimeProvider, true);

	Polytempo_TimeProvider();
	~Polytempo_TimeProvider();

	void initialize(int oscPort);
	void toggleMaster(bool master);
	bool getSyncTime(uint32* pTime);
	uint32 getDelaySafeTimestamp();
	int32 getMRT() const;
	bool isMaster() const;
	String getMasterIP() const;
	void setRemoteMasterPeer(String ip, Uuid id, bool master);
#ifdef POLYTEMPO_NETWORK
	void registerUserInterface(Polytempo_TimeSyncControl* pControl);
#endif
	enum MessageType { MessageType_Info, MessageType_Warning, MessageType_Error };

private:
	void handleTimeSyncMessage(Uuid senderId, uint32 masterTime, int timeIndex, int32 roundTrip);
	void createTimeIndex(int* pIndex, uint32* pTimestamp);
	void oscMessageReceived(const OSCMessage& message) override;
	void timerCallback() override;
	void displayMessage(String message, MessageType messageType) const;
	void resetTimeSync();

private:
#ifdef POLYTEMPO_NETWORK
	Polytempo_TimeSyncControl* pTimeSyncControl;
#endif
	ScopedPointer<OSCSender> oscSender;
	ScopedPointer<OSCReceiver> oscReceiver;
	int oscPort;

	int32 relativeMsToMaster;
	int32 maxRoundTrip;

	int32 timeDiffHistory[TIME_DIFF_HISTORY_SIZE];
	int32 roundTripTime[ROUND_TRIP_HISTORY_SIZE];
	int timeDiffHistorySize;
	int timeDiffHistoryWritePosition;
	int roundTripHistorySize;
	int roundTripHistoryWritePosition;
	
	bool masterFlag;
	bool sync;
	
	int lastSentTimeIndex;
	uint32 lastSentTimestamp;
	uint32 lastReceivedTimestamp;
	int32 lastRoundTrip;

	Uuid lastMasterID;
	String lastMasterIp;
};