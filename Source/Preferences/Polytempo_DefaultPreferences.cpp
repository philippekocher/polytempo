#include "Polytempo_DefaultPreferences.h"

Polytempo_DefaultPreferences::Polytempo_DefaultPreferences()
{
    // composer
    setValue("zoomX", 50);
    setValue("timeMapZoomY", 40);
    setValue("tempoMapZoomY", 700);
    setValue("mainWindowContent", "0.5 1 3");
    setValue("tempoMeasurement", "1/4 60");
    setValue("defaultBeatPattern", "4/4 1");

    // network
    setValue("showVisualMetro", 1);
    setValue("stripWidth", 30);
    setValue("stripNormalColour", "ff000000");
    setValue("stripCueColour", "ffff0000");
    setValue("stripBackgroundColour", "ffeeeeee");
    setValue("stripFrameColour", "ff000000");
    setValue("showAuxiliaryView", 1);
    setValue("zoom", 0.4);

    // common
    setValue("midiDownbeatPitch", 100);
    setValue("midiDownbeatVelocity", 127);
    setValue("midiBeatPitch", 95);
    setValue("midiBeatVelocity", 127);
    setValue("midiCuePitch", 90);
    setValue("midiCueVelocity", 127);
    setValue("midiChannel", 1);

    setValue("audioDownbeatPitch", 100);
    setValue("audioDownbeatVolume", 1.0);
    setValue("audioBeatPitch", 95);
    setValue("audioBeatVolume", 1.0);
    setValue("audioCuePitch", 90);
    setValue("audioCueVolume", 1.0);

    setValue("annotationMode", 1);
    setValue("checkForNewVersion", 1);
}

Polytempo_DefaultPreferences::~Polytempo_DefaultPreferences()
{
    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_DefaultPreferences)
