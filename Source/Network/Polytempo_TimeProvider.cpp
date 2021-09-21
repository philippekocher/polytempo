#include "Polytempo_TimeProvider.h"
#include "Polytempo_InterprocessCommunication.h"
#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
#include "Polytempo_NetworkSupervisor.h"
#endif

Polytempo_TimeProvider::Polytempo_TimeProvider(): relativeMsToMaster(0), maxRoundTrip(0), timeDiffHistorySize(0), timeDiffHistoryWritePosition(0), roundTripHistorySize(0), roundTripHistoryWritePosition(0), masterFlag(false), sync(false), lastSentTimeIndex(0), lastSentTimestamp(0), lastReceivedTimestamp(0), lastRoundTrip(0)
{
    pTimeSyncControl = nullptr;
#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
    toggleMaster(false);
#endif
}

Polytempo_TimeProvider::~Polytempo_TimeProvider()
{
    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_TimeProvider)

bool Polytempo_TimeProvider::getSyncTime(uint32* pTime)
{
    sync = masterFlag || (lastReceivedTimestamp > lastSentTimestamp) || (lastSentTimestamp - lastReceivedTimestamp < SYNC_TIME_VALID_PERIOD_MS && lastSentTimestamp > 0);

    if (masterFlag || !sync)
        relativeMsToMaster = 0;

    *pTime = Time::getMillisecondCounter() + uint32(relativeMsToMaster);

    return sync;
}

uint32 Polytempo_TimeProvider::getMRT() const
{
    return maxRoundTrip;
}

