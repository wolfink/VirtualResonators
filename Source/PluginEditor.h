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


