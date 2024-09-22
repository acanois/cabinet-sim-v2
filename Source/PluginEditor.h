/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class MainPanel : public juce::Component,
juce::Slider::Listener
{
public:
    MainPanel(BasicAmpSimAudioProcessor& processor);
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    void sliderValueChanged (juce::Slider* slider) override;
    
    int getComponentHeight() { return mComponentBounds.getHeight(); }
    
private:
    juce::OwnedArray<juce::Slider> mGainControls;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> mDistAttachments;
    
    juce::Rectangle<int> mComponentBounds;
    juce::Rectangle<int> sliderBounds;
};

//==============================================================================
/**
*/
class BasicAmpSimAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicAmpSimAudioProcessorEditor (BasicAmpSimAudioProcessor&);
    ~BasicAmpSimAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::OwnedArray<juce::Slider> mGainControls;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> mDistAttachments;
    
    juce::Rectangle<int> mComponentBounds;
    juce::Rectangle<int> sliderBounds;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicAmpSimAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicAmpSimAudioProcessorEditor)
};
