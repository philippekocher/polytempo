#include "Polytempo_AudioPlayer.h"

#include "Polytempo_AudioDeviceManager.h"
#include "../Preferences/Polytempo_StoredPreferences.h"
#include "../Misc/Polytempo_Alerts.h"


class EventAudioSource : public AudioSource
{
public:
    Synthesiser& synth;

    EventAudioSource(Synthesiser& aSynth) : synth(aSynth)
    {
    }

    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate(sampleRate);
    }

    void releaseResources() override
    {
    }

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        // the synth adds its output to the audio buffer, so we have to clear it first
        bufferToFill.clearActiveBufferRegion();

        MidiBuffer incomingMidi;
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }
};

Polytempo_AudioPlayer::Polytempo_AudioPlayer() : audioDeviceManager(Polytempo_AudioDeviceManager::getSharedAudioDeviceManager())
{
    formatManager.registerBasicFormats();
    
    eventAudioSource.reset(new EventAudioSource(synth));
    eventAudioSourcePlayer.reset(new AudioSourcePlayer());
    eventAudioSourcePlayer->setSource(eventAudioSource.get());

    audioDeviceManager.addAudioCallback(eventAudioSourcePlayer.get());

    for(int i=0; i<10; i++) synth.addVoice(new SamplerVoice());
}

Polytempo_AudioPlayer::~Polytempo_AudioPlayer()
{
    eventAudioSourcePlayer->setSource(nullptr);
    audioDeviceManager.removeAudioCallback(eventAudioSourcePlayer.get());
    audioDeviceManager.closeAudioDevice();

    eventAudioSource = nullptr;
    eventAudioSourcePlayer = nullptr;
}

bool Polytempo_AudioPlayer::loadAudio(var audioID, String url)
{
    File directory(Polytempo_StoredPreferences::getInstance()->getProps().getValue("scoreFileDirectory"));
    File file(directory.getChildFile(url));

    if (!file.existsAsFile())
    {
        Polytempo_Alert::show("Error", "Can't open file:\n" + directory.getChildFile(url).getFullPathName());
        return false;
    }
    
    std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));

    if(reader)
    {
        noteNumber++;
        BigInteger usedNotes;
        usedNotes.setRange(noteNumber, noteNumber + 1, true);
        synth.addSound(new SamplerSound(audioID, *reader, usedNotes, noteNumber, 0.0f, 0.1f, 999.0));
        noteNumberHash.set(audioID, noteNumber);
        return true;
    }
    
    return false;
}

void Polytempo_AudioPlayer::playAudio(Polytempo_Event *event)
{
    var audioID(event->getProperty(eventPropertyString_AudioID));
    float gain = 1.0;
    if(event->hasProperty(eventPropertyString_Gain))
        gain = event->getProperty(eventPropertyString_Gain);
    
    if (noteNumberHash.contains(audioID))
    {
        synth.noteOn(1, noteNumberHash[audioID], gain);
    }
}

void Polytempo_AudioPlayer::eventNotification(Polytempo_Event* event)
{
    if (event->getType() == eventType_DeleteAll)
    {
        synth.clearSounds();
    }
    else if (event->getType() == eventType_LoadAudio)
    {
        var audioID(event->getProperty(eventPropertyString_AudioID));
        String url(event->getProperty(eventPropertyString_URL).toString());
        MessageManager::callAsync([this, audioID, url]() { loadAudio(audioID, url); });
    }
    else if (event->getType() == eventType_Audio)
    {
        playAudio(event);
    }
    else if (event->getType() == eventType_Stop || event->getType() == eventType_GotoTime)
    {
        synth.allNotesOff(1, true);
    }
}
