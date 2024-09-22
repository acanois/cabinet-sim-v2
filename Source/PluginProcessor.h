/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class BasicAmpSimAudioProcessor  : public juce::AudioProcessor,
                                   public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    BasicAmpSimAudioProcessor();
    ~BasicAmpSimAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    
    juce::AudioProcessorValueTreeState& getValueTreeState() { return mValueTree; }
    

private:
    //==============================================================================
    juce::Atomic<float> mInput  { 0.f };
    juce::Atomic<float> mOutput { 0.f };
    
    // Initialize everything below after the atomic parameters
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>, juce::dsp::Convolution, juce::dsp::Gain<float>> processorChain;
    
    juce::AudioProcessorValueTreeState mValueTree;
    
    enum
    {
        preGainIndex,
        convolutionIndex,
        postGainIndex
    };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicAmpSimAudioProcessor)
};
