#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/HottLookAndFeel.h"
#include "UI/GRMeter.h"

class HottMasterEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit HottMasterEditor(HottMasterProcessor&);
    ~HottMasterEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    // Helpers
    void drawBackground(juce::Graphics&);
    void drawSectionPanel(juce::Graphics&, juce::Rectangle<float> bounds, const juce::String& title, juce::Colour accent);
    void drawSignalChainLine(juce::Graphics&);
    void drawHeader(juce::Graphics&);

    HottMasterProcessor& proc;
    HottLookAndFeel lnf;

    struct ParamKnob
    {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    struct Section
    {
        juce::String name;
        juce::Colour accent;
        std::vector<std::pair<juce::String, juce::String>> params;
        std::vector<ParamKnob> knobs;
        juce::Rectangle<float> bounds;
    };

    std::vector<Section> sections;

    GRMeter compMeter{juce::Colour(0xFF34D399)};
    GRMeter limMeter{juce::Colour(0xFFF87171)};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HottMasterEditor)
};
