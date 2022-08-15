#pragma once

#include "../Scheduler/Polytempo_EventObserver.h"

class SynthAudioSource;

class Polytempo_AudioClick : public Polytempo_EventObserver
{
public:
    Polytempo_AudioClick();
    ~Polytempo_AudioClick() override;

    juce_DeclareSingleton(Polytempo_AudioClick, false)

    void setNumVoices(int num);
    void eventNotification(Polytempo_Event* event) override;
    void writeClickforEvent(Polytempo_Event* event, AudioBuffer<float> &buffer, int channel, int sampleRate, int sampleOffset = 0);

    int downbeatPitch, beatPitch, cuePitch;
    float downbeatVolume, beatVolume, cueVolume;

private:
    void getAudioParameter(int* pitch, float* volume, Polytempo_Event* event);
    
    MidiKeyboardState keyboardState;
    Synthesiser synth;
    AudioDeviceManager& audioDeviceManager;

    std::unique_ptr<AudioSourcePlayer> audioSourcePlayer;
    std::unique_ptr<SynthAudioSource> synthAudioSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_AudioClick)
};
