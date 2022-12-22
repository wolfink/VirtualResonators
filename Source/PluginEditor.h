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
#include "VirtualResonators.h"

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
    ResonatorProjectAudioProcessor& audio_processor;

#if(_DEBUG)
    ShapeButton* buffer_view;
    ShapeButton* component_view;
    ShapeButton* valueTree_view;
    ShapeButton* fontAndColour_view;

    void toggleComponentDebugger();
    void openValueTreeDebugger();
    void toggleFontAndColourDesigner();

    jcf::ComponentDebugger*     componentDebugger     = nullptr;
    jcf::ValueTreeDebugger*     valueTreeDebugger     = nullptr;
    jcf::FontAndColourDesigner* fontAndColourDesigner = nullptr;
#endif

    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>> note_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>> octave_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>> decay_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>> volume_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>> damping_attachments;

    uptr<AudioProcessorValueTreeState::SliderAttachment> input_attachment;
    uptr<AudioProcessorValueTreeState::SliderAttachment> wet_attachment;
    uptr<AudioProcessorValueTreeState::SliderAttachment> output_attachment;

    NoteSlider        resonator_note_values[NUM_RESONATORS];
    Slider            resonator_octaves    [NUM_RESONATORS];
    Slider            resonator_decays     [NUM_RESONATORS];
    Slider            resonator_volumes    [NUM_RESONATORS];
    Slider            resonator_dampings   [NUM_RESONATORS];
    uptr<ShapeButton> pluck_buttons        [NUM_RESONATORS];
    DecibelSlider     input_volume_slider;
    Slider            wet_slider;
    DecibelSlider     output_volume_slider;

    Label resonator_number_labels[NUM_RESONATORS];
    Label resonator_note_value_label;
    Label resonator_octave_label;
    Label resonator_decay_label;
    Label resonator_damping_label;
    Label volume_slider_label;
    Label input_volume_label;
    Label wet_label;
    Label output_volume_label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessorEditor)
};
