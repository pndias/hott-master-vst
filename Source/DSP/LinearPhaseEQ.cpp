#include "LinearPhaseEQ.h"

void LinearPhaseEQ::prepare(double sampleRate, int blockSize)
{
    sr = sampleRate;
    juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)blockSize, 2};
    convolution.prepare(spec);
    dirty = true;
    rebuildKernel();
}

void LinearPhaseEQ::rebuildKernel()
{
    if (!dirty) return;
    dirty = false;

    // Build minimum-phase IIR response, then make it linear-phase via
    // symmetric FIR kernel (time-reverse trick).
    std::array<float, fftSize> kernel{};
    kernel[fftSize / 2] = 1.0f; // impulse at center (linear phase)

    // Apply EQ curves in frequency domain
    juce::dsp::FFT fft(fftOrder);
    std::vector<float> fftData(fftSize * 2, 0.f);

    // Copy kernel into FFT buffer (real part only)
    for (int i = 0; i < fftSize; ++i)
        fftData[i * 2] = kernel[i];

    fft.performRealOnlyForwardTransform(fftData.data());

    // Shape magnitude response
    for (int i = 0; i <= fftSize / 2; ++i)
    {
        float freq = (float)i * (float)sr / (float)fftSize;
        float gainDB = 0.f;

        // Low shelf < 200 Hz
        if (freq < 200.f)
            gainDB += lowGain;
        // Low-mid 200-1000 Hz (bell)
        else if (freq < 1000.f)
            gainDB += lmGain * std::exp(-0.5f * std::pow((freq - 500.f) / 200.f, 2.f));
        // High-mid 1-6 kHz (bell)
        if (freq > 1000.f && freq < 6000.f)
            gainDB += hmGain * std::exp(-0.5f * std::pow((freq - 3000.f) / 1000.f, 2.f));
        // High shelf > 6 kHz
        if (freq > 6000.f)
            gainDB += highGain;

        float gain = juce::Decibels::decibelsToGain(gainDB);
        fftData[i * 2]     *= gain;
        fftData[i * 2 + 1] *= gain;
    }

    fft.performRealOnlyInverseTransform(fftData.data());

    // Extract real part as kernel
    juce::AudioBuffer<float> irBuffer(1, fftSize);
    for (int i = 0; i < fftSize; ++i)
        irBuffer.setSample(0, i, fftData[i * 2]);

    convolution.loadImpulseResponse(std::move(irBuffer), sr, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no);
}

void LinearPhaseEQ::process(juce::dsp::AudioBlock<float>& block)
{
    rebuildKernel();
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    convolution.process(ctx);
}
