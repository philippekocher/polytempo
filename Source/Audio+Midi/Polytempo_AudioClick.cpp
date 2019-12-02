#include "Polytempo_AudioClick.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "Polytempo_AudioDeviceManager.h"


class SineWaveSound : public SynthesiserSound
{
public:
    SineWaveSound()     {}
    
    bool appliesToNote (int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel (int /*midiChannel*/) override { return true; }
};


class SineWaveVoice : public SynthesiserVoice
{
public:
    SineWaveVoice()
		: currentAngle(0), angleDelta(0.0), level(0), tailOff(0.0)
	{
	}

    bool canPlaySound(SynthesiserSound* sound) override
    {
        return dynamic_cast <SineWaveSound*> (sound) != nullptr;
    }
    
    void setChannel(int ch)
    {
        audioCh0 = ch;
        audioCh1 = ch;
    }
    
    void startNote(int midiNoteNumber, float velocity,
                   SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 1.0;
        
        double cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        double cyclesPerSample = cyclesPerSecond / getSampleRate();
        
        angleDelta = cyclesPerSample * 2.0 * double_Pi;
    }
    
    void stopNote (float /*velocity*/, bool /*allowTailOff*/) override
    {
        if (tailOff == 0.0) tailOff = 1.0;
        // we only need to begin a tail-off if it's not already doing so
    }
    
    void pitchWheelMoved (int /*newValue*/) override
    {}
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {}
    
    void renderNextBlock(AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            int ch0 = audioCh0 > outputBuffer.getNumChannels() - 1 ? 0 : audioCh0;
            int ch1 = audioCh1 > outputBuffer.getNumChannels() - 1 ? 1 : audioCh1;
            
            if(tailOff > 0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (sin (currentAngle) * level * tailOff);
                    
                    outputBuffer.addSample(ch0, startSample, currentSample);
                    outputBuffer.addSample(ch1, startSample, currentSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                    
                    tailOff *= 0.999;
                    
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (sin (currentAngle) * level);
                    
                    outputBuffer.addSample(ch0, startSample, currentSample);
                    outputBuffer.addSample(ch1, startSample, currentSample);
                    
                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }
    
private:
    double currentAngle, angleDelta, level, tailOff;
    int audioCh0 = 0, audioCh1 = 1;
};

class SynthAudioSource : public AudioSource
{
public:
    MidiKeyboardState& keyboardState;
    Synthesiser& synth;
    
    SynthAudioSource(MidiKeyboardState& aKeyboardState, Synthesiser& aSynth)
    : keyboardState(aKeyboardState), synth(aSynth)
    {
        synth.addSound(new SineWaveSound());
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate(sampleRate);
    }
    
    void releaseResources() override
    {}
    
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        // the synth adds its output to the audio buffer, so we have to clear it first
        bufferToFill.clearActiveBufferRegion();
        
        MidiBuffer incomingMidi;
        keyboardState.processNextMidiBuffer(incomingMidi, 0, bufferToFill.numSamples, true);
        
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }
};

Polytempo_AudioClick::Polytempo_AudioClick()
:audioDeviceManager(Polytempo_AudioDeviceManager::getSharedAudioDeviceManager())
{
    synthAudioSource.reset(new SynthAudioSource(keyboardState, synth));
    audioSourcePlayer.reset(new AudioSourcePlayer());
    audioSourcePlayer->setSource(synthAudioSource.get());
    
    audioDeviceManager.addAudioCallback(audioSourcePlayer.get());
    
    // add one voice to our synth
    synth.addVoice(new SineWaveVoice());
    
    downbeatPitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("audioDownbeatPitch");
    beatPitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("audioBeatPitch");
    cuePitch = Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("audioCuePitch");
    downbeatVolume = (float)Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("audioDownbeatVolume");
    beatVolume = (float)Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("audioBeatVolume");
    cueVolume = (float)Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("audioCueVolume");
}

Polytempo_AudioClick::~Polytempo_AudioClick()
{
    audioSourcePlayer->setSource(0);
    audioDeviceManager.removeAudioCallback(audioSourcePlayer.get());
    audioDeviceManager.closeAudioDevice();
    
    audioSourcePlayer = nullptr;
    synthAudioSource = nullptr;
    
    Polytempo_AudioDeviceManager::clearSharedAudioDeviceManager();
    
    clearSingletonInstance();
}

juce_ImplementSingleton(Polytempo_AudioClick);

void Polytempo_AudioClick::setNumVoices(int num)
{
    while(num > synth.getNumVoices())
        synth.addVoice(new SineWaveVoice());
}

void Polytempo_AudioClick::eventNotification(Polytempo_Event *event)
{
    if(event->getType() == eventType_Beat)
    {
        // take pitch and volume from the event's properties
        if(!event->getProperty("audioPitch").isVoid())
        {
            int pitch = event->getProperty("audioPitch");
            float volume = event->getProperty("audioVolume");
            
            keyboardState.noteOn(1, pitch, volume);
            keyboardState.noteOff(1, pitch, 0);
        }
#ifdef POLYTEMPO_NETWORK
        // or take default values if audio is turned on
        else if(Polytempo_StoredPreferences::getInstance()->getProps().getBoolValue("audioClick"))
        {
            int pattern;
            int pitch;
            float volume;
            
            if(event->hasProperty(eventPropertyString_Pattern))
                pattern = event->getProperty(eventPropertyString_Pattern);
            else
                pattern = eventPropertyDefault_Pattern;

            if(pattern < 3) return;
            
            if(int(event->getProperty(eventPropertyString_Cue)) != 0)
            {
                pitch = cuePitch;
                volume = cueVolume;
            }
            else
            {
                if(pattern < 20)
                {
                    pitch = downbeatPitch;
                    volume = downbeatVolume;
                }
                else
                {
                    pitch = beatPitch;
                    volume = beatVolume;
                }
            }
            
            keyboardState.noteOn(1, pitch, volume);
            keyboardState.noteOff(1, pitch, 0);
        }
#endif
    }
}
