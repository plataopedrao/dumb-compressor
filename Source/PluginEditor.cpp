#include "PluginProcessor.h"
#include "PluginEditor.h"

static const char* kRatioLabels[] = { "Off", "4:1", "8:1", "12:1", "20:1" };

DumbCompressorEditor::DumbCompressorEditor(DumbCompressorProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(320, 320);

    // Ratio knob
    ratioKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratioKnob.setRange(0.0, 4.0, 1.0);
    ratioKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 24);
    ratioKnob.textFromValueFunction = [](double val) -> juce::String {
        return kRatioLabels[juce::jlimit(0, 4, (int)std::round(val))];
    };
    ratioKnob.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xff4488dd));
    ratioKnob.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff1e2d4a));
    ratioKnob.setColour(juce::Slider::thumbColourId,               juce::Colour(0xff88ccff));
    ratioKnob.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    ratioKnob.setColour(juce::Slider::textBoxBackgroundColourId,   juce::Colour(0xff0d0d1a));
    ratioKnob.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colour(0xff2a3a5a));
    addAndMakeVisible(ratioKnob);

    ratioLabel.setText("RATIO", juce::dontSendNotification);
    ratioLabel.setJustificationType(juce::Justification::centred);
    ratioLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    ratioLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8899bb));
    addAndMakeVisible(ratioLabel);

    // Attach after knob is configured
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.apvts, "ratioIndex", ratioKnob);
}

DumbCompressorEditor::~DumbCompressorEditor() {}

void DumbCompressorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff12121e));

    // Header band
    g.setColour(juce::Colour(0xff1a1a32));
    g.fillRect(0, 0, getWidth(), 48);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawFittedText("DUMB COMPRESSOR", 0, 0, getWidth(), 34, juce::Justification::centred, 1);

    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colour(0xff4455aa));
    g.drawFittedText("Peposoft GmbH", 0, 30, getWidth(), 18, juce::Justification::centred, 1);

    // Fixed params strip at bottom
    g.setColour(juce::Colour(0xff1a1a32));
    g.fillRect(0, getHeight() - 44, getWidth(), 44);

    g.setFont(juce::Font(11.0f));
    g.setColour(juce::Colour(0xff6677aa));
    g.drawFittedText("Attack  10 ms      Release  100 ms      Threshold  -18 dB",
                     0, getHeight() - 32, getWidth(), 24, juce::Justification::centred, 1);
}

void DumbCompressorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(48);
    area.removeFromBottom(44);

    ratioLabel.setBounds(area.removeFromTop(22).reduced(80, 0));
    ratioKnob.setBounds(area.reduced(60, 10));
}
