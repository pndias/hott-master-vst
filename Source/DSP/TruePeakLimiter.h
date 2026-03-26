#pragma once
#include <juce_dsp/juce_dsp.h>

// Transparent true-peak limiter (inspired by Pro-L 2 "Modern" algorithm).
// Uses lookahead + oversampled peak detection for inter-sample peak limiting.
class TruePeakLimiter
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(juce::dsp::AudioBlock<float>& block);

    void setCeiling(float dB)  { ceilingDB = dB; }
    void setRelease(float ms)  { releaseMs = ms; }

    float getGainReduction() const { return currentGR; }

private:
    float ceilingDB = -1.0f;
    float releaseMs = 100.f;
    float currentGR = 0.f;
    double sr = 44100.0;

    // Lookahead delay (5ms)
    static constexpr float lookaheadMs = 5.f;
    juce::AudioBuffer<float> delayBuffer;
    int delayWritePos = 0;
    int delaySamples = 0;

    // Oversampling for true peak detection
    juce::dsp::Oversampling<float> oversampling{2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR};

    float envGain = 1.f;
};
