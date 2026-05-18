#include "PluginProcessor.h"
#include "PluginEditor.h"

static const float kRatioValues[] = { 0.0f, 4.0f, 8.0f, 12.0f, 20.0f };

juce::AudioProcessorValueTreeState::ParameterLayout DumbCompressorProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ratioIndex", "Ratio",
        juce::NormalisableRange<float>(0.0f, 4.0f, 1.0f),
        0.0f));
    return layout;
}

DumbCompressorProcessor::DumbCompressorProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{}

DumbCompressorProcessor::~DumbCompressorProcessor() {}

const juce::String DumbCompressorProcessor::getName() const { return JucePlugin_Name; }
bool DumbCompressorProcessor::acceptsMidi()  const { return false; }
bool DumbCompressorProcessor::producesMidi() const { return false; }
bool DumbCompressorProcessor::isMidiEffect() const { return false; }
double DumbCompressorProcessor::getTailLengthSeconds() const { return 0.0; }

int  DumbCompressorProcessor::getNumPrograms()                              { return 1; }
int  DumbCompressorProcessor::getCurrentProgram()                           { return 0; }
void DumbCompressorProcessor::setCurrentProgram(int)                        {}
const juce::String DumbCompressorProcessor::getProgramName(int)             { return {}; }
void DumbCompressorProcessor::changeProgramName(int, const juce::String&)   {}

void DumbCompressorProcessor::prepareToPlay(double sampleRate, int)
{
    envelope     = 0.0f;
    attackCoeff  = std::exp(-1.0f / (float(sampleRate) * kAttackMs  * 0.001f));
    releaseCoeff = std::exp(-1.0f / (float(sampleRate) * kReleaseMs * 0.001f));
}

void DumbCompressorProcessor::releaseResources() {}

void DumbCompressorProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer&)
{
    int idx   = juce::jlimit(0, 4, (int)std::round(apvts.getRawParameterValue("ratioIndex")->load()));
    float ratio = kRatioValues[idx];

    if (ratio <= 0.0f)
        return; // Off — pass through

    const float threshold = juce::Decibels::decibelsToGain(kThresholdDb);

    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        // Peak level across channels
        float level = 0.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            level = std::max(level, std::abs(buffer.getSample(ch, s)));

        // Branching envelope follower (attack faster than release)
        float coeff = (level > envelope) ? attackCoeff : releaseCoeff;
        envelope    = coeff * envelope + (1.0f - coeff) * level;

        // Gain reduction above threshold
        float gain = 1.0f;
        if (envelope > threshold)
        {
            float overDb = juce::Decibels::gainToDecibels(envelope) - kThresholdDb;
            gain = juce::Decibels::decibelsToGain(overDb * (1.0f / ratio - 1.0f));
        }

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, s, buffer.getSample(ch, s) * gain);
    }
}

bool DumbCompressorProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* DumbCompressorProcessor::createEditor()
{
    return new DumbCompressorEditor(*this);
}

void DumbCompressorProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DumbCompressorProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DumbCompressorProcessor();
}
