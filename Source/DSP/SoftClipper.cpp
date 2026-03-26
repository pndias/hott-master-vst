#include "SoftClipper.h"

void SoftClipper::prepare(double sampleRate, int blockSize)
{
    oversampling.initProcessing((size_t)blockSize);
}

void SoftClipper::process(juce::dsp::AudioBlock<float>& block)
{
    auto osBlock = oversampling.processSamplesUp(block);
    float ceiling = juce::Decibels::decibelsToGain(ceilingDB);

    for (int ch = 0; ch < (int)osBlock.getNumChannels(); ++ch)
    {
        auto* data = osBlock.getChannelPointer(ch);
        for (int s = 0; s < (int)osBlock.getNumSamples(); ++s)
        {
            float x = data[s] * drive;
            // tanh soft clip scaled to ceiling
            data[s] = std::tanh(x / ceiling) * ceiling;
        }
    }

    oversampling.processSamplesDown(block);
}
