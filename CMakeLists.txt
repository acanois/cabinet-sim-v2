cmake_minimum_required(VERSION 3.24.1)

set(PROJECT_NAME "CabinetSim")

set(FORMATS AU Standalone)

file(STRINGS VERSION CURRENT_VERSION)

project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

set(CMAKE_XCODE_GENERATE_SCHEME OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Support macOS down to High Sierra")

add_subdirectory(JUCE)

juce_add_plugin("${PROJECT_NAME}"
    # VERSION ...
    # ICON_BIG ...
    # ICON_SMALL ...
    COMPANY_NAME acanois
    BUNDLE_ID com.acanois.acanois
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    # NEEDS_MIDI_OUTPUT TRUE/FALSE
    # IS_MIDI_EFFECT TRUE/FALSE
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE  # copied to /Users/<user>/Library/Audio/Plug-Ins/
    PLUGIN_MANUFACTURER_CODE Acan
    PLUGIN_CODE P001
    FORMATS "${FORMATS}"
    PRODUCT_NAME "${PROJECT_NAME}")


target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

set(Source
    Source/PluginEditor.h
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginProcessor.cpp)

target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

set_target_properties("${PROJECT_NAME}" PROPERTIES FOLDER "")

target_compile_definitions("${PROJECT_NAME}"
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
