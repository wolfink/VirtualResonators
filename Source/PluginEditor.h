/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DecibelSlider.h"
#include "NoteSlider.h"

enum class Parameter {
    AddNoise,
    BufferToggle,
    DecayTime,
    Frequency,
    OutputVolume,
    InputVolume,
    WetDry,
    ResonatorVolume
};

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
    NoteSlider    resonatorNoteValue[NUM_RESONATORS];
    double resonatorBaseFrequency[NUM_RESONATORS]; // = { 32.70, 36.71, 41.20, 43.65, 49.00, 55.00, 61.74, 32.70 };
    juce::Slider  resonatorOctave[NUM_RESONATORS];
    double resonatorMultiplier[NUM_RESONATORS]; // = { 8.0,   8.0,   8.0,   8.0,   8.0,   8.0,   8.0,  16.0 };
    juce::Slider  resonatorDecay[NUM_RESONATORS];
    juce::Slider  volumeSlider[NUM_RESONATORS];
    juce::ShapeButton* pulseButtons[NUM_RESONATORS];
    DecibelSlider inputVolumeSlider;
    juce::Slider  wetDrySlider;
    DecibelSlider outputVolumeSlider;

    juce::Label   resonatorNumberLabel[NUM_RESONATORS];
    juce::Label   resonatorFrequencyLabel;
    juce::Label   resonatorNoteValueLabel;
    juce::Label   resonatorOctaveLabel;
    juce::Label   resonatorDecayLabel;
    juce::Label   volumeSliderLabel;
    juce::Label   inputVolumeLabel;
    juce::Label   wetDryLabel;
    juce::Label   outputVolumeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessorEditor)
};
