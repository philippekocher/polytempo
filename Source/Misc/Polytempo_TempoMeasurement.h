/* ==============================================================================
 
 This file is part of the POLYTEMPO software package.
 Copyright (c) 2016 - Zurich University of the Arts,
 ICST Institute for Computer Music and Sound Technology
 http://www.icst.net
 
 Author: Philippe Kocher
 
 POLYTEMPO is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 POLYTEMPO is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this software. If not, see <http://www.gnu.org/licenses/>.
 
 ============================================================================== */

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
