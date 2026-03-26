#include "BusCompressor.h"

void BusCompressor::prepare(double sampleRate, int /*blockSize*/)
{
    sr = sampleRate;
    envDB = 0.f;
}

void BusCompressor::process(juce::dsp::AudioBlock<float>& block)
{
    auto numSamples = (int)block.getNumSamples();
    auto numChannels = (int)block.getNumChannels();

    float attackCoeff  = std::exp(-1.0f / (float)(sr * attackMs * 0.001f));
    float releaseCoeff = std::exp(-1.0f / (float)(sr * releaseMs * 0.001f));
    float makeup = juce::Decibels::decibelsToGain(makeupDB);

    for (int s = 0; s < numSamples; ++s)
    {
        // RMS-style sidechain (sum of squares across channels)
        float sumSq = 0.f;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float v = block.getChannelPointer(ch)[s];
            sumSq += v * v;
        }
        float inputDB = juce::Decibels::gainToDecibels(std::sqrt(sumSq / (float)numChannels), -100.f);

        // Gain computer (soft knee)
        float overDB = inputDB - thresholdDB;
        float gcDB = 0.f;
        if (overDB > 0.f)
            gcDB = overDB * (1.f - 1.f / ratio);

        // Smooth envelope
        float coeff = gcDB > envDB ? attackCoeff : releaseCoeff;
        envDB = envDB * coeff + gcDB * (1.f - coeff);

        float gainLin = juce::Decibels::decibelsToGain(-envDB) * makeup;
        currentGR = envDB;

        for (int ch = 0; ch < numChannels; ++ch)
            block.getChannelPointer(ch)[s] *= gainLin;
    }
}
