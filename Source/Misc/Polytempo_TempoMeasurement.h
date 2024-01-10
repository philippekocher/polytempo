#pragma once

#include "../Preferences/Polytempo_StoredPreferences.h"


class Polytempo_TempoMeasurement
{
public:
    static float decodeTempoForUI(float absoluteTempo)
    {
        String defaultBeatPattern = Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurement");
        
        StringArray tokens;
        tokens.addTokens(defaultBeatPattern, false);

        // round to max. 3 decimal places
        float value = absoluteTempo / Rational(tokens[0]).toFloat() * tokens[1].getFloatValue();
        value = (float)(int(value * 1000.0f + 0.5f));
        return value / 1000.0f;
    }

    static float encodeTempoFromUI(float displayedTempo)
    {
        String defaultBeatPattern = Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurement");
        
        StringArray tokens;
        tokens.addTokens(defaultBeatPattern, false);

        return displayedTempo * Rational(tokens[0]).toFloat() / tokens[1].getFloatValue();
    }
    
    static String getMeasurementAsString()
    {
        String defaultBeatPattern = Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurement");
        
        StringArray tokens;
        tokens.addTokens(defaultBeatPattern, false);

        return tokens[0]+String(" per ")+tokens[1]+String("s");
    }
};
