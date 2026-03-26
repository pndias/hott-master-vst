cmake_minimum_required(VERSION 3.22)
project(HottMaster VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(HottMaster
    COMPANY_NAME "HottAudio"
    PLUGIN_MANUFACTURER_CODE Hott
    PLUGIN_CODE Htms
    FORMATS VST3
    PRODUCT_NAME "Hott Master"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
)

target_sources(HottMaster PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/ResonanceSuppressor.cpp
    Source/DSP/LinearPhaseEQ.cpp
    Source/DSP/BusCompressor.cpp
    Source/DSP/SoftClipper.cpp
    Source/DSP/TruePeakLimiter.cpp
    Source/UI/HottLookAndFeel.h
    Source/UI/GRMeter.h
)

target_include_directories(HottMaster PRIVATE Source)

target_compile_definitions(HottMaster PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(HottMaster
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
