set(CMAKE_BUILD_TYPE Release)

juce_add_gui_app(PolytempoNetwork
    VERSION 0.5
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
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkApplication.cpp
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkMenuBarModel.cpp
    ../Source/Application/PolytempoNetwork/Polytempo_NetworkWindow.cpp
    ../Source/Application/Polytempo_AboutWindow.cpp
    ../Source/Application/Polytempo_LatestVersionChecker.cpp
    ../Source/Audio+Midi/Polytempo_AudioClick.cpp
    ../Source/Audio+Midi/Polytempo_MidiClick.cpp
    ../Source/Audio+Midi/Polytempo_MidiInput.cpp
    ../Source/Data/Polytempo_Score.cpp
    ../Source/Network/Polytempo_OSCSender.cpp
    ../Source/Network/Polytempo_InterprocessCommunication.cpp
    ../Source/Network/Polytempo_IPAddress.cpp
    ../Source/Network/Polytempo_NetworkInterfaceManager.cpp
    ../Source/Network/Polytempo_NetworkSupervisor.cpp
    ../Source/Network/Polytempo_OSCListener.cpp
    ../Source/Network/Polytempo_TimeProvider.cpp
    ../Source/Preferences/Polytempo_DefaultPreferences.cpp
    ../Source/Preferences/Polytempo_NetworkPrefenercesPanel.cpp
    ../Source/Preferences/Polytempo_StoredPreferences.cpp
    ../Source/Scheduler/Polytempo_Event.cpp
    ../Source/Scheduler/Polytempo_EventDispatcher.cpp
    ../Source/Scheduler/Polytempo_EventObserver.cpp
    ../Source/Scheduler/Polytempo_ScoreScheduler.cpp
    ../Source/Scheduler/Polytempo_ScoreSchedulerEngine.cpp
    ../Source/Scheduler/Polytempo_EventScheduler.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_AnnotationView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_ScoreEditorView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationLayer.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsPalette.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_AuxiliaryView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotation.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationManager.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationSet.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsAnnotationSettingsDialog.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_GraphicsViewRegion.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_PageEditorView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_PageEditorComponent.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_SectionInstancesComponent.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_RegionEditorView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_ImageManager.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_NetworkInfoView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_NetworkMainView.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_Progressbar.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_TimeSyncControl.cpp
    ../Source/Views/PolytempoNetwork/Polytempo_VisualMetro.cpp)

target_compile_definitions(PolytempoNetwork PRIVATE
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
    JUCE_LOG_ASSERTIONS=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
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
