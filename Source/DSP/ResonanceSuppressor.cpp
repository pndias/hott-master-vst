#include "ResonanceSuppressor.h"

void ResonanceSuppressor::prepare(double sampleRate, int blockSize)
{
    sr = sampleRate;
    juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)blockSize, 2};

    for (int i = 0; i < numBands; ++i)
    {
        *bandpass[i].state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, crossovers[i], 1.0f + selectivity * 4.0f);
        bandpass[i].prepare(spec);
        envelope[i] = 0.f;
    }
}

void ResonanceSuppressor::process(juce::dsp::AudioBlock<float>& block)
{
    auto numSamples = (int)block.getNumSamples();
    auto numChannels = (int)block.getNumChannels();

    // Attack/release for envelope follower
    float attackCoeff  = std::exp(-1.0f / (float)(sr * (0.001f + (1.f - speed) * 0.05f)));
    float releaseCoeff = std::exp(-1.0f / (float)(sr * (0.01f + (1.f - speed) * 0.2f)));

    // Temp buffer for band analysis
    juce::AudioBuffer<float> tempBuf((int)block.getNumChannels(), numSamples);

    for (int b = 0; b < numBands; ++b)
    {
        // Copy input to temp
        for (int ch = 0; ch < numChannels; ++ch)
            std::copy(block.getChannelPointer(ch), block.getChannelPointer(ch) + numSamples, tempBuf.getWritePointer(ch));

        juce::dsp::AudioBlock<float> tempBlock(tempBuf);
        bandpass[b].process(juce::dsp::ProcessContextReplacing<float>(tempBlock));

        // Measure band energy
        float bandEnergy = 0.f;
        for (int ch = 0; ch < numChannels; ++ch)
            for (int s = 0; s < numSamples; ++s)
                bandEnergy += tempBuf.getSample(ch, s) * tempBuf.getSample(ch, s);

        bandEnergy = std::sqrt(bandEnergy / (float)(numSamples * numChannels));

        // Envelope follower
        float coeff = bandEnergy > envelope[b] ? attackCoeff : releaseCoeff;
        envelope[b] = envelope[b] * coeff + bandEnergy * (1.f - coeff);

        // Dynamic attenuation: suppress only resonant bands
        float threshold = 0.1f + (1.f - depth) * 0.5f;
        if (envelope[b] > threshold)
        {
            float reduction = 1.f - depth * (envelope[b] - threshold) / (envelope[b] + 0.001f);
            reduction = juce::jlimit(0.2f, 1.0f, reduction);

            // Subtract attenuated band content from original
            for (int ch = 0; ch < numChannels; ++ch)
                for (int s = 0; s < numSamples; ++s)
                    block.getChannelPointer(ch)[s] -= tempBuf.getSample(ch, s) * (1.f - reduction);
        }
    }
}
