#include "PluginProcessor.h"
#include "PluginEditor.h"

static const float kRatioValues[]  = { 0.0f, 4.0f, 8.0f, 12.0f, 20.0f };
static const float kOutputGains[]  = { 0.0f, 0.25f, 0.50f, 0.75f, 1.0f };

juce::AudioProcessorValueTreeState::ParameterLayout DumbCompressorProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "ratioIndex", "Ratio",
        juce::NormalisableRange<float>(0.0f, 4.0f, 1.0f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "outputLevel", "Output",
        juce::NormalisableRange<float>(0.0f, 4.0f, 1.0f), 4.0f));   // default 100%

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
    envelopeL = envelopeR = 0.0f;
    // Coefficient formula: e^(-1 / (sampleRate * time_s))
    attackCoeff  = std::exp(-1.0f / (float(sampleRate) * kAttackMs  * 0.001f));
    releaseCoeff = std::exp(-1.0f / (float(sampleRate) * kReleaseMs * 0.001f));
}

void DumbCompressorProcessor::releaseResources() {}

void DumbCompressorProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer&)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    const int ratioIdx = juce::jlimit(0, 4,
        (int)std::round(apvts.getRawParameterValue("ratioIndex")->load()));
    const int outIdx = juce::jlimit(0, 4,
        (int)std::round(apvts.getRawParameterValue("outputLevel")->load()));

    const float ratio      = kRatioValues[ratioIdx];
    const float outputGain = kOutputGains[outIdx];
    const float threshold  = juce::Decibels::decibelsToGain(kThresholdDb);

    if (ratio <= 0.0f)
    {
        // Ratio = Off: bypass compression, still apply output level
        buffer.applyGain(outputGain);
        return;
    }

    float* chL = numChannels > 0 ? buffer.getWritePointer(0) : nullptr;
    float* chR = numChannels > 1 ? buffer.getWritePointer(1) : chL;

    for (int s = 0; s < numSamples; ++s)
    {
        const float inL = chL[s];
        const float inR = chR[s];

        // --- Feed-forward peak detector (1176 topology) ---
        // Track each channel independently, use the louder one for GR
        const float peakL = std::abs(inL);
        const float peakR = std::abs(inR);

        envelopeL = (peakL > envelopeL)
            ? attackCoeff  * envelopeL + (1.0f - attackCoeff)  * peakL
            : releaseCoeff * envelopeL + (1.0f - releaseCoeff) * peakL;

        envelopeR = (peakR > envelopeR)
            ? attackCoeff  * envelopeR + (1.0f - attackCoeff)  * peakR
            : releaseCoeff * envelopeR + (1.0f - releaseCoeff) * peakR;

        const float detectedLevel = std::max(envelopeL, envelopeR);

        // --- Gain reduction (hard knee, dB domain) ---
        float gain = 1.0f;
        if (detectedLevel > threshold)
        {
            // overDb > 0: how many dB above threshold
            const float overDb = juce::Decibels::gainToDecibels(detectedLevel) - kThresholdDb;
            // Compress: output = threshold + overDb/ratio
            // Gain change = (overDb/ratio) - overDb = overDb*(1/ratio - 1)  (negative)
            gain = juce::Decibels::decibelsToGain(overDb * (1.0f / ratio - 1.0f));
        }

        chL[s] = inL * gain * outputGain;
        chR[s] = inR * gain * outputGain;
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
