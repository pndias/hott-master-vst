#pragma once
#include <juce_dsp/juce_dsp.h>

// Soft clipper — shaves transient peaks before the limiter,
// adding subtle harmonic saturation and preventing pumping.
class SoftClipper
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(juce::dsp::AudioBlock<float>& block);

    void setCeiling(float dB) { ceilingDB = dB; }
    void setDrive(float d)    { drive = d; }

private:
    float ceilingDB = -0.3f;
    float drive = 1.0f;
    juce::dsp::Oversampling<float> oversampling{2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR};
};
