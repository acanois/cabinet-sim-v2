/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicAmpSimAudioProcessorEditor::BasicAmpSimAudioProcessorEditor (BasicAmpSimAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    std::array<std::string, 2> controlNames = { "input", "output" };

    for (auto name : controlNames)
    {
        auto* slider = new juce::Slider(name);
        auto* sliderAttachment = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getValueTreeState(), name, *slider);
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 25);
        mGainControls.add(slider);
        mDistAttachments.add(sliderAttachment);
        addAndMakeVisible(slider);
    }
    
    sliderBounds.setBounds(0, 100, 100, 100);
    mComponentBounds.setBounds(100, 100, (sliderBounds.getWidth() * 4), (sliderBounds.getHeight()));
    
    setSize (400, 300);
}

BasicAmpSimAudioProcessorEditor::~BasicAmpSimAudioProcessorEditor()
{
}

//==============================================================================
void BasicAmpSimAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void BasicAmpSimAudioProcessorEditor::resized()
{
    int xPos = mComponentBounds.getX() / 2;
    for (juce::Slider* control : mGainControls) {
        control->setBounds(xPos, mComponentBounds.getY(), sliderBounds.getWidth(), sliderBounds.getHeight());
        xPos += (sliderBounds.getX() + sliderBounds.getWidth() + 100);
    }
}
