/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DecibelSlider.h"

//==============================================================================
/**
*/
class ResonatorProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ResonatorProjectAudioProcessorEditor (ResonatorProjectAudioProcessor&);
    ~ResonatorProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ResonatorProjectAudioProcessor& audioProcessor;

    juce::Slider resonatorFrequency;
    juce::Label  resonatorFrequencyLabel;
    juce::Slider resonatorFeedback;
    juce::Label  resonatorFeedbackLabel;
    juce::ShapeButton pulseButton { juce::String("Pulse"), juce::Colour(128, 128, 128), juce::Colour(150, 150, 150), juce::Colour(90, 90, 90) };
    juce::Slider volumeSlider;
    juce::Label  volumeSliderLabel;
    DecibelSlider outputVolumeSlider;
    juce::Label outputVolumeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessorEditor)
};
