#pragma once

#include <JuceHeader.h>

class DumbCompressorProcessor : public juce::AudioProcessor
{
public:
    DumbCompressorProcessor();
    ~DumbCompressorProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Per-channel envelope state
    float envelopeL = 0.0f;
    float envelopeR = 0.0f;
    float attackCoeff  = 0.0f;
    float releaseCoeff = 0.0f;

    // 1176-style: fast attack, moderate release, fixed threshold
    static constexpr float kThresholdDb = -18.0f;
    static constexpr float kAttackMs    =   1.0f;   // fast FET-style grab
    static constexpr float kReleaseMs   = 100.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DumbCompressorProcessor)
};
