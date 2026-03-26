#pragma once
#include <juce_dsp/juce_dsp.h>

// Dynamic resonance suppressor inspired by Soothe2.
// Splits signal into bands, detects energy per-band, and attenuates
// only where resonances exceed a threshold — preserving life in the mix.
class ResonanceSuppressor
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(juce::dsp::AudioBlock<float>& block);

    void setDepth(float d)       { depth = d; }
    void setSpeed(float s)       { speed = s; }
    void setSelectivity(float s) { selectivity = s; }

private:
    static constexpr int numBands = 6;

    // Band-split filters (Linkwitz-Riley 2nd order)
    using Filter = juce::dsp::IIR::Filter<float>;
    std::array<juce::dsp::ProcessorDuplicator<Filter, juce::dsp::IIR::Coefficients<float>>, numBands> bandpass;

    std::array<float, numBands> envelope{};
    float depth = 0.5f;
    float speed = 0.5f;       // envelope follower speed
    float selectivity = 0.5f; // how narrow the suppression
    double sr = 44100.0;

    // Crossover frequencies
    static constexpr std::array<float, numBands> crossovers = {200.f, 500.f, 1500.f, 4000.f, 8000.f, 16000.f};
};
