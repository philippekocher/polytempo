set(CMAKE_BUILD_TYPE Release)

juce_add_gui_app(PolytempoComposer
    VERSION 0.0
    ICON_BIG ../Resources/icon.png
    ICON_SMALL ../Resources/icon.png
    DOCUMENT_EXTENSIONS ptsco
    COMPANY_NAME "ICST Institute for Computermusic and Sound Technology, Zurich"
    BUNDLE_ID net.icst.PolytempoComposer
    PRODUCT_NAME PolytempoComposer)

juce_generate_juce_header(PolytempoComposer)

set_target_properties(PolytempoComposer PROPERTIES CXX_STANDARD 11)

target_sources(PolytempoComposer PRIVATE
    ../Source/Polytempo_ComposerMain.cpp
    ../Source/Application/PolytempoComposer/Polytempo_ComposerApplication.h
    ../Source/Application/PolytempoComposer/Polytempo_ComposerApplication.cpp
    ../Source/Application/PolytempoComposer/Polytempo_ComposerMenuBarModel.h
    ../Source/Application/PolytempoComposer/Polytempo_ComposerMenuBarModel.cpp
    ../Source/Application/PolytempoComposer/Polytempo_ComposerWindow.h
    ../Source/Application/PolytempoComposer/Polytempo_ComposerWindow.cpp
    ../Source/Application/Polytempo_AboutWindow.h
    ../Source/Application/Polytempo_AboutWindow.cpp
    ../Source/Application/Polytempo_LatestVersionChecker.h
    ../Source/Application/Polytempo_LatestVersionChecker.cpp
    ../Source/Audio+Midi/Polytempo_AudioClick.h
    ../Source/Audio+Midi/Polytempo_AudioClick.cpp
    ../Source/Audio+Midi/Polytempo_MidiClick.h
    ../Source/Audio+Midi/Polytempo_MidiClick.cpp
    ../Source/Audio+Midi/Polytempo_MidiInput.h
    ../Source/Audio+Midi/Polytempo_MidiInput.cpp
    ../Source/Data/Polytempo_BeatPattern.h
    ../Source/Data/Polytempo_BeatPattern.cpp
    ../Source/Data/Polytempo_Composition.h
    ../Source/Data/Polytempo_Composition.cpp
    ../Source/Data/Polytempo_Score.h
    ../Source/Data/Polytempo_Score.cpp
    ../Source/Data/Polytempo_Sequence.h
    ../Source/Data/Polytempo_Sequence.cpp
    ../Source/Data/Polytempo_TempoInterpolation.h
    ../Source/Data/Polytempo_TempoInterpolation.cpp
    ../Source/Network/Polytempo_OSCSender.h
    ../Source/Network/Polytempo_OSCSender.cpp
    ../Source/Network/Polytempo_TimeProvider.h
    ../Source/Network/Polytempo_TimeProvider.cpp
    ../Source/Preferences/Polytempo_DefaultPreferences.h
    ../Source/Preferences/Polytempo_DefaultPreferences.cpp
    ../Source/Preferences/Polytempo_ComposerPreferencesPanel.h
    ../Source/Preferences/Polytempo_ComposerPreferencesPanel.cpp
    ../Source/Preferences/Polytempo_StoredPreferences.h
    ../Source/Preferences/Polytempo_StoredPreferences.cpp
    ../Source/Scheduler/Polytempo_Event.h
    ../Source/Scheduler/Polytempo_Event.cpp
    ../Source/Scheduler/Polytempo_EventDispatcher.h
    ../Source/Scheduler/Polytempo_EventDispatcher.cpp
    ../Source/Scheduler/Polytempo_EventObserver.h
    ../Source/Scheduler/Polytempo_EventObserver.cpp
    ../Source/Scheduler/Polytempo_ScoreScheduler.h
    ../Source/Scheduler/Polytempo_ScoreScheduler.cpp
    ../Source/Scheduler/Polytempo_ScoreSchedulerEngine.h
    ../Source/Scheduler/Polytempo_ScoreSchedulerEngine.cpp
    ../Source/Scheduler/Polytempo_EventScheduler.h
    ../Source/Scheduler/Polytempo_EventScheduler.cpp
    ../Source/Views/PolytempoComposer/Polytempo_ListComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_ListComponent.cpp
    ../Source/Views/PolytempoComposer/Polytempo_PointListComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_PointListComponent.cpp
    ../Source/Views/PolytempoComposer/Polytempo_BeatPatternListComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_BeatPatternListComponent.cpp
    ../Source/Views/PolytempoComposer/Polytempo_ComposerMainView.h
    ../Source/Views/PolytempoComposer/Polytempo_ComposerMainView.cpp
    ../Source/Views/PolytempoComposer/Polytempo_ComposerToolbarComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_ComposerToolbarComponent.cpp
    ../Source/Views/PolytempoComposer/Polytempo_CoordinateSystem.h
    ../Source/Views/PolytempoComposer/Polytempo_CoordinateSystem.cpp
    ../Source/Views/PolytempoComposer/Polytempo_Ruler.h
    ../Source/Views/PolytempoComposer/Polytempo_Ruler.cpp
    ../Source/Views/PolytempoComposer/Polytempo_SequenceGraphicalSettings.h
    ../Source/Views/PolytempoComposer/Polytempo_SequenceGraphicalSettings.cpp
    ../Source/Views/PolytempoComposer/Polytempo_SequencePlaybackSettings.h
    ../Source/Views/PolytempoComposer/Polytempo_SequencePlaybackSettings.cpp
    ../Source/Views/PolytempoComposer/Polytempo_SequencesViewport.h
    ../Source/Views/PolytempoComposer/Polytempo_SequencesViewport.cpp
    ../Source/Views/PolytempoComposer/Polytempo_TempoMapComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_TempoMapComponent.cpp
    ../Source/Views/PolytempoComposer/Polytempo_TimeMapComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_TimeMapComponent.cpp
    ../Source/Views/PolytempoComposer/Polytempo_TransportComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_TransportComponent.cpp
    ../Source/Views/PolytempoComposer/Polytempo_GraphicExportView.h
    ../Source/Views/PolytempoComposer/Polytempo_GraphicExportView.cpp
    ../Source/Views/PolytempoComposer/Polytempo_GraphicExportPage.h
    ../Source/Views/PolytempoComposer/Polytempo_GraphicExportPage.cpp
    ../Source/Views/PolytempoComposer/Polytempo_GraphicExportSettingsComponent.h
    ../Source/Views/PolytempoComposer/Polytempo_GraphicExportSettingsComponent.cpp)

target_compile_definitions(PolytempoComposer PRIVATE
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
    JUCE_LOG_ASSERTIONS=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    POLYTEMPO_COMPOSER=1)


juce_add_binary_data(PolytempoComposerData SOURCES
    ../Resources/adjustPositionBwd.png
    ../Resources/adjustPositionFwd.png
    ../Resources/adjustTimeBwd.png
    ../Resources/adjustTimeFwd.png
    ../Resources/icon.png)


target_link_libraries(PolytempoComposer PRIVATE
    PolytempoComposerData
    juce::juce_build_tools
    juce::juce_gui_extra
    juce::juce_osc
    juce::juce_opengl
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)