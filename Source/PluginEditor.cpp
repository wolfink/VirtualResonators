/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <sstream>

#define SET_AND_ATTACH_LABEL(label, component, text, left) {\
    Font font(18.0, Font::FontStyleFlags::bold);\
	label.setText           (text, NotificationType::dontSendNotification);\
    label.setJustificationType(Justification::horizontallyCentred);\
    label.setFont(font);\
	label.attachToComponent (component, left);\
}

//==============================================================================
VirtualResonatorsProcessorEditor::VirtualResonatorsProcessorEditor(VirtualResonatorsAudioProcessor& p): VirtualResonatorsComponent(&p),
    _audioProcessor(p),
    _resonatorControls {
        ResonatorControl(p, 0),
        ResonatorControl(p, 1),
        ResonatorControl(p, 2),
        ResonatorControl(p, 3),
        ResonatorControl(p, 4),
        ResonatorControl(p, 5),
        ResonatorControl(p, 6),
        ResonatorControl(p, 7),
        ResonatorControl(p, 8),
        ResonatorControl(p, 9),
        ResonatorControl(p, 10),
        ResonatorControl(p, 11)
    },
    _presetControl(p)
{
  setLookAndFeel(&_look_and_feel);
#if(_DEBUG)
  configTextButton(_debug_btn, "Debug");
  _debug_btn.onClick = [this] {
      DebugPanel::getInstance()->open();
  };
#endif
    // Set parameters for volume and mix sliders
	configVertSlider(_out_sld, -100.0,  12.0, 0.1);
	configVertSlider(_in_sld , -100.0,  12.0, 0.1);
	configVertSlider(_wet_sld,    0.0, 100.0, 0.1);
	_wet_sld.setTextValueSuffix("%");

	// Attach volume and mix sliders to parameters
	_in_attachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_audioProcessor._parameters, INPUT_ID , _in_sld);
	_out_attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_audioProcessor._parameters, OUTPUT_ID, _out_sld);
	_wet_attachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_audioProcessor._parameters, WET_ID   , _wet_sld);

	configComboBox(_mono_stereo_cmb, { "Stereo", "Mono" });
	_mono_stereo_cmb.setSelectedId(1);
	_mono_stereo_cmb.onChange = [this] {
		switch (_mono_stereo_cmb.getSelectedId()) {
        case 1:  _audioProcessor._mono = false; break;
        case 2:  _audioProcessor._mono = true;  break;
        default: break;
		}
	};


    // Initialize and add resonator controls
	for (int i = 0; i < NUM_RESONATORS; i++) {
		addAndMakeVisible(_resonatorControls[i]);
	}
	addAndMakeVisible(_presetControl);

	SET_AND_ATTACH_LABEL(_noteval_lbl, &_resonatorControls[0]._noteval_cmb, "Note"   , true);
	SET_AND_ATTACH_LABEL(_octave_lbl , &_resonatorControls[0]._octave_cmb , "Octave" , true);
	SET_AND_ATTACH_LABEL(_detune_lbl , &_resonatorControls[0]._detune_sld , "Detune" , true);
	SET_AND_ATTACH_LABEL(_decay_lbl  , &_resonatorControls[0]._decay_sld  , "Decay"  , true);
	SET_AND_ATTACH_LABEL(_damping_lbl, &_resonatorControls[0]._damping_sld, "Damping", true);
	SET_AND_ATTACH_LABEL(_volume_lbl , &_resonatorControls[0]._volume_sld , "Volume" , true);
	SET_AND_ATTACH_LABEL(_out_lbl, &_out_sld, "Out", false);
	SET_AND_ATTACH_LABEL(_in_lbl , &_in_sld , "In" , false);
	SET_AND_ATTACH_LABEL(_wet_lbl, &_wet_sld, "Mix", false);

  setSize (1400, 800);
}

VirtualResonatorsProcessorEditor::~VirtualResonatorsProcessorEditor()
{
}

