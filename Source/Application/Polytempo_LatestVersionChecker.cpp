#include "Polytempo_LatestVersionChecker.h"
#include "../Misc/Polytempo_Alerts.h"

JUCE_IMPLEMENT_SINGLETON (Polytempo_LatestVersionChecker)

Polytempo_LatestVersionChecker::Polytempo_LatestVersionChecker()
    : Thread ("VersionChecker")
{}

Polytempo_LatestVersionChecker::~Polytempo_LatestVersionChecker()
{
    stopThread (6000);
    clearSingletonInstance();
}

void Polytempo_LatestVersionChecker::checkForNewVersion(bool userInteraction)
{
    if (!isThreadRunning())
    {
        showAlert = userInteraction;
        startThread(3);
    }
}

static void downloadNewVersion(int result, String url)
{
    if(result)
    {
        URL(url).launchInDefaultBrowser();
    }
}

void Polytempo_LatestVersionChecker::run()
{
    versionString.clear();
    parsedAssets.clear();

    String applicationName = JUCEApplication::getInstance()->getApplicationName();

    if(!fetchFromUpdateServer())
    {
        if(showAlert)
            MessageManager::callAsync([applicationName]() {
                Polytempo_Alert::show("Error", "Failed to communicate with the update server");
            });
        return;
    }
    
    if(!newerVersionAvailable())
    {
        if(showAlert)
            MessageManager::callAsync([applicationName]() {
                Polytempo_Alert::show("No new version available", applicationName + " is up to date");
            });
        return;
    }
    
#if JUCE_MAC
    String requiredFilename = applicationName + "_MacOS_" + versionString + ".zip";
#elif JUCE_WINDOWS
    String requiredFilename = applicationName + "_Windows_" + versionString + ".zip";
#elif JUCE_LINUX
    String requiredFilename = applicationName + "_Linux_" + versionString + ".zip";
#else
    String requiredFilename = "";
#endif
 
    for(auto& asset : parsedAssets)
    {
        if(asset.name == requiredFilename)
        {
            Polytempo_OkCancelAlert::show("A new version of " + applicationName + " is available",
                                          "Do you want to download it?",
                                          ModalCallbackFunction::create(downloadNewVersion, asset.url));
           return;
        }
    }

    if (showAlert)
        MessageManager::callAsync([applicationName]() {
            Polytempo_Alert::show("Error", "Failed to find any new downloads. Please try again later.");
        });
}

bool Polytempo_LatestVersionChecker::fetchFromUpdateServer()
{
    URL serverURL ("https://api.github.com/repos/philippekocher/polytempo/releases/latest");
    std::unique_ptr<InputStream> inStream(serverURL.createInputStream (false, nullptr, nullptr, {}, 5000));

    if(inStream == nullptr)
        return false;
    
    auto details = JSON::parse(inStream->readEntireStreamAsString());
    auto* versionObject = details.getDynamicObject();

    if (versionObject == nullptr)
        return false;

    versionString = versionObject->getProperty("tag_name").toString();

    if (versionString.isEmpty())
        return false;

    auto* assets = versionObject->getProperty("assets").getArray();

    if (assets == nullptr)
        return false;

    releaseNotes = versionObject->getProperty("body").toString();

    for(auto& asset : *assets)
    {
        if(auto* assetObject = asset.getDynamicObject())
        {
            String name = assetObject->getProperty("name").toString();
            String url  = assetObject->getProperty("browser_download_url").toString();
            if(name.isNotEmpty() && url.isNotEmpty())
                parsedAssets.push_back({name, url});
        }
        else return false;
    }
    return true;
}

bool Polytempo_LatestVersionChecker::newerVersionAvailable()
{
    if(versionString.isEmpty())
        return false;

    auto currentVersionTokens = StringArray::fromTokens(JUCEApplication::getInstance()->getApplicationVersion(), ".", {});
    auto serverVersionTokens  = StringArray::fromTokens(versionString, ".", {});

    if(currentVersionTokens[0].getIntValue() == serverVersionTokens[0].getIntValue())
    {
        if (currentVersionTokens[1].getIntValue() == serverVersionTokens[1].getIntValue())
            return currentVersionTokens[2].getIntValue() < serverVersionTokens[2].getIntValue();

        return currentVersionTokens[1].getIntValue() < serverVersionTokens[1].getIntValue();
    }

    return currentVersionTokens[0].getIntValue() < serverVersionTokens[0].getIntValue();
}
