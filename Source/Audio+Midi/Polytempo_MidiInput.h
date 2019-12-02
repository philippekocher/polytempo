#ifndef __Polytempo_MidiInput__
#define __Polytempo_MidiInput__


#include "Polytempo_AudioDeviceManager.h"


class Polytempo_MidiInput : public MidiInputCallback

{
public:
    Polytempo_MidiInput()
//    :audioDeviceManager(Polytempo_AudioDeviceManager::getSharedAudioDeviceManager())
    {
//        const StringArray list (MidiInput::getDevices());
//        const String newInput (list[1]);
//        
//        if (! audioDeviceManager.isMidiInputEnabled (newInput))
//            audioDeviceManager.setMidiInputEnabled (newInput, true);
//        
//        audioDeviceManager.addMidiInputCallback (newInput, this);
    }
    ~Polytempo_MidiInput()
    {
//        audioDeviceManager.removeMidiInputCallback (MidiInput::getDevices()[1], this);
    }
    
    void handleIncomingMidiMessage(MidiInput*, const MidiMessage& message)
    {
		DBG(message.getDescription());
    }
    
private:
//    AudioDeviceManager& audioDeviceManager;
    
};



#endif  // __Polytempo_MidiInput__
