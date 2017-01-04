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

#ifndef __Polytempo_NetworkApplication__
#define __Polytempo_NetworkApplication__

//#include "../../JuceLibraryCode/JuceHeader.h"

#include "Polytempo_NetworkWindow.h"
#include "Polytempo_NetworkMenuBarModel.h"
#include "../../Network/Polytempo_OSCSender.h"
#include "../../Network/Polytempo_OSCListener.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Audio+Midi/Polytempo_MidiInput.h"


class Polytempo_NetworkApplication : public JUCEApplication
{
public:
    Polytempo_NetworkApplication();
    
    const String getApplicationName()       { return ProjectInfo::projectName; }
    const String getApplicationVersion()    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()       { return false; }

    void initialise(const String& commandLine);
    void shutdown();
    void systemRequestedQuit();
    void applicationShouldQuit();
    bool quitApplication = false;
    void anotherInstanceStarted(const String& commandLine);
    
    Polytempo_Score* getScore() { return score; };

    void openFileDialog();

    void openScoreFilePath(String filePath = String::empty);
    void openScoreFile(File newScoreFile = File::nonexistent);
    void saveScoreFile(bool showFileDialog);
    
private:
    ScopedPointer<Polytempo_NetworkWindow> mainWindow;
    ScopedPointer<Polytempo_MenuBarModel> menuBarModel;
    ScopedPointer<Polytempo_OSCSender> oscSender;
    ScopedPointer<Polytempo_OSCListener> oscListener;
    ScopedPointer<Polytempo_MidiInput> midiInput;
    ScopedPointer<Polytempo_Score> score;
    
    File scoreFile;
};


#endif  // __Polytempo_NetworkApplication__
