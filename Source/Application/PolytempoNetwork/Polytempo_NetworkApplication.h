#pragma once

#include "Polytempo_NetworkWindow.h"
#include "Polytempo_NetworkMenuBarModel.h"
#include "../../Network/Polytempo_OSCListener.h"
#include "../../Data/Polytempo_Score.h"
#include "../../Audio+Midi/Polytempo_MidiInput.h"
#include "../../Misc/Polytempo_Alerts.h"

#define OSC_PORT_COMMUNICATION		47522

class Polytempo_NetworkApplication : public JUCEApplication
{
public:
    Polytempo_NetworkApplication();

    const String getApplicationName() override { return ProjectInfo::projectName; }
    const String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const String& commandLine) override;
    void shutdown() override;
    void systemRequestedQuit() override;
    void applicationShouldQuit();
    bool quitApplication = false;
    void anotherInstanceStarted(const String& commandLine) override;

    Polytempo_Score* getScore() { return score.get(); };
    bool scoreFileExists() { return scoreFile.exists(); }
    Polytempo_NetworkWindow* getMainWindow() { return mainWindow.get(); }

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
    std::unique_ptr<Polytempo_NetworkWindow> mainWindow;
    std::unique_ptr<Polytempo_MenuBarModel> menuBarModel;
    std::unique_ptr<Polytempo_OSCListener> oscListener;
    std::unique_ptr<Polytempo_MidiInput> midiInput;
    std::unique_ptr<Polytempo_Score> score;
    std::unique_ptr<FileLogger> fileLogger;

#ifdef JUCE_ANDROID
	std::unique_ptr<FileChooser> fc;
#endif

    ApplicationCommandManager commandManager;
    File scoreFile, newScoreFile;
    LookAndFeel_V3 lookAndFeelV3;
};
