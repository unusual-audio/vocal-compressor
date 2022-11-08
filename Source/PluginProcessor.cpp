/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalCompressorAudioProcessor::VocalCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (threshold = new juce::AudioParameterFloat (
        "threshold", "Threshold", -60.0f, 0.0f, -18.0f));
    addParameter (ratio = new juce::AudioParameterFloat (
        "ratio", "Ratio", 1.0f, 20.0f, 4.0f));
    addParameter (attack = new juce::AudioParameterFloat (
        "attack", "Attack", 0.0f, 50.0f, 5.0f));
    addParameter (release = new juce::AudioParameterFloat (
        "release", "Release", 0.0f, 1000.0f, 250.0f));
    addParameter (knee = new juce::AudioParameterFloat (
        "knee", "Knee", 0.0f, 96.0f, 18.0f));
    addParameter (autoGain = new juce::AudioParameterFloat (
        "autoGain", "Auto Gain", 0.0f, 1.0f, 0.5f));
}

VocalCompressorAudioProcessor::~VocalCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String VocalCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VocalCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VocalCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VocalCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VocalCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VocalCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocalCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VocalCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void VocalCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VocalCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    envelopeDetector.prepare(sampleRate);
    envelopeDetector.reset();
    
    compressor.prepare(sampleRate);
    compressor.reset();
}

void VocalCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocalCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VocalCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    envelopeDetector.setAttackTime(*attack / 1000);
    envelopeDetector.setReleaseTime(*release / 1000);
    compressor.setThreshold(*threshold);
    compressor.setRatio(*ratio);
    compressor.setKnee(*knee);
    
    float sample;
    float envelope;
    float gainReduction;
    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for (auto i = 0; i < buffer.getNumSamples(); i++)
        {
            sample = buffer.getSample(channel, i);
            envelope = envelopeDetector.getEnvelope(sample);
            gainReduction = compressor.getGainReduction(juce::Decibels::gainToDecibels(envelope));
            buffer.setSample(channel, i, sample * juce::Decibels::decibelsToGain(-gainReduction));
        }
    }
}

//==============================================================================
bool VocalCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VocalCompressorAudioProcessor::createEditor()
{
    return new VocalCompressorAudioProcessorEditor (*this);
}

//==============================================================================
void VocalCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("VocalCompressorParameter"));
    xml->setAttribute ("threshold", (double) *threshold);
    xml->setAttribute ("ratio", (double) *ratio);
    xml->setAttribute ("attack", (double) *attack);
    xml->setAttribute ("release", (double) *release);
    xml->setAttribute ("knee", (double) *knee);
    xml->setAttribute ("autoGain", (double) *autoGain);
    copyXmlToBinary (*xml, destData);
}

void VocalCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName ("VocalCompressorParameter"))
        {
            *threshold = (float) xmlState->getDoubleAttribute ("threshold", *threshold);
            *ratio = (float) xmlState->getDoubleAttribute ("ratio", *ratio);
            *attack = (float) xmlState->getDoubleAttribute ("attack", *attack);
            *release = (float) xmlState->getDoubleAttribute ("release", *release);
            *knee = (float) xmlState->getDoubleAttribute ("knee", *knee);
            *autoGain = (float) xmlState->getDoubleAttribute ("autoGain", *autoGain);
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalCompressorAudioProcessor();
}
