set(CMAKE_BUILD_TYPE Release)

juce_add_gui_app(PolytempoNetwork
    VERSION 0.0
    ICON_BIG ../Resources/icon.png
    ICON_SMALL ../Resources/icon.png
    DOCUMENT_EXTENSIONS ptsco
    COMPANY_NAME "ICST Institute for Computermusic and Sound Technology, Zurich"
    BUNDLE_ID net.icst.PolytempoNetwork
    PRODUCT_NAME PolytempoNetwork)

juce_generate_juce_header(PolytempoNetwork)

set_target_properties(PolytempoNetwork PROPERTIES CXX_STANDARD 11)

target_sources(PolytempoNetwork PRIVATE
    ../Source/Polytempo_NetworkMain.cpp
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkApplication.h
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkApplication.cpp
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkMenuBarModel.h
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkMenuBarModel.cpp
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkWindow.h
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkWindow.cpp
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
    ../Source/Data/Polytempo_Score.h
    ../Source/Data/Polytempo_Score.cpp
    ../Source/Network/Polytempo_OSCSender.h
    ../Source/Network/Polytempo_OSCSender.cpp
    ../Source/Network/Polytempo_InterprocessCommunication.h
    ../Source/Network/Polytempo_InterprocessCommunication.cpp
    ../Source/Network/Polytempo_IPAddress.h
    ../Source/Network/Polytempo_IPAddress.cpp
    ../Source/Network/Polytempo_NetworkInterfaceManager.h
    ../Source/Network/Polytempo_NetworkInterfaceManager.cpp
    ../Source/Network/Polytempo_NetworkSupervisor.h
    ../Source/Network/Polytempo_NetworkSupervisor.cpp
    ../Source/Network/Polytempo_OSCListener.h
    ../Source/Network/Polytempo_OSCListener.cpp
    ../Source/Network/Polytempo_TimeProvider.h
    ../Source/Network/Polytempo_TimeProvider.cpp
    ../Source/Network/Polytempo_PortDefinition.h
    ../Source/Preferences/Polytempo_DefaultPreferences.h
    ../Source/Preferences/Polytempo_DefaultPreferences.cpp
    ../Source/Preferences/Polytempo_NetworkPreferencesPanel.h
    ../Source/Preferences/Polytempo_NetworkPreferencesPanel.cpp
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
    ../Source/Views/PolytempoNetwork/Polytempo_AnnotationView.h
    ../Source/Views/PolytempoNetwork/Polytempo_AnnotationView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_ScoreEditorView.h
    ../Source/Views/PolytempoNetwork/Polytempo_ScoreEditorView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationLayer.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationLayer.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsPalette.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsPalette.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_AuxiliaryView.h
    ../Source/Views/PolytempoNetwork/Polytempo_AuxiliaryView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotation.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotation.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationManager.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationManager.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationSet.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationSet.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationSettingsDialog.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationSettingsDialog.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsView.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsViewRegion.h
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsViewRegion.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_PageEditorView.h
    ../Source/Views/PolytempoNetwork/Polytempo_PageEditorView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_PageEditorComponent.h
    ../Source/Views/PolytempoNetwork/Polytempo_PageEditorComponent.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_SectionInstancesComponent.h
    ../Source/Views/PolytempoNetwork/Polytempo_SectionInstancesComponent.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_RegionEditorView.h
    ../Source/Views/PolytempoNetwork/Polytempo_RegionEditorView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_ImageManager.h
    ../Source/Views/PolytempoNetwork/Polytempo_ImageManager.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_NetworkInfoView.h
    ../Source/Views/PolytempoNetwork/Polytempo_NetworkInfoView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_NetworkMainView.h
    ../Source/Views/PolytempoNetwork/Polytempo_NetworkMainView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_Progressbar.h
    ../Source/Views/PolytempoNetwork/Polytempo_Progressbar.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_TimeSyncControl.h
    ../Source/Views/PolytempoNetwork/Polytempo_TimeSyncControl.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_VisualMetro.h
    ../Source/Views/PolytempoNetwork/Polytempo_VisualMetro.cpp)

target_compile_definitions(PolytempoNetwork PRIVATE
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
    JUCE_LOG_ASSERTIONS=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_MODAL_LOOPS_PERMITTED=1
    POLYTEMPO_NETWORK=1)


juce_add_binary_data(PolytempoNetworkData SOURCES
    ../Resources/colorPalette.png
    ../Resources/delete.png
    ../Resources/fontSize.png
    ../Resources/icon.png
    ../Resources/lineWeight.png
    ../Resources/no.png
    ../Resources/settings.png
    ../Resources/textSize.png
    ../Resources/transparency.png
    ../Resources/yes.png)


target_link_libraries(PolytempoNetwork PRIVATE
    PolytempoNetworkData
    juce::juce_build_tools
    juce::juce_gui_extra
    juce::juce_osc
    juce::juce_opengl
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
