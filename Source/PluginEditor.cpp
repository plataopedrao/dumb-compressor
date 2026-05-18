#include "PluginProcessor.h"
#include "PluginEditor.h"

DumbCompressorEditor::DumbCompressorEditor(DumbCompressorProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);
}

DumbCompressorEditor::~DumbCompressorEditor() {}

void DumbCompressorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(juce::Font(14.0f));
    g.setColour(juce::Colour(0xffaaaaaa));
    g.drawFittedText("Dumb Compressor v0.0.1",
                     getLocalBounds().withTrimmedTop(getHeight() / 2 + 20),
                     juce::Justification::centred, 1);
}

void DumbCompressorEditor::resized() {}