void Polytempo_TimeProvider::handleTimeSyncMessage(Uuid senderId, uint32 masterTime, int timeIndex, uint32 roundTrip)
{
    uint32 localLastSentTimestamp = lastSentTimestamp;
    int localLastSentTimeIndex = lastSentTimeIndex;

    if ((sync && timeIndex != localLastSentTimeIndex) || senderId != lastMasterID)
    {
        displayMessage("Wrong time index or master ID", MessageType_Error);
        return;
    }

    uint32 currentTimestamp = Time::getMillisecondCounter();
    uint32 localTimeDiff = currentTimestamp > localLastSentTimestamp ? currentTimestamp - localLastSentTimestamp : 0;
    int32 newTimeDiffToMaster = int32(int64(masterTime) - int64(currentTimestamp - localTimeDiff * 0.5));
    lastReceivedTimestamp = currentTimestamp;

    timeDiffHistory[timeDiffHistoryWritePosition] = newTimeDiffToMaster;
    timeDiffHistorySize = jmin(++timeDiffHistorySize, TIME_DIFF_HISTORY_SIZE);
    timeDiffHistoryWritePosition = (++timeDiffHistoryWritePosition) % TIME_DIFF_HISTORY_SIZE;

    // handle timing
    Array<int32> valueArray;
    for (int i = 0; i < timeDiffHistorySize; i++)
        valueArray.add(timeDiffHistory[i]);
    valueArray.sort();
    if (valueArray.size() == 0)
        resetTimeSync();
    else if (valueArray.size() % 2)
        relativeMsToMaster = valueArray[valueArray.size() / 2];
    else
        relativeMsToMaster = valueArray[valueArray.size() / 2] / 2 + valueArray[valueArray.size() / 2 - 1] / 2;
    lastRoundTrip = localTimeDiff;
    maxRoundTrip = jmax(lastRoundTrip, roundTrip);

#if(JUCE_DEBUG)
    displayMessage(String(relativeMsToMaster) + "; RT " + String(localTimeDiff) + "ms; MRT " + String(maxRoundTrip), MessageType_Info);
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

#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
bool Polytempo_TimeProvider::toggleMaster(bool master)
{
    // returns true if successful

    stopTimer();
    masterFlag = master;
    bool ok = Polytempo_InterprocessCommunication::getInstance()->reset(master);
    resetTimeSync();
    
    if (!masterFlag)
        startTimer(TIME_SYNC_INTERVAL_MS);

    return ok;
}

uint32 Polytempo_TimeProvider::getDelaySafeTimestamp()
{
    uint32 safeTime;
    getSyncTime(&safeTime);
    safeTime += maxRoundTrip + 200; // allow 200ms for calculation time

    return safeTime;
}

bool Polytempo_TimeProvider::isMaster() const
{
    return masterFlag;
}

void Polytempo_TimeProvider::setRemoteMasterPeer(String ip, Uuid id)
{
    if (masterFlag)
    {
        displayMessage("Another master detected", MessageType_Error);
        resetTimeSync();
        return;
    }

    // master ID check
    if (lastMasterID != id || !Polytempo_InterprocessCommunication::getInstance()->isClientConnected())
    {
        resetTimeSync();
        displayMessage("Master changed", MessageType_Warning);
        bool ok = Polytempo_InterprocessCommunication::getInstance()->connectToMaster(ip);
        if (!ok)
            displayMessage("Connecting to master " + ip + " failed!", MessageType_Error);
    }

    lastMasterID = id;
}


void Polytempo_TimeProvider::handleMessage(XmlElement message, Ipc* sender)
{
    NamedValueSet syncParams;
    syncParams.setFromXmlAttributes(message);

    if (message.getTagName() == "TimeSyncRequest")
    {
        Uuid senderId = Uuid(syncParams.getWithDefault("Id", ""));
        String senderScoreName = syncParams.getWithDefault("ScoreName", "");
        String senderPeerName = syncParams.getWithDefault("PeerName", "");
        int timeIndex = syncParams.getWithDefault("Index", 0);
        uint32 lastRoundTripFromClient = uint32(int32(syncParams.getWithDefault("LastRT", 0)));
        sender->setRemoteNames(senderScoreName, senderPeerName);

        if (masterFlag)
        {
            uint32 ts = Time::getMillisecondCounter();
            NamedValueSet replayParams;
            replayParams.set("Id", Polytempo_NetworkSupervisor::getInstance()->getUniqueId().toString());
            replayParams.set("ScoreName", Polytempo_NetworkSupervisor::getInstance()->getScoreName());
            replayParams.set("PeerName", Polytempo_NetworkSupervisor::getInstance()->getPeerName());
            replayParams.set("Timestamp", int32(ts));
            replayParams.set("Index", timeIndex);
            replayParams.set("MaxRT", int32(maxRoundTrip));
            XmlElement xml = XmlElement("TimeSyncReply");
            replayParams.copyToXmlAttributes(xml);
            bool ok = sender->sendMessage(Polytempo_InterprocessCommunication::xmlToMemoryBlock(xml));
            displayMessage(ok ? "Mastertime sent" : "Fail", ok ? MessageType_Info : MessageType_Error);

            // handle round trip time
            roundTripTime[roundTripHistoryWritePosition] = lastRoundTripFromClient;
            roundTripHistorySize = jmin(++roundTripHistorySize, ROUND_TRIP_HISTORY_SIZE);
            roundTripHistoryWritePosition = (++roundTripHistoryWritePosition) % ROUND_TRIP_HISTORY_SIZE;

            uint32 localMaxRoundTrip = 0;
            for (int i = 0; i < roundTripHistorySize; i++)
                localMaxRoundTrip = jmax(localMaxRoundTrip, roundTripTime[i]);
            maxRoundTrip = localMaxRoundTrip;
        }
        else
        {
            displayMessage("Wrong timeSyncRequest received", MessageType_Error);
        }
    }
    else if (message.getTagName() == "TimeSyncReply")
    {
        Uuid senderId = Uuid(syncParams.getWithDefault("Id", ""));
        String senderScoreName = syncParams.getWithDefault("ScoreName", "");
        String senderPeerName = syncParams.getWithDefault("PeerName", "");
        uint32 argMasterTime = uint32(int32(syncParams.getWithDefault("Timestamp", 0)));
        int timeIndex = syncParams.getWithDefault("Index", 0);
        uint32 maxRoundTripFromMaster = uint32(int32(syncParams.getWithDefault("MaxRT", 0)));
        sender->setRemoteNames(senderScoreName, senderPeerName);

        handleTimeSyncMessage(senderId, argMasterTime, timeIndex, maxRoundTripFromMaster);
    }
}
#endif

void Polytempo_TimeProvider::registerUserInterface(Polytempo_TimeSyncInfoInterface* pControl)
{
    pTimeSyncControl = pControl;
}

void Polytempo_TimeProvider::timerCallback()
{
#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
    if (!Polytempo_InterprocessCommunication::getInstance()->isClientConnected())
    {
        displayMessage("No master detected", MessageType_Error);
    }
    else
    {
        int index;
        uint32 timestamp;
        createTimeIndex(&index, &timestamp);

        NamedValueSet syncParams;
        syncParams.set("Id", Polytempo_NetworkSupervisor::getInstance()->getUniqueId().toString());
        syncParams.set("ScoreName", Polytempo_NetworkSupervisor::getInstance()->getScoreName());
        syncParams.set("PeerName", Polytempo_NetworkSupervisor::getInstance()->getPeerName());
        syncParams.set("Index", index);
        syncParams.set("LastRT", int32(lastRoundTrip));
        XmlElement xml = XmlElement("TimeSyncRequest");
        syncParams.copyToXmlAttributes(xml);
        bool ok = Polytempo_InterprocessCommunication::getInstance()->notifyServer(xml);
        if (!ok)
            displayMessage("No connection to master", MessageType_Error);
    }
#endif
}

#if defined(POLYTEMPO_NETWORK) || defined(POLYTEMPO_LIB)
void Polytempo_TimeProvider::displayMessage(String message, MessageType messageType) const
{
    if (pTimeSyncControl != nullptr)
    {
        pTimeSyncControl->showInfoMessage(messageType, message);
        return;
    }
    DBG(message);
}
#else
void Polytempo_TimeProvider::displayMessage(String, MessageType) const {}
#endif

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
    lastRoundTrip = 0;
    lastMasterID = Uuid::null();
}
