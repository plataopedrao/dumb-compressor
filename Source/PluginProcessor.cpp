#include "PluginProcessor.h"
#include "PluginEditor.h"

DumbCompressorProcessor::DumbCompressorProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
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

void DumbCompressorProcessor::prepareToPlay(double, int) {}
void DumbCompressorProcessor::releaseResources() {}

void DumbCompressorProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer&)
{
    // Hello World: pass audio through unchanged
    juce::ignoreUnused(buffer);
}

bool DumbCompressorProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* DumbCompressorProcessor::createEditor()
{
    return new DumbCompressorEditor(*this);
}

void DumbCompressorProcessor::getStateInformation(juce::MemoryBlock&)       {}
void DumbCompressorProcessor::setStateInformation(const void*, int)         {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DumbCompressorProcessor();
}
