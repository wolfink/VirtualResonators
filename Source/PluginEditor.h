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

    juce::Slider  resonatorFrequency[NUM_RESONATORS];
    juce::Slider  resonatorFeedback[NUM_RESONATORS];
    juce::Slider  volumeSlider[NUM_RESONATORS];
    juce::ShapeButton* pulseButtons[NUM_RESONATORS];
    DecibelSlider outputVolumeSlider;

    juce::Label   resonatorNumberLabel[NUM_RESONATORS];
    juce::Label   resonatorFrequencyLabel;
    juce::Label   resonatorFeedbackLabel;
    juce::Label   volumeSliderLabel;
    juce::Label   outputVolumeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessorEditor)
};
