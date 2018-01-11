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
        Rational tempoMeasurementUnit = Rational(Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurementUnit"));
        float tempoMeasurementTime = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMeasurementTime");
        return absoluteTempo / tempoMeasurementUnit.toFloat() * tempoMeasurementTime;
    }

    static float encodeTempoFromUI(float displayedTempo)
    {
        Rational tempoMeasurementUnit = Rational(Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurementUnit"));
        float tempoMeasurementTime = Polytempo_StoredPreferences::getInstance()->getProps().getDoubleValue("tempoMeasurementTime");
        return displayedTempo * tempoMeasurementUnit.toFloat() / tempoMeasurementTime;
    }
    
    static String getMeasurementAsString()
    {
        String unitString = Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurementUnit");
        String timeString = Polytempo_StoredPreferences::getInstance()->getProps().getValue("tempoMeasurementTime");
        return unitString+String(" per ")+timeString+String("s");
    }
};
