#pragma once

#include "../PluginProcessor.h"
#include "../VirtualResonators.h"
#include "VRComponent.h"

class ResonatorControl : public VirtualResonatorsComponent<>
{
    VirtualResonatorsAudioProcessor& _audioProcessor;
public:
    Label       _lbl;
    ComboBox    _noteval_cmb, _octave_cmb;
    Slider      _detune_sld, _decay_sld, _volume_sld, _damping_sld;
    VRShapeButton _pluck_btn, _toggle_btn;
    uptr<AudioProcessorValueTreeState::ComboBoxAttachment> _noteval_attachments, _octave_attachments;
    uptr<AudioProcessorValueTreeState::SliderAttachment>
        _detune_attachments, _decay_attachments, _damping_attachments, _volume_attachments;
    uptr<AudioProcessorValueTreeState::ButtonAttachment> _toggle_attachments;

    ResonatorControl(VirtualResonatorsAudioProcessor& a, int index) : 
        _audioProcessor(a)
    {
        Path pluck_btn_path;
        pluck_btn_path.addEllipse(50, 200, 50, 50);

        Path toggle_btn_path;
        toggle_btn_path.addEllipse(0, 0, 20, 20);

        configComboBox(_noteval_cmb, notevals);
        configComboBox(_octave_cmb, { "0", "1", "2", "3", "4", "5", "6", "7", "8" });
        configRotarySlider(_detune_sld, -100.0, 100.0, 1.0);
        configRotarySlider(_decay_sld, 0.0, 100.0, 0.1);
        configRotarySlider(_damping_sld, 0.0, 100.0, 0.1);
        configRotarySlider(_volume_sld, 0.0, 10.0, 0.01);
        configShapeButton(_pluck_btn , "Pluck" , pluck_btn_path, Colour(128, 128, 128));
        configShapeButton(_toggle_btn, "On/Off", toggle_btn_path, Colour(30, 0, 0), false);
        _toggle_btn.setOnColours(
            Colour(128, 0, 0),
            Colour(128, 0, 0),
            Colour(128, 0, 0)
        );
        _toggle_btn.shouldUseOnColours     (true);
        _toggle_btn.setShape               (pluck_btn_path, false, false, false);
        _toggle_btn.setToggleable          (true);
        _toggle_btn.setClickingTogglesState(true);
        _toggle_btn.setToggleState         (true, NotificationType::dontSendNotification);

        _lbl.setText             (std::to_string(index + 1), NotificationType::dontSendNotification);
        _lbl.setJustificationType(Justification::horizontallyCentred);
        _lbl.attachToComponent   (&_noteval_cmb, false);

        _pluck_btn.onClick = [this, index] { _audioProcessor.pluckResonator(index); };

        // Attach GUI controls to parameters
        _noteval_attachments = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
            _audioProcessor._parameters,  NOTEVAL_ID(index), _noteval_cmb);
        _octave_attachments = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
            _audioProcessor._parameters,   OCTAVE_ID(index), _octave_cmb);
        _detune_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            _audioProcessor._parameters,   DETUNE_ID(index), _detune_sld);
        _decay_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            _audioProcessor._parameters,    DECAY_ID(index), _decay_sld);
        _damping_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            _audioProcessor._parameters,  DAMPING_ID(index), _damping_sld);
        _volume_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            _audioProcessor._parameters,   VOLUME_ID(index), _volume_sld);
        _toggle_attachments = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(
            _audioProcessor._parameters,   TOGGLE_ID(index), _toggle_btn);
    }
    // void initialize(VirtualResonatorsProcessorEditor* e, int index);
    void resized() override
    {
        auto diameter     = 80;
        auto combo_height = 20;

        Path toggle_icon_path;
        auto arc_start = -MathConstants<double>::pi / 3.0;
        auto arc_end   = 4 * MathConstants<double>::pi / 3.0;
        toggle_icon_path.addArc(0, 0, 16, 16, arc_start, arc_end);
        toggle_icon_path.addRectangle(8, 8, 2, 10);

        FlexBox resonator_flex;

        resonator_flex.flexWrap       = FlexBox::Wrap::wrap;
        resonator_flex.flexDirection  = FlexBox::Direction::column;
        resonator_flex.justifyContent = FlexBox::JustifyContent::spaceBetween;
        resonator_flex.alignItems     = FlexBox::AlignItems::flexEnd;

        resonator_flex.items.add(FlexItem(_noteval_cmb)
            .withWidth(diameter)
            .withHeight(combo_height)
            .withMargin(FlexItem::Margin(30, 0, 0, 0)));
        resonator_flex.items.add(FlexItem(_octave_cmb)
            .withWidth(diameter)
            .withHeight(combo_height));
        resonator_flex.items.add(FlexItem(_detune_sld)
            .withWidth(diameter)
            .withHeight(diameter));
        resonator_flex.items.add(FlexItem(_decay_sld)
            .withWidth(diameter)
            .withHeight(diameter));
        resonator_flex.items.add(FlexItem(_damping_sld)
            .withWidth(diameter)
            .withHeight(diameter));
        resonator_flex.items.add(FlexItem(_volume_sld)
            .withWidth(diameter)
            .withHeight(diameter));
        resonator_flex.items.add(FlexItem(_pluck_btn)
            .withWidth(diameter)
            .withHeight(diameter));
        resonator_flex.items.add(FlexItem(_toggle_btn)
            .withWidth(diameter)
            .withHeight(diameter));
        resonator_flex.performLayout(getLocalBounds());
    }
};
