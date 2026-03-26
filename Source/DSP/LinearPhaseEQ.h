#pragma once
#include <juce_dsp/juce_dsp.h>

// Linear-phase EQ using FFT convolution (zero phase distortion).
// 4-band parametric: Low Shelf, Low-Mid, High-Mid, High Shelf.
class LinearPhaseEQ
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(juce::dsp::AudioBlock<float>& block);

    void setLowShelfGain(float dB)   { lowGain = dB; dirty = true; }
    void setLowMidGain(float dB)     { lmGain = dB; dirty = true; }
    void setHighMidGain(float dB)    { hmGain = dB; dirty = true; }
    void setHighShelfGain(float dB)  { highGain = dB; dirty = true; }

private:
    void rebuildKernel();

    static constexpr int fftOrder = 10; // 1024 taps
    static constexpr int fftSize = 1 << fftOrder;

    juce::dsp::Convolution convolution;
    double sr = 44100.0;

    float lowGain = 0.f, lmGain = 0.f, hmGain = 0.f, highGain = 0.f;
    bool dirty = true;
};
