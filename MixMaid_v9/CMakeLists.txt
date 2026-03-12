cmake_minimum_required(VERSION 3.22)
project(MixMaid_AAAAAA_v9 VERSION 0.9.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    message(FATAL_ERROR "JUCE submodule/folder missing. Add JUCE at MixMaid_AAAAAA_v9/JUCE before configuring.")
endif()

add_subdirectory(JUCE)

juce_add_plugin(MixMaid
    COMPANY_NAME "Gary Doman Audio"
    BUNDLE_ID com.garydoman.mixmaid
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE GDom
    PLUGIN_CODE MmA9
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "MixMaid")

target_sources(MixMaid PRIVATE
    Source/PluginEntry.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Shared/Analysis/AnalyzerState.cpp
    Shared/Analysis/ReferenceMatcher.cpp
    Shared/DSP/SmartBands.cpp
    Shared/DSP/SafetyLimiter.cpp
    Shared/Core/PresetBank.cpp
    Shared/Core/ABState.cpp)

target_include_directories(MixMaid PRIVATE
    Source
    Shared/Analysis
    Shared/DSP
    Shared/Core)

target_compile_definitions(MixMaid PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_link_libraries(MixMaid PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
