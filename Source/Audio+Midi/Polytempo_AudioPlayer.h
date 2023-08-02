#pragma once

#include "../Scheduler/Polytempo_EventObserver.h"

class EventAudioSource;

class Polytempo_AudioPlayer : public Polytempo_EventObserver
{
public:
    Polytempo_AudioPlayer();
    ~Polytempo_AudioPlayer() override;
    
    bool loadAudio(var audioID, String url);
    void playAudio(Polytempo_Event *event);
    
    void eventNotification(Polytempo_Event* event) override;
private:
    Synthesiser synth;
    AudioDeviceManager& audioDeviceManager;
    AudioFormatManager formatManager;
    
    std::unique_ptr<AudioSourcePlayer> eventAudioSourcePlayer;
    std::unique_ptr<EventAudioSource> eventAudioSource;
    
    int noteNumber = 0;
    HashMap<var, int> noteNumberHash;
};
