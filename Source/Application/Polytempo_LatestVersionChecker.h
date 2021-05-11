#pragma once

#include "JuceHeader.h"

class Polytempo_LatestVersionChecker :  public DeletedAtShutdown,
                                        private Thread
{
public:
    Polytempo_LatestVersionChecker();
    ~Polytempo_LatestVersionChecker() override;

    void checkForNewVersion(bool userInteraction);

    JUCE_DECLARE_SINGLETON_SINGLETHREADED_MINIMAL (Polytempo_LatestVersionChecker)

private:
    void run() override;
    bool fetchFromUpdateServer();
    bool newerVersionAvailable();

    bool showAlert = false;
    String versionString;
    String releaseNotes;
    struct Asset
    {
        const String name;
        const String url;
    };
    std::vector<Asset> parsedAssets;
};
