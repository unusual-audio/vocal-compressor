/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalCompressorAudioProcessorEditor::VocalCompressorAudioProcessorEditor (VocalCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
        thresholdAttachment(*p.threshold, thresholdSlider),
        ratioAttachment(*p.ratio, ratioSlider),
        attackAttachment(*p.attack, attackSlider),
        releaseAttachment(*p.release, releaseSlider),
        kneeAttachment(*p.knee, kneeSlider),
        autoGainAttachment(*p.autoGain, autoGainSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (440, 400);
    
    addAndMakeVisible (thresholdSlider);
    thresholdSlider.setTextValueSuffix (" dBFS");
    thresholdSlider.setRange(thresholdSlider.getRange(), 1.0f);
    thresholdSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value);
    };
    thresholdSlider.updateText();
    thresholdSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    thresholdSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (ratioSlider);
    ratioSlider.setTextValueSuffix (":1");
    ratioSlider.setRange(ratioSlider.getRange(), 0.1f);
    ratioSlider.setSkewFactor(0.5f);
    ratioSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.1f", value);
    };
    ratioSlider.updateText();
    ratioSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    ratioSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    ratioSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    ratioSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (attackSlider);
    attackSlider.setTextValueSuffix (" ms");
    attackSlider.setRange(attackSlider.getRange(), 0.1f);
    attackSlider.setSkewFactor(0.5f);
    attackSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.1f", value);
    };
    attackSlider.updateText();
    attackSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    attackSlider.setColour(juce::Slider::ColourIds::thumbColourId, blue);
    attackSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, blue);
    attackSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (releaseSlider);
    releaseSlider.setTextValueSuffix (" ms");
    releaseSlider.setRange(releaseSlider.getRange(), 1.0f);
    attackSlider.setSkewFactor(0.5f);
    releaseSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value);
    };
    releaseSlider.updateText();
    releaseSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    releaseSlider.setColour(juce::Slider::ColourIds::thumbColourId, blue);
    releaseSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, blue);
    releaseSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (kneeSlider);
    kneeSlider.setTextValueSuffix (" dB");
    kneeSlider.setRange(kneeSlider.getRange(), 1.f);
    kneeSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value);
    };
    kneeSlider.updateText();
    kneeSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    kneeSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    kneeSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    kneeSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    kneeSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    kneeSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    kneeSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (autoGainSlider);
    autoGainSlider.setTextValueSuffix ("%");
    autoGainSlider.setRange(autoGainSlider.getRange(), 0.01f);
    autoGainSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value * 100.f);
    };
    autoGainSlider.valueFromTextFunction = [](juce::String text)
    {
        return text.getDoubleValue() / 100.f;
    };
    autoGainSlider.updateText();
    autoGainSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    autoGainSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    autoGainSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    autoGainSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    autoGainSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    autoGainSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    autoGainSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
}

VocalCompressorAudioProcessorEditor::~VocalCompressorAudioProcessorEditor()
{
}

//==============================================================================
void VocalCompressorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (black);
    g.setColour(white);
    g.setFont(18);
    g.drawFittedText("Vocal Compressor", 40,  30, 320, 30, juce::Justification::left, 1);
    
    g.setColour(red);
    g.setFont(15);
    g.drawFittedText("Threshold", 40, 90, 100, 30, juce::Justification::left, 1);
    
    g.setColour(red);
    g.setFont(15);
    g.drawFittedText("Ratio", 40,  130, 100, 30, juce::Justification::left, 1);
    
    g.setColour(blue);
    g.setFont(15);
    g.drawFittedText("Attack", 40, 170, 100, 30, juce::Justification::left, 1);
    
    g.setColour(blue);
    g.setFont(15);
    g.drawFittedText("Release", 40, 210, 100, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("Knee", 40, 250, 100, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("Auto Gain", 40, 290, 100, 30, juce::Justification::left, 1);
    
    g.setColour(grey);
    g.setFont(12);
    g.drawFittedText("Unusual Audio", 40, getHeight() - 60, 300, 30, juce::Justification::left, 1);
}

void VocalCompressorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    thresholdSlider .setBounds (140,  95, getWidth() - 140 - 40, 20);
    ratioSlider     .setBounds (140, 135, getWidth() - 140 - 40, 20);
    attackSlider    .setBounds (140, 175, getWidth() - 140 - 40, 20);
    releaseSlider   .setBounds (140, 215, getWidth() - 140 - 40, 20);
    kneeSlider      .setBounds (140, 255, getWidth() - 140 - 40, 20);
    autoGainSlider  .setBounds (140, 295, getWidth() - 140 - 40, 20);
}
