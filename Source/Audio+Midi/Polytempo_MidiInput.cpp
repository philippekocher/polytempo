#include "Polytempo_MidiInput.h"

#ifdef JUCE_DEBUG
void Polytempo_MidiInput::handleIncomingMidiMessage(MidiInput*, const MidiMessage& message)
#else
void Polytempo_MidiInput::handleIncomingMidiMessage(MidiInput*, const MidiMessage&)
#endif
{
    DBG(message.getDescription());
}
