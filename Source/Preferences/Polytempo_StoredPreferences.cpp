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

#include "Polytempo_StoredPreferences.h"
#include "Polytempo_DefaultPreferences.h"


//==============================================================================
Polytempo_StoredPreferences::Polytempo_StoredPreferences()
{
    defaults.reset((PropertiesFile *) new Polytempo_DefaultPreferences());
    flush();
}

Polytempo_StoredPreferences::~Polytempo_StoredPreferences()
{
    flush();
    props = nullptr;
    defaults = nullptr;
    clearSingletonInstance();
}

juce_ImplementSingleton (Polytempo_StoredPreferences);


PropertiesFile& Polytempo_StoredPreferences::getProps()
{
    return *props;
}

void Polytempo_StoredPreferences::flush()
{
    if (props != nullptr)
    {
        props->setValue("recentFiles", recentFiles.toString());
    }
    else
    {
        PropertiesFile::Options options;
        options.folderName           = "Polytempo/"+JUCEApplication::getInstance()->getApplicationName();
        options.applicationName      = "settings";
        options.filenameSuffix       = "xml";
        options.osxLibrarySubFolder  = "Application Support";

        props.reset(new PropertiesFile(options));
        props->setFallbackPropertySet(defaults.get());
    }

    // recent files
    recentFiles.restoreFromString(props->getValue("recentFiles"));
    recentFiles.removeNonExistentFiles();
}