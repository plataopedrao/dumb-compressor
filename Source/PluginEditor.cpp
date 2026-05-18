#include "PluginProcessor.h"
#include "PluginEditor.h"

void DumbCompressorEditor::setupKnob(juce::Slider& knob, juce::Label& label,
                                     const juce::String& labelText,
                                     std::function<juce::String(double)> textFn)
{
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setRange(0.0, 4.0, 1.0);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 24);
    knob.textFromValueFunction = textFn;
    knob.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xff4488dd));
    knob.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff1a2a40));
    knob.setColour(juce::Slider::thumbColourId,               juce::Colour(0xff88ccff));
    knob.setColour(juce::Slider::textBoxTextColourId,         juce::Colours::white);
    knob.setColour(juce::Slider::textBoxBackgroundColourId,   juce::Colour(0xff0a0a18));
    knob.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colour(0xff253550));
    addAndMakeVisible(knob);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(11.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xff7788aa));
    addAndMakeVisible(label);
}

DumbCompressorEditor::DumbCompressorEditor(DumbCompressorProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(440, 300);

    setupKnob(outputKnob, outputLabel, "OUTPUT",
        [](double val) -> juce::String {
            static const char* labels[] = { "0%", "25%", "50%", "75%", "100%" };
            return labels[juce::jlimit(0, 4, (int)std::round(val))];
        });

    setupKnob(ratioKnob, ratioLabel, "RATIO",
        [](double val) -> juce::String {
            static const char* labels[] = { "Off", "4:1", "8:1", "12:1", "20:1" };
            return labels[juce::jlimit(0, 4, (int)std::round(val))];
        });

    // Attachments after knobs are fully configured
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.apvts, "outputLevel", outputKnob);
    ratioAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.apvts, "ratioIndex",  ratioKnob);
}

DumbCompressorEditor::~DumbCompressorEditor() {}

void DumbCompressorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff10101c));

    // Header
    g.setColour(juce::Colour(0xff181828));
    g.fillRect(0, 0, getWidth(), 46);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(17.0f, juce::Font::bold));
    g.drawFittedText("DUMB COMPRESSOR", 0, 4, getWidth(), 24,
                     juce::Justification::centred, 1);
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colour(0xff3a3a70));
    g.drawFittedText("Peposoft GmbH", 0, 28, getWidth(), 16,
                     juce::Justification::centred, 1);

    // Divider between knobs
    g.setColour(juce::Colour(0xff222235));
    g.fillRect(getWidth() / 2 - 1, 50, 2, getHeight() - 94);

    // Footer
    g.setColour(juce::Colour(0xff181828));
    g.fillRect(0, getHeight() - 40, getWidth(), 40);
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colour(0xff445577));
    g.drawFittedText("Attack  1ms   |   Release  100ms   |   Threshold  -18dB",
                     0, getHeight() - 28, getWidth(), 20,
                     juce::Justification::centred, 1);
}

void DumbCompressorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(46);
    area.removeFromBottom(40);

    auto leftHalf  = area.removeFromLeft(getWidth() / 2);
    auto rightHalf = area;

    outputLabel.setBounds(leftHalf.removeFromTop(22).reduced(30, 0));
    outputKnob.setBounds(leftHalf.reduced(28, 8));

    ratioLabel.setBounds(rightHalf.removeFromTop(22).reduced(30, 0));
    ratioKnob.setBounds(rightHalf.reduced(28, 8));
}
