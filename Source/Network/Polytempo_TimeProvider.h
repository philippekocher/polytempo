#pragma once

#include "JuceHeader.h"
#ifdef POLYTEMPO_NETWORK
#include "Polytempo_InterprocessCommunication.h"
#include "../Views/PolytempoNetwork/Polytempo_TimeSyncControl.h"
#endif

#define TIME_DIFF_HISTORY_SIZE		10
#define	ROUND_TRIP_HISTORY_SIZE		20
#define SYNC_TIME_VALID_PERIOD_MS	10000
#define TIME_SYNC_INTERVAL_MS		1000

class Polytempo_TimeProvider : Timer
{
public:
    juce_DeclareSingleton(Polytempo_TimeProvider, true)

    Polytempo_TimeProvider();
    ~Polytempo_TimeProvider() override;

    bool getSyncTime(uint32* pTime);
    int32 getMRT() const;

#ifdef POLYTEMPO_NETWORK
    void toggleMaster(bool master);
    uint32 getDelaySafeTimestamp();
    bool isMaster() const;
    void setRemoteMasterPeer(String ip, Uuid id);
    void handleMessage(XmlElement message, Ipc* sender);
    void registerUserInterface(Polytempo_TimeSyncControl* pControl);
#endif
    enum MessageType
    {
        MessageType_Info,
        MessageType_Warning,
        MessageType_Error
    };

private:
    void handleTimeSyncMessage(Uuid senderId, uint32 masterTime, int timeIndex, int32 roundTrip);
    void createTimeIndex(int* pIndex, uint32* pTimestamp);
    void timerCallback() override;
    void displayMessage(String message, MessageType messageType) const;
    void resetTimeSync();

private:
#ifdef POLYTEMPO_NETWORK
    Polytempo_TimeSyncControl* pTimeSyncControl;
#endif

    int32 relativeMsToMaster;
    int32 maxRoundTrip;

    int32 timeDiffHistory[TIME_DIFF_HISTORY_SIZE];
#ifdef POLYTEMPO_NETWORK
    int32 roundTripTime[ROUND_TRIP_HISTORY_SIZE];
#endif
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
};
