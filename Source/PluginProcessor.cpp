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
    addParameter (threshold = new juce::AudioParameterFloat ({"threshold", 1}, "Threshold", -60.0f, 0.0f, -18.0f));
    addParameter (ratio = new juce::AudioParameterFloat ({"ratio", 1}, "Ratio", 1.0f, 20.0f, 4.0f));
    addParameter (attack = new juce::AudioParameterFloat ({"attack", 1}, "Attack", 0.0f, 50.0f, 5.0f));
    addParameter (release = new juce::AudioParameterFloat ({"release", 1}, "Release", 0.0f, 1000.0f, 250.0f));
    addParameter (knee = new juce::AudioParameterFloat ({"knee", 1}, "Knee", 0.0f, 96.0f, 18.0f));
    addParameter (autoGain = new juce::AudioParameterFloat ({"autoGain", 1}, "Auto Gain", 0.0f, 1.0f, 0.5f));
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
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    envelopeFilter.prepare(spec);
    envelopeFilter.reset();
    
    gain.prepare(spec);
    gain.reset();
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

float VocalCompressorAudioProcessor::getGainReduction(float envelope)
{
    float lowerKneeBound = *threshold - (*knee / 2.0f);
    float upperKneeBound = *threshold + (*knee / 2.0f);
    
    float gainReduction = 1.0f - (1.0f / *ratio);

    if (envelope < lowerKneeBound)
    {
        return 0.0;
    }
    else if (envelope < upperKneeBound)
    {
        gainReduction *= ((envelope - lowerKneeBound) / *knee) / 2.0f;
        return gainReduction * (lowerKneeBound - envelope);
    }
    else
    {
        return gainReduction * (*threshold - envelope);
    }
}

float VocalCompressorAudioProcessor::processSample (int channel, float sample)
{
    float envelope = juce::Decibels::gainToDecibels(envelopeFilter.processSample(channel, sample));
    float gain = getGainReduction(envelope);
    return sample * juce::Decibels::decibelsToGain(gain);
}

void VocalCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    juce::dsp::AudioBlock<float> block (buffer);
    auto context = juce::dsp::ProcessContextReplacing<float> (block);

    envelopeFilter.setAttackTime(*attack);
    envelopeFilter.setReleaseTime(*release);
    
    for (int channel = 0; channel < block.getNumChannels(); channel++)
    {
        float* samples  = block.getChannelPointer(channel);
        for (int i = 0; i < block.getNumSamples(); ++i)
            samples[i] = processSample(channel, samples[i]);
    }
    
    gain.setGainDecibels(-getGainReduction(-0.0f) * *autoGain);
    gain.process(context);
    
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
