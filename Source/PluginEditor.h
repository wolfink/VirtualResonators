/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DecibelSlider.h"

typedef enum Parameters {
    AddNoise,
	BufferToggle,
	DecayTime,
	Frequency,
	OutputVolume,
	ResonatorVolume
} Parameter;

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

	void setParameter(Parameter parameter, juce::var& value, int index=0);

#if(_DEBUG)
    juce::ShapeButton *bufferView;
    juce::ShapeButton *componentView;
    juce::ShapeButton *valueTreeView;
    juce::ShapeButton *fontAndColourView;

    void toggleComponentDebugger();
    void openValueTreeDebugger();
    void toggleFontAndColourDesigner();

    jcf::ComponentDebugger* componentDebugger;
    jcf::ValueTreeDebugger* valueTreeDebugger;
    jcf::FontAndColourDesigner* fontAndColourDesigner;
    bool componentDebuggerOn;
    bool fontAndColourDesignerOn;
#endif

    juce::Slider  resonatorFrequency[NUM_RESONATORS];
    juce::Slider  resonatorDecay[NUM_RESONATORS];
    juce::Slider  volumeSlider[NUM_RESONATORS];
    juce::ShapeButton* pulseButtons[NUM_RESONATORS];
    DecibelSlider outputVolumeSlider;

    juce::Label   resonatorNumberLabel[NUM_RESONATORS];
    juce::Label   resonatorFrequencyLabel;
    juce::Label   resonatorDecayLabel;
    juce::Label   volumeSliderLabel;
    juce::Label   outputVolumeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessorEditor)
};
