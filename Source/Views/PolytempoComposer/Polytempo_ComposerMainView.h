#pragma once

#include "JuceHeader.h"
#include "Polytempo_ComposerToolbarComponent.h"
#include "Polytempo_TransportComponent.h"
#include "Polytempo_SequencesViewport.h"
#include "Polytempo_TimeMapComponent.h"
#include "Polytempo_TempoMapComponent.h"
#include "Polytempo_BeatPatternListComponent.h"
#include "Polytempo_PointListComponent.h"



class Polytempo_ComposerMainView : public Component
{
public:
    Polytempo_ComposerMainView();
    ~Polytempo_ComposerMainView();

    void paint (Graphics&);
    void resized();
    void childBoundsChanged(Component* child);
    
    void setMapDimension(float, Rational);
    
    String getComponentStateAsString();
    void   restoreComponentStateFromString(const String&);
    
    enum componentType
    {
        componentType_None = 0,
        componentType_TimeMap,
        componentType_TempoMap,
        componentType_PatternList,
        componentType_PointList
    };
    
    
    void setLeftComponent(componentType type);
    componentType getLeftComponent();
    void setRightComponent(componentType type);
    
private:
    std::unique_ptr<Polytempo_ComposerToolbarComponent>  toolbarComponent;
    std::unique_ptr<Polytempo_TransportComponent>        transportComponent;
    Polytempo_SequencesViewport        sequencesViewport;
    
    Component                           leftComponent;      // split view
    Component                           rightComponent;
    
    std::unique_ptr<Polytempo_TimeMapComponent>    timeMapComponent;
    std::unique_ptr<Polytempo_TempoMapComponent>   tempoMapComponent;
    
    Polytempo_BeatPatternListComponent  beatPatternListComponent;
    Polytempo_PointListComponent        pointListComponent;
    
    StretchableLayoutManager    stretchableManager;
    std::unique_ptr<StretchableLayoutResizerBar> resizerBar;
    
    float resizerBarPosition;
    componentType leftComponentType = componentType_None;
    componentType rightComponentType = componentType_None;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Polytempo_ComposerMainView)
};
