#pragma once
#include <juce_dsp/juce_dsp.h>

// SSL-style bus compressor. Transparent glue compression.
// Feed-forward VCA design with RMS detection.
class BusCompressor
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(juce::dsp::AudioBlock<float>& block);

    void setThreshold(float dB)  { thresholdDB = dB; }
    void setRatio(float r)       { ratio = r; }
    void setAttack(float ms)     { attackMs = ms; }
    void setRelease(float ms)    { releaseMs = ms; }
    void setMakeupGain(float dB) { makeupDB = dB; }

    float getGainReduction() const { return currentGR; }

private:
    float thresholdDB = -12.f;
    float ratio = 2.f;
    float attackMs = 30.f;
    float releaseMs = 300.f;
    float makeupDB = 0.f;

    float envDB = 0.f;
    float currentGR = 0.f;
    double sr = 44100.0;
};
