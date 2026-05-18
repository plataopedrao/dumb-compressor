#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class DumbCompressorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DumbCompressorEditor(DumbCompressorProcessor&);
    ~DumbCompressorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DumbCompressorProcessor& audioProcessor;

    juce::Slider ratioKnob;
    juce::Label  ratioLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;

    juce::Slider outputKnob;
    juce::Label  outputLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;

    void setupKnob(juce::Slider& knob,
                   juce::Label& label,
                   const juce::String& labelText,
                   std::function<juce::String(double)> textFn);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DumbCompressorEditor)
};
