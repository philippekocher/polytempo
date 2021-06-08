#pragma once

#include "../Preferences/Polytempo_StoredPreferences.h"

static AudioDeviceManager* sharedAudioDeviceManager;

class Polytempo_AudioDeviceManager
{
public:
    static AudioDeviceManager& getSharedAudioDeviceManager()
    {
        if (sharedAudioDeviceManager == nullptr)
        {
            sharedAudioDeviceManager = new AudioDeviceManager();
            std::unique_ptr<XmlElement> audioDeviceManagerState = Polytempo_StoredPreferences::getInstance()->getProps().getXmlValue("audioDevice");
            sharedAudioDeviceManager->initialise(0, 2,
                                                 nullptr,
                                                 true, String(), nullptr);
        }

        return *sharedAudioDeviceManager;
    }

    static void clearSharedAudioDeviceManager()
    {
        delete sharedAudioDeviceManager;
    }
};
