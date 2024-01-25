/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#if(_DEBUG)
#include "Debug/DebugPanel.h"
#endif
#include "PluginProcessor.h"
#include "VirtualResonators.h"
#include "GUI/GUI.h"

//==============================================================================


class VirtualResonatorsProcessorEditor : public VirtualResonatorsComponent<AudioProcessorEditor>
{
    // class ResonatorControl : public VirtualResonatorsComponent<>
    // {
    //     VirtualResonatorsProcessorEditor* _parent;
    // public:
    //     Label       _lbl;
    //     ComboBox    _noteval_cmb, _octave_cmb;
    //     Slider      _detune_sld, _decay_sld, _volume_sld, _damping_sld;
    //     VRShapeButton _pluck_btn, _toggle_btn;
    //     uptr<AudioProcessorValueTreeState::ComboBoxAttachment> _noteval_attachments, _octave_attachments;
    //     uptr<AudioProcessorValueTreeState::SliderAttachment>
    //       _detune_attachments, _decay_attachments, _damping_attachments, _volume_attachments;
    //     uptr<AudioProcessorValueTreeState::ButtonAttachment> _toggle_attachments;

    //     ResonatorControl();
    //     void initialize(VirtualResonatorsProcessorEditor* e, int index);
    //     void resized() override;
    // } _res_controls[NUM_RESONATORS];

    VRLookAndFeel    _look_and_feel;
    PresetControl    _presetControl;
    ResonatorControl _resonatorControls[NUM_RESONATORS];

#if(_DEBUG)
    TextButton _debug_btn;
#endif


    // I/O components
    DecibelSlider _in_sld, _out_sld;
    Slider        _wet_sld;

    // Footer components
    ComboBox _mono_stereo_cmb;

    // Paramter Attachments

    uptr<AudioProcessorValueTreeState::SliderAttachment>
        _in_attachment, _wet_attachment, _out_attachment;

    // Labels
    Label _noteval_lbl, _octave_lbl, _detune_lbl, _decay_lbl, _damping_lbl,
        _volume_lbl, _in_lbl, _wet_lbl, _out_lbl;

    VirtualResonatorsAudioProcessor& _audioProcessor;

public:
    VirtualResonatorsProcessorEditor (VirtualResonatorsAudioProcessor&);
    ~VirtualResonatorsProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualResonatorsProcessorEditor)
};


