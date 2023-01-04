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

//==============================================================================
/**
*/
class ResonatorProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
    struct ResonatorControl {
        Label             _lbl;
		ComboBox          _noteval_cmb;
		ComboBox          _octave_cmb;
		Slider            _detune_sld;
		Slider            _decay_sld;
		Slider            _volume_sld;
		Slider            _damping_sld;
		uptr<ShapeButton> _pluck_btn;
		uptr<ShapeButton> _toggle_btn;
    } _res_controls[NUM_RESONATORS];

    // I/O components
    DecibelSlider     _in_sld;
    Slider            _wet_sld;
    DecibelSlider     _out_sld;

    // Footer components
    ComboBox _mono_stereo_cmb;

    // Paramter Attachments
    std::vector<uptr<AudioProcessorValueTreeState::ComboBoxAttachment>> _noteval_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::ComboBoxAttachment>> _octave_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>>   _detune_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>>   _decay_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>>   _damping_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>>   _volume_attachments;
    std::vector<uptr<AudioProcessorValueTreeState::ButtonAttachment>>   _toggle_attachments;
    uptr<AudioProcessorValueTreeState::SliderAttachment> _in_attachment;
    uptr<AudioProcessorValueTreeState::SliderAttachment> _wet_attachment;
    uptr<AudioProcessorValueTreeState::SliderAttachment> _out_attachment;

    // Labels
    Label _noteval_lbl;
    Label _octave_lbl;
    Label _detune_lbl;
    Label _decay_lbl;
    Label _damping_lbl;
    Label _volume_lbl;
    Label _in_lbl;
    Label _wet_lbl;
    Label _out_lbl;


#if(_DEBUG)
    ShapeButton* _buffer_view;
    ShapeButton* _component_view;
    ShapeButton* _valueTree_view;
    ShapeButton* _fontAndColour_view;

    void toggleComponentDebugger();
    void openValueTreeDebugger();
    void toggleFontAndColourDesigner();

    jcf::ComponentDebugger*     _componentDebugger     = nullptr;
    jcf::ValueTreeDebugger*     _valueTreeDebugger     = nullptr;
    jcf::FontAndColourDesigner* _fontAndColourDesigner = nullptr;
#endif

    ResonatorProjectAudioProcessor& _audioProcessor;

public:

    ResonatorProjectAudioProcessorEditor (ResonatorProjectAudioProcessor&);
    ~ResonatorProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessorEditor)
};
