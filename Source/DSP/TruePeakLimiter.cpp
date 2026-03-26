#include "TruePeakLimiter.h"

void TruePeakLimiter::prepare(double sampleRate, int blockSize)
{
    sr = sampleRate;
    delaySamples = (int)(sr * lookaheadMs * 0.001f);

    delayBuffer.setSize(2, delaySamples + blockSize);
    delayBuffer.clear();
    delayWritePos = 0;
    envGain = 1.f;

    oversampling.initProcessing((size_t)blockSize);
}

void TruePeakLimiter::process(juce::dsp::AudioBlock<float>& block)
{
    auto numSamples = (int)block.getNumSamples();
    auto numChannels = (int)block.getNumChannels();
    float ceiling = juce::Decibels::decibelsToGain(ceilingDB);
    float releaseCoeff = std::exp(-1.0f / (float)(sr * releaseMs * 0.001f));

    // True peak detection via oversampling
    juce::AudioBuffer<float> peakBuf(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
        std::copy(block.getChannelPointer(ch), block.getChannelPointer(ch) + numSamples, peakBuf.getWritePointer(ch));

    juce::dsp::AudioBlock<float> peakBlock(peakBuf);
    auto osBlock = oversampling.processSamplesUp(peakBlock);

    // Find true peak per sample (downsampled back)
    std::vector<float> truePeaks(numSamples, 0.f);
    int osRatio = (int)(osBlock.getNumSamples() / numSamples);
    for (int s = 0; s < numSamples; ++s)
    {
        float maxPeak = 0.f;
        for (int os = 0; os < osRatio; ++os)
        {
            int idx = s * osRatio + os;
            if (idx < (int)osBlock.getNumSamples())
                for (int ch = 0; ch < numChannels; ++ch)
                    maxPeak = std::max(maxPeak, std::abs(osBlock.getChannelPointer(ch)[idx]));
        }
        truePeaks[s] = maxPeak;
    }

    // Write input to delay buffer and read delayed output
    for (int s = 0; s < numSamples; ++s)
    {
        // Write to delay
        for (int ch = 0; ch < numChannels; ++ch)
            delayBuffer.setSample(ch, delayWritePos, block.getChannelPointer(ch)[s]);

        // Compute gain from lookahead peak
        float targetGain = truePeaks[s] > ceiling ? ceiling / (truePeaks[s] + 1e-10f) : 1.f;

        // Smooth: instant attack, slow release
        if (targetGain < envGain)
            envGain = targetGain;
        else
            envGain = envGain * releaseCoeff + targetGain * (1.f - releaseCoeff);

        currentGR = juce::Decibels::gainToDecibels(envGain);

        // Read from delay
        int readPos = (delayWritePos - delaySamples + delayBuffer.getNumSamples()) % delayBuffer.getNumSamples();
        for (int ch = 0; ch < numChannels; ++ch)
            block.getChannelPointer(ch)[s] = delayBuffer.getSample(ch, readPos) * envGain;

        delayWritePos = (delayWritePos + 1) % delayBuffer.getNumSamples();
    }
}