//==============================================================================
void VirtualResonatorsProcessorEditor::paint (juce::Graphics& g)
{
    const auto windowBackground = _look_and_feel
        .getCurrentColourScheme()
        .getUIColour(VRLookAndFeel::ColourScheme::UIColour::windowBackground);
    const auto title_color = _look_and_feel
        .getCurrentColourScheme()
        .getUIColour(VRLookAndFeel::ColourScheme::UIColour::defaultFill);
    const auto center_x = getWidth() * 0.5;
    const auto text_width = 250.0;
    const auto title_size = 25.0;
    const auto info_size  = 18.0;
    const auto text_x   = 20.0;
    const auto text_y   = 10.0;
    const Font title_font(title_size, Font::FontStyleFlags::bold);
    const Font info_font(info_size, Font::FontStyleFlags::bold);

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (windowBackground);

    g.setColour(Colours::linen);
    g.fillRoundedRectangle(text_x, text_y, text_width, title_size + text_y, 15.0);

    g.setColour(title_color);
    g.setFont (title_font);
    g.drawFittedText("Virtual Resonators", 
                     text_x, text_y + 5.0,
                     text_width, title_size,
                     juce::Justification::centred, 1);

    g.setFont(info_font);
    g.setColour (Colours::linen);
    g.drawFittedText("by",
                     text_x + text_width + 15.0, text_y + 12.0,
                     25.0, info_size,
                     Justification::centred, 1);

    g.setColour (Colours::peru);
    g.drawFittedText("wolfink",
                     text_x + text_width + 40.0, text_y + 12.0,
                     70.0, info_size,
                     Justification::centred, 1);

    g.setColour (Colours::linen);
    std::stringstream version_string;
    version_string << "v" << PLUGIN_VERSION;
    g.drawFittedText(version_string.str(),
                     text_x + text_width + 110.0, text_y + 12.0,
                     70.0, info_size,
                     Justification::centred, 1);
}

void VirtualResonatorsProcessorEditor::resized()
{
	auto area = getLocalBounds();
	auto header_height = 60;
	auto footer_height = 60;
	auto section_width = 80;
	auto section_pad   = 10;
	auto combo_height  = 20;
    auto label_margin  = 30;

	auto header_area = area.removeFromTop(header_height);

#if(_DEBUG)
    _debug_btn.setBounds(header_area.removeFromRight(section_width));
#endif

	//_mono_stereo_cmb.setBounds(getWidth() - section_width, getHeight() - 60, section_width, 40);

	FlexBox mono_stereo_section;
	auto footer_area = area.removeFromBottom(footer_height);

	footer_area.removeFromLeft(2 * section_width);

	mono_stereo_section.justifyContent = FlexBox::JustifyContent::center;
	mono_stereo_section.alignItems     = FlexBox::AlignItems::center;
	mono_stereo_section.items.add(FlexItem(_mono_stereo_cmb).withHeight(50).withWidth(section_width));
	mono_stereo_section.performLayout(footer_area.removeFromRight(2 * section_width));

	_presetControl.setBounds(footer_area);

	_in_sld.setBounds(area
                    .removeFromLeft(section_width)
                    .removeFromBottom(area.getHeight() - label_margin));
	_out_sld.setBounds(area
                    .removeFromRight(section_width)
                    .removeFromBottom(area.getHeight() - label_margin));
	_wet_sld.setBounds(area
                    .removeFromRight(section_width)
                    .removeFromBottom(area.getHeight() - label_margin));

	for (int i = 0; i < NUM_RESONATORS; i++) {
		if (i == 0) _resonatorControls[i].setBounds(area.removeFromLeft(2 * section_width));
		else        _resonatorControls[i].setBounds(area.removeFromLeft(section_width));
		if (i < NUM_RESONATORS - 1) area.removeFromLeft(section_pad);
	}
}
