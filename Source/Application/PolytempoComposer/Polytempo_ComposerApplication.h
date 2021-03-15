#pragma once

#include "JuceHeader.h"
#include "Polytempo_ComposerWindow.h"
#include "../../Network/Polytempo_OSCSender.h"

class Polytempo_ComposerApplication : public JUCEApplication
{
public:
    Polytempo_ComposerApplication();
    
    const String getApplicationName() override      { return ProjectInfo::projectName; }
    const String getApplicationVersion() override   { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override      { return false; }
    
    void initialise (const String& commandLine) override;
    void shutdown() override;
    void systemRequestedQuit() override;
    void applicationShouldQuit();
    void anotherInstanceStarted(const String& commandLine) override;

    static ApplicationCommandManager& getCommandManager();
    static DocumentWindow& getDocumentWindow();
    static Polytempo_ComposerMainView& getMainView();
    
private:
    std::unique_ptr<Polytempo_ComposerWindow> composerWindow;
    std::unique_ptr<ApplicationCommandManager> commandManager;
    std::unique_ptr<Polytempo_OSCSender> oscSender;
};
