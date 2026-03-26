#include "PluginProcessor.h"
#include "PluginEditor.h"

HottMasterProcessor::HottMasterProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout HottMasterProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Resonance Suppressor
    params.push_back(std::make_unique<juce::AudioParameterFloat>("soothe_depth", "Soothe Depth", 0.f, 1.f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("soothe_speed", "Soothe Speed", 0.f, 1.f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("soothe_select", "Soothe Selectivity", 0.f, 1.f, 0.5f));

    // Linear Phase EQ
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eq_low", "EQ Low Shelf", -12.f, 12.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eq_lowmid", "EQ Low-Mid", -12.f, 12.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eq_highmid", "EQ High-Mid", -12.f, 12.f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eq_high", "EQ High Shelf", -12.f, 12.f, 0.f));

    // Bus Compressor
    params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_thresh", "Comp Threshold", -40.f, 0.f, -12.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_ratio", "Comp Ratio", 1.f, 10.f, 2.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_attack", "Comp Attack", 0.1f, 100.f, 30.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_release", "Comp Release", 10.f, 1000.f, 300.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_makeup", "Comp Makeup", 0.f, 12.f, 0.f));

    // Soft Clipper
    params.push_back(std::make_unique<juce::AudioParameterFloat>("clip_ceiling", "Clip Ceiling", -6.f, 0.f, -0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("clip_drive", "Clip Drive", 0.5f, 4.f, 1.f));

    // True Peak Limiter
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lim_ceiling", "Limiter Ceiling", -3.f, 0.f, -1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lim_release", "Limiter Release", 10.f, 500.f, 100.f));

    // Output
    params.push_back(std::make_unique<juce::AudioParameterFloat>("output_gain", "Output Gain", -12.f, 12.f, 0.f));

    return {params.begin(), params.end()};
}

void HottMasterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    resonanceSuppressor.prepare(sampleRate, samplesPerBlock);
    linearPhaseEQ.prepare(sampleRate, samplesPerBlock);
    compressor.prepare(sampleRate, samplesPerBlock);
    softClipper.prepare(sampleRate, samplesPerBlock);
    limiter.prepare(sampleRate, samplesPerBlock);
}

void HottMasterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    juce::dsp::AudioBlock<float> block(buffer);

    // Update parameters
    resonanceSuppressor.setDepth(*apvts.getRawParameterValue("soothe_depth"));
    resonanceSuppressor.setSpeed(*apvts.getRawParameterValue("soothe_speed"));
    resonanceSuppressor.setSelectivity(*apvts.getRawParameterValue("soothe_select"));

    linearPhaseEQ.setLowShelfGain(*apvts.getRawParameterValue("eq_low"));
    linearPhaseEQ.setLowMidGain(*apvts.getRawParameterValue("eq_lowmid"));
    linearPhaseEQ.setHighMidGain(*apvts.getRawParameterValue("eq_highmid"));
    linearPhaseEQ.setHighShelfGain(*apvts.getRawParameterValue("eq_high"));

    compressor.setThreshold(*apvts.getRawParameterValue("comp_thresh"));
    compressor.setRatio(*apvts.getRawParameterValue("comp_ratio"));
    compressor.setAttack(*apvts.getRawParameterValue("comp_attack"));
    compressor.setRelease(*apvts.getRawParameterValue("comp_release"));
    compressor.setMakeupGain(*apvts.getRawParameterValue("comp_makeup"));

    softClipper.setCeiling(*apvts.getRawParameterValue("clip_ceiling"));
    softClipper.setDrive(*apvts.getRawParameterValue("clip_drive"));

    limiter.setCeiling(*apvts.getRawParameterValue("lim_ceiling"));
    limiter.setRelease(*apvts.getRawParameterValue("lim_release"));

    // Signal chain
    resonanceSuppressor.process(block);
    linearPhaseEQ.process(block);
    compressor.process(block);
    softClipper.process(block);
    limiter.process(block);

    // Output gain
    float outGain = juce::Decibels::decibelsToGain((float)*apvts.getRawParameterValue("output_gain"));
    block.multiplyBy(outGain);
}

juce::AudioProcessorEditor* HottMasterProcessor::createEditor()
{
    return new HottMasterEditor(*this);
}

void HottMasterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HottMasterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HottMasterProcessor();
}
