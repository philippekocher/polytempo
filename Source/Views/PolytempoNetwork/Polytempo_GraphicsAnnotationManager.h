#pragma once

#include "JuceHeader.h"
#include "Polytempo_GraphicsAnnotation.h"
#include "Polytempo_GraphicsAnnotationSet.h"
#include "Polytempo_GraphicsAnnotationLayer.h"
#include "../../Preferences/Polytempo_StoredPreferences.h"

class Polytempo_GraphicsAnnotationManager : public ChangeBroadcaster, public ChangeListener
{
public:
    juce_DeclareSingleton(Polytempo_GraphicsAnnotationManager, false)

    enum eAnnotationMode
    {
        Off,
        ReadOnly,
        Standard,
        Edit
    };

    void initialize(String folder, String scoreName);
    void showSettingsDialog();

    void getAnnotationsForImage(String imageId, OwnedArray<Polytempo_GraphicsAnnotation>* pAnnotations) const;
    void createAndAddNewLayer(bool editable);
    void addAnnotation(Polytempo_GraphicsAnnotation annotation);
    void saveAll() const;
    bool getAnchorFlag() const;

    void changeListenerCallback(ChangeBroadcaster*) override;
    bool getAnnotation(Uuid id, Polytempo_GraphicsAnnotation* pAnnotation);
    bool removeAnnotation(Uuid id);
    bool isAnnotationPending() const;
    bool trySetAnnotationPending(Polytempo_GraphicsAnnotationLayer* pEditableRegion);
    void resetAnnotationPending(Polytempo_GraphicsAnnotationLayer* pEditableRegion);
    Polytempo_GraphicsAnnotationLayer* getCurrentPendingAnnotationLayer() const;
    void setAnnotationMode(eAnnotationMode mode);
    eAnnotationMode getAnnotationMode() const;
    bool currentDirectoryExists();

private:
    Polytempo_GraphicsAnnotationManager() : annotationMode(ReadOnly), annotationPending(false), pCurrentPendingAnnotationLyer(nullptr)
    {
        annotationMode = eAnnotationMode(Polytempo_StoredPreferences::getInstance()->getProps().getIntValue("annotationMode", Standard));
    }

    ~Polytempo_GraphicsAnnotationManager() override
    {
        Polytempo_StoredPreferences::getInstance()->getProps().setValue("annotationMode", annotationMode);
    }

private:
    OwnedArray<Polytempo_GraphicsAnnotationSet> annotationSets;
    std::unique_ptr<File> currentDirectory;
    String currentScoreName;
    eAnnotationMode annotationMode;
    bool annotationPending;
    CriticalSection csPendingAnnotation;
    Polytempo_GraphicsAnnotationLayer* pCurrentPendingAnnotationLyer;
};
