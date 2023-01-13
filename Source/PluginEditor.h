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
class VRShapeButton : public ShapeButton {
public:
    VRShapeButton() : ShapeButton("Noname", Colours::white, Colours::white, Colours::black) { }
};

template<class ComponentClass = Component>
class VirtualResonatorsComponent : public ComponentClass
{
public:

    VirtualResonatorsComponent() : ComponentClass() {}

    template<typename CC = ComponentClass,
        typename std::enable_if_t<std::is_same<CC, AudioProcessorEditor>::value, bool> = true>
    VirtualResonatorsComponent(AudioProcessor& p) : AudioProcessorEditor(p) {}

    template<typename CC = ComponentClass,
        typename std::enable_if_t<std::is_same<CC, AudioProcessorEditor>::value, bool> = true>
    VirtualResonatorsComponent(AudioProcessor* p) : AudioProcessorEditor(p) {}

	void configComboBox(ComboBox& box, StringArray items) {
        box.addItemList(items, 1);
        addAndMakeVisible(box);
	}

	void configRotarySlider(Slider& slider, float low, float high, float step) {
		slider.setSliderStyle           (Slider::RotaryHorizontalVerticalDrag);
        configSlider(slider, low, high, step);
	}

	void configVertSlider(Slider& slider, float low, float high, float step) {
		slider.setSliderStyle           (Slider::LinearVertical);
        configSlider(slider, low, high, step);
	}

	void configShapeButton(VRShapeButton& button, String name, Path path, Colour color, bool change_color=true) {
        int R     = color.getRed();
        int G     = color.getGreen();
        int B     = color.getBlue();
        if (change_color) {
            Colour over_color(std::min(R + 30, 255), std::min(G + 30, 255), std::min(B + 30, 255));
            Colour down_color(std::max(R - 30, 0), std::max(G - 30, 0), std::max(B - 30, 0));
            button.setColours(color, over_color, down_color);
        } else {
            button.setColours(color, color, color);
        }

        button.setName(name);
        button.setShape(path, false, false, false);
        addAndMakeVisible(button);
	}

    void configTextButton(TextButton& button, String text) {
        button.setButtonText(text);
        addAndMakeVisible(button);
    }
private:
    void configSlider(Slider& slider, float low, float high, float step) {
		slider.setRange                 (low, high, step);
		slider.setPopupDisplayEnabled   (true, false, this);
		slider.setTextBoxStyle          (Slider::NoTextBox, false, 90, 0);
		addAndMakeVisible(slider);
    }
};

class VirtualResonatorsProcessorEditor : public VirtualResonatorsComponent<AudioProcessorEditor>
{
    struct ResonatorControl {
        Label       _lbl;
		ComboBox    _noteval_cmb, _octave_cmb;
        Slider      _detune_sld, _decay_sld, _volume_sld, _damping_sld;
		VRShapeButton _pluck_btn, _toggle_btn;
    } _res_controls[NUM_RESONATORS];

    struct PresetControl {
        VRShapeButton _left_btn, _right_btn;
        TextButton  _save_btn, _delete_btn;
        ComboBox    _preset_name;
    } _preset_control;

    // I/O components
    DecibelSlider _in_sld, _out_sld;
    Slider        _wet_sld;

    // Footer components
    ComboBox _mono_stereo_cmb;

    // Paramter Attachments
    std::vector<uptr<AudioProcessorValueTreeState::ComboBoxAttachment>> 
        _noteval_attachments, _octave_attachments;

    std::vector<uptr<AudioProcessorValueTreeState::SliderAttachment>>
        _detune_attachments, _decay_attachments, _damping_attachments, _volume_attachments;

    std::vector<uptr<AudioProcessorValueTreeState::ButtonAttachment>>
        _toggle_attachments;

    uptr<AudioProcessorValueTreeState::SliderAttachment>
        _in_attachment, _wet_attachment, _out_attachment;

    // Labels
    Label _noteval_lbl, _octave_lbl, _detune_lbl, _decay_lbl, _damping_lbl,
        _volume_lbl, _in_lbl, _wet_lbl, _out_lbl;


#if(_DEBUG)
    struct DebugPanel : public VirtualResonatorsComponent<> {
        VirtualResonatorsProcessorEditor& parent;
		jcf::ComponentDebugger _componentDebugger;
		jcf::ValueTreeDebugger _valueTreeDebugger;
    public:
		VRShapeButton _buffer_view, _component_view, _valueTree_view;

        DebugPanel(VirtualResonatorsProcessorEditor& e);
		void toggleComponentDebugger();
		void openValueTreeDebugger();

        void resized() override;
    } _debug;
#endif

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


