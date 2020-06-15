#include "Polytempo_MidiInput.h"

void Polytempo_MidiInput::handleIncomingMidiMessage(MidiInput*, const MidiMessage& message)
{
    DBG(message.getDescription());
}
