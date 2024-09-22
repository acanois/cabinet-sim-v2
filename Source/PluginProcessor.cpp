/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicAmpSimAudioProcessor::BasicAmpSimAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
mValueTree(*this, nullptr, "ValueTree", {
    std::make_unique<juce::AudioParameterFloat> ("input", "Input", juce::NormalisableRange<float>   (0.f, 2.f, 0.001f), mInput.get()),
    std::make_unique<juce::AudioParameterFloat> ("output", "Output", juce::NormalisableRange<float> (0.f, 2.f, 0.001f), mOutput.get())
})
{
    mValueTree.addParameterListener("input", this);
    mValueTree.addParameterListener("output", this);
    
    // This is ridiculous. I realize that.
    auto dir = juce::File::getSpecialLocation (juce::File::currentApplicationFile)
        .getParentDirectory()
        .getParentDirectory()
        .getParentDirectory()
        .getParentDirectory()
        .getParentDirectory()
        .getChildFile("Resources");
    
    auto& convolution = processorChain.template get<convolutionIndex>();
    convolution.loadImpulseResponse(
            dir.getChildFile("celestion_g12h_75_creamback_1x12_heavy_mix.wav"),
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            1024
    );
}

BasicAmpSimAudioProcessor::~BasicAmpSimAudioProcessor()
{
}

//==============================================================================
const juce::String BasicAmpSimAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicAmpSimAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicAmpSimAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicAmpSimAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicAmpSimAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicAmpSimAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicAmpSimAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicAmpSimAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicAmpSimAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicAmpSimAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicAmpSimAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    auto& preGain = processorChain.template get<preGainIndex>();
    auto& postGain = processorChain.template get<postGainIndex>();
    
    preGain.setGainLinear(1.f);
    postGain.setGainLinear(1.f);
    
    processorChain.prepare(spec);
}

void BasicAmpSimAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicAmpSimAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
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

void BasicAmpSimAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    juce::dsp::AudioBlock<float> processBlock (buffer);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    processorChain.process (juce::dsp::ProcessContextReplacing<float> (processBlock));
}

//===============================s===============================================
bool BasicAmpSimAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicAmpSimAudioProcessor::createEditor()
{
    return new BasicAmpSimAudioProcessorEditor (*this);
}

//==============================================================================
void BasicAmpSimAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // For saving parameter state
    auto state = mValueTree.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BasicAmpSimAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Also for saving parameter state
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (mValueTree.state.getType()))
            mValueTree.replaceState (juce::ValueTree::fromXml (*xmlState));
}

void BasicAmpSimAudioProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == "input") {
        mInput.set(newValue);
        auto& preGain = processorChain.template get<preGainIndex>();
        preGain.setGainLinear(mInput.get());
    }
    if (parameterID == "output") {
        mOutput.set (newValue);
        auto& postGain = processorChain.template get<postGainIndex>();
        postGain.setGainLinear(mOutput.get());
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicAmpSimAudioProcessor();
}
