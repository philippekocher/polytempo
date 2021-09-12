#pragma once

#include "JuceHeader.h"

#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
#include "Polytempo_InterprocessCommunication.h"
#endif
#ifdef POLYTEMPO_NETWORK
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
    uint32 getMRT() const;

#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
    bool toggleMaster(bool master);
    uint32 getDelaySafeTimestamp();
    bool isMaster() const;
    void setRemoteMasterPeer(String ip, Uuid id);
    void handleMessage(XmlElement message, Ipc* sender);
#endif
    #ifdef POLYTEMPO_NETWORK
    void registerUserInterface(Polytempo_TimeSyncControl* pControl);
#endif
    enum MessageType
    {
        MessageType_Info,
        MessageType_Warning,
        MessageType_Error
    };

private:
    void handleTimeSyncMessage(Uuid senderId, uint32 masterTime, int timeIndex, uint32 roundTrip);
    void createTimeIndex(int* pIndex, uint32* pTimestamp);
    void timerCallback() override;
    void displayMessage(String message, MessageType messageType) const;
    void resetTimeSync();

private:
#ifdef POLYTEMPO_NETWORK
    Polytempo_TimeSyncControl* pTimeSyncControl;
#endif

    int32 relativeMsToMaster;
    uint32 maxRoundTrip;

    int32 timeDiffHistory[TIME_DIFF_HISTORY_SIZE];
#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
    uint32 roundTripTime[ROUND_TRIP_HISTORY_SIZE];
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
    uint32 lastRoundTrip;

    Uuid lastMasterID;
};
