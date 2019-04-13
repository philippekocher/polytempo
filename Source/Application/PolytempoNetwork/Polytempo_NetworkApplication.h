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


#include "Polytempo_NetworkWindow.h"
#include "Polytempo_NetworkMenuBarModel.h"
#include "../../Network/Polytempo_OSCListener.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Audio+Midi/Polytempo_MidiInput.h"
#include "../../Misc/Polytempo_Alerts.h"
#include "../../Network/Polytempo_BroadcastWrapper.h"

#define OSC_PORT_TIME_SYNC			9999
#define OSC_PORT_COMMUNICATION		47522

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
    bool scoreFileExists() { return scoreFile.exists(); }
    Polytempo_NetworkWindow* getMainWindow() { return mainWindow; }
    
    void unsavedChangesAlert(Polytempo_YesNoCancelAlert::callbackTag);
    void newScore();
    void openFileDialog();
    void openScoreFilePath(String filePath = String());
    void openScoreFile(File aFile = File());
    void saveScoreFile(bool showFileDialog);
    void commandStatusChanged();
    
    static ApplicationCommandManager* getCommandManager();

private:
	void saveAs(File targetFile);

private:
	ScopedPointer<Polytempo_NetworkWindow> mainWindow;
    ScopedPointer<Polytempo_MenuBarModel> menuBarModel;
    ScopedPointer<Polytempo_BroadcastWrapper> broadcastWrapper;
    ScopedPointer<Polytempo_OSCListener> oscListener;
    ScopedPointer<Polytempo_MidiInput> midiInput;
    ScopedPointer<Polytempo_Score> score;
	ScopedPointer<FileLogger> fileLogger;

#ifdef JUCE_ANDROID
	ScopedPointer<FileChooser> fc;
#endif

    ApplicationCommandManager commandManager;
    File scoreFile, newScoreFile;
    LookAndFeel_V3 lookAndFeelV3;
};


#endif  // __Polytempo_NetworkApplication__
