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

        return absoluteTempo / Rational(tokens[0]).toFloat() * tokens[1].getFloatValue();
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
