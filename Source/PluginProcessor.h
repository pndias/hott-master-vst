#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/ResonanceSuppressor.h"
#include "DSP/LinearPhaseEQ.h"
#include "DSP/BusCompressor.h"
#include "DSP/SoftClipper.h"
#include "DSP/TruePeakLimiter.h"

class HottMasterProcessor : public juce::AudioProcessor
{
public:
    HottMasterProcessor();
    ~HottMasterProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Hott Master"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // Metering
    float getCompressorGR() const { return compressor.getGainReduction(); }
    float getLimiterGR() const    { return limiter.getGainReduction(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    ResonanceSuppressor resonanceSuppressor;
    LinearPhaseEQ linearPhaseEQ;
    BusCompressor compressor;
    SoftClipper softClipper;
    TruePeakLimiter limiter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HottMasterProcessor)
};
