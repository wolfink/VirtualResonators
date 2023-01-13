/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VirtualResonatorsProcessorEditor::VirtualResonatorsProcessorEditor(VirtualResonatorsAudioProcessor& p)
	: VirtualResonatorsComponent(&p), _audioProcessor(p), _debug(*this)
{

    // Useful variables
	juce::Path path;
	path.addEllipse(50, 200, 50, 50);

    // Set parameters for resonator components
    for (int i = 0; i < NUM_RESONATORS; i++) {
		ResonatorControl& res = _res_controls[i];

		configComboBox(res._noteval_cmb, notevals);

		res._lbl.setText          (std::to_string(i + 1), NotificationType::dontSendNotification);
		res._lbl.attachToComponent(&res._noteval_cmb, false);

		configComboBox(res._octave_cmb, { "0", "1", "2", "3", "4", "5", "6", "7", "8" });

		configRotarySlider(res._detune_sld, -100.0, 100.0, 1.0);
		configRotarySlider(res._decay_sld, 0.0, 100.0, 0.1);
		configRotarySlider(res._damping_sld, 0.0, 100.0, 0.1);
		configRotarySlider(res._volume_sld, 0.0, 10.0, 0.01);

		configShapeButton(res._pluck_btn , "Pluck" , path, Colour(128, 128, 128));
		res._pluck_btn.onClick = [this, i] { _audioProcessor.pluckResonator(i); };

		configShapeButton(res._toggle_btn, "On/Off", path, Colour(30, 0, 0), false);
		res._toggle_btn.setOnColours(
			Colour(128, 0, 0),
			Colour(128, 0, 0),
			Colour(128, 0, 0)
		);
		res._toggle_btn.shouldUseOnColours     (true);
		res._toggle_btn.setShape               (path, false, false, false);
		res._toggle_btn.setToggleable          (true);
		res._toggle_btn.setClickingTogglesState(true);
		res._toggle_btn.setToggleState         (true, NotificationType::dontSendNotification);
    }

    // Set parameters for volume and mix sliders
	configVertSlider(_out_sld, -100.0,  12.0, 0.1);
	configVertSlider(_in_sld , -100.0,  12.0, 0.1);
	configVertSlider(_wet_sld,    0.0, 100.0, 0.1);
	_wet_sld.setTextValueSuffix("%");

    //_out_sld.setSkewFactorFromMidPoint (0.0);
    //_in_sld.setSkewFactorFromMidPoint  (0.0);


	configComboBox(_mono_stereo_cmb, { "Stereo", "Mono" });
	_mono_stereo_cmb.setSelectedId(1);
	_mono_stereo_cmb.onChange = [this] {
		switch (_mono_stereo_cmb.getSelectedId()) {
			case 1:  _audioProcessor._mono = true;  break;
			case 2:  _audioProcessor._mono = false; break;
			default: break;
		}
	};

	{
		Path preset_btn_shape;
		preset_btn_shape.addTriangle(0, 10, 20, 20, 20, 0);
		configShapeButton(_preset_control._left_btn , "LeftPre" , preset_btn_shape, Colour(128, 128, 128));
		preset_btn_shape.applyTransform(AffineTransform::rotation(MathConstants<float>::pi));
		configShapeButton(_preset_control._right_btn, "RightPre", preset_btn_shape, Colour(128, 128, 128));
		configComboBox(_preset_control._preset_name, { "Empty" });
		configTextButton(_preset_control._save_btn, "Save");
	}

	// Attach components to audio processer state tree
	for (size_t i = 0; i < NUM_RESONATORS; i++) {
		ResonatorControl& res = _res_controls[i];
		_noteval_attachments.push_back   (std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
			_audioProcessor._parameters,  NOTEVAL_ID(i), res._noteval_cmb));
		_octave_attachments.push_back (std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
			_audioProcessor._parameters, OCTAVE_ID(i), res._octave_cmb));
		_detune_attachments.push_back (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			_audioProcessor._parameters,   DETUNE_ID(i), res._detune_sld));
		_decay_attachments.push_back  (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			_audioProcessor._parameters,    DECAY_ID(i), res._decay_sld));
		_damping_attachments.push_back  (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			_audioProcessor._parameters,  DAMPING_ID(i), res._damping_sld));
		_volume_attachments.push_back (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			_audioProcessor._parameters,   VOLUME_ID(i), res._volume_sld));
		_toggle_attachments.push_back (std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(
			_audioProcessor._parameters,   TOGGLE_ID(i), res._toggle_btn));
	}

	_in_attachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_audioProcessor._parameters, INPUT_ID , _in_sld);
	_out_attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_audioProcessor._parameters, OUTPUT_ID, _out_sld);
	_wet_attachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_audioProcessor._parameters, WET_ID   , _wet_sld);

    // Set parameters for labels
	_noteval_lbl.setText           ("Note", NotificationType::dontSendNotification);
	_noteval_lbl.attachToComponent (&_res_controls[0]._noteval_cmb, true);

	_octave_lbl.setText               ("Octave", NotificationType::dontSendNotification);
	_octave_lbl.attachToComponent     (&_res_controls[0]._octave_cmb, true);

	_detune_lbl.setText               ("Detune", NotificationType::dontSendNotification);
	_detune_lbl.attachToComponent     (&_res_controls[0]._detune_sld, true);

    _decay_lbl.setText                ("Decay", NotificationType::dontSendNotification);
    _decay_lbl.attachToComponent      (&_res_controls[0]._decay_sld, true);

	_damping_lbl.setText              ("Damping", NotificationType::dontSendNotification);
	_damping_lbl.attachToComponent    (&_res_controls[0]._damping_sld, true);
    
	_volume_lbl.setText                  ("Volume", NotificationType::dontSendNotification);
	_volume_lbl.attachToComponent        (&_res_controls[0]._volume_sld, true);

    _out_lbl.setText                  ("Out", NotificationType::dontSendNotification);
    _out_lbl.attachToComponent        (&_out_sld, false);

    _in_lbl.setText                   ("In", NotificationType::dontSendNotification);
    _in_lbl.attachToComponent         (&_in_sld, false);

    _wet_lbl.setText                            ("Wet", NotificationType::dontSendNotification);
    _wet_lbl.attachToComponent                  (&_wet_sld, false);

#if(_DEBUG)
	addAndMakeVisible(_debug);

	setSize(1090, 780);
#else
    setSize (1040, 780);
#endif
}

VirtualResonatorsProcessorEditor::~VirtualResonatorsProcessorEditor()
{
}

//==============================================================================
void VirtualResonatorsProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (25.0f);
    g.drawFittedText("Virtual Resonators", 0, 0, getWidth(), 40, juce::Justification::centred, 1);
}

void VirtualResonatorsProcessorEditor::resized()
{
	auto area = getLocalBounds();

#if(_DEBUG)
	_debug.setBounds(area.removeFromRight(50));
#endif

	auto header_height = 80;
	auto footer_height = 80;
	auto section_width = 80;
	auto section_pad   = 10;
	auto combo_height  = 20;
	auto preset_height = 30;

	area.removeFromTop(header_height);

	//_mono_stereo_cmb.setBounds(getWidth() - section_width, getHeight() - 60, section_width, 40);

	FlexBox preset_bar;
	FlexBox mono_stereo_section;
	auto footer_area = area.removeFromBottom(footer_height);

	footer_area.removeFromLeft(2 * section_width);

	mono_stereo_section.justifyContent = FlexBox::JustifyContent::center;
	mono_stereo_section.alignItems     = FlexBox::AlignItems::center;
	mono_stereo_section.items.add(FlexItem(_mono_stereo_cmb).withHeight(50).withWidth(section_width));
	mono_stereo_section.performLayout(footer_area.removeFromRight(2 * section_width));

	preset_bar.alignItems = FlexBox::AlignItems::center;
	preset_bar.items.add(FlexItem(_preset_control._left_btn)
		.withWidth(preset_height)
		.withHeight(preset_height)
		.withMargin(FlexItem::Margin(0, 5, 0, 0)));
	preset_bar.items.add(FlexItem(_preset_control._preset_name)
		.withFlex(1)
		.withHeight(preset_height)
		.withMargin(FlexItem::Margin(0, 5, 0, 0)));
	preset_bar.items.add(FlexItem(_preset_control._right_btn)
		.withWidth(preset_height)
		.withHeight(preset_height));
	preset_bar.items.add(FlexItem(_preset_control._save_btn)
		.withFlex(0.25)
		.withHeight(preset_height)
		.withMargin(FlexItem::Margin(0, 0, 0, section_pad)));
	preset_bar.performLayout(footer_area);

	_in_sld.setBounds(area.removeFromLeft(section_width));
	_out_sld.setBounds(area.removeFromRight(section_width));
	_wet_sld.setBounds(area.removeFromRight(section_width));

	area.removeFromLeft(section_width); // space for resonator component labels
	FlexBox resonator_sections[NUM_RESONATORS];
	for (int i = 0; i < NUM_RESONATORS; i++) {
		ResonatorControl& res = _res_controls[i];

		resonator_sections[i].flexWrap = FlexBox::Wrap::wrap;
		resonator_sections[i].flexDirection = FlexBox::Direction::column;
		resonator_sections[i].justifyContent = FlexBox::JustifyContent::spaceBetween;

		resonator_sections[i].items.add(FlexItem(res._noteval_cmb)
			.withWidth(section_width)
			.withHeight(combo_height));
		resonator_sections[i].items.add(FlexItem(res._octave_cmb)
			.withWidth(section_width)
			.withHeight(combo_height));
		resonator_sections[i].items.add(FlexItem(res._detune_sld)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].items.add(FlexItem(res._decay_sld)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].items.add(FlexItem(res._damping_sld)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].items.add(FlexItem(res._volume_sld)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].items.add(FlexItem(res._pluck_btn)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].items.add(FlexItem(res._toggle_btn)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].performLayout(area.removeFromLeft(section_width));

		if (i < NUM_RESONATORS - 1) area.removeFromLeft(section_pad);
	}
}

#if(_DEBUG)
using namespace jcf;
VirtualResonatorsProcessorEditor::DebugPanel::DebugPanel(VirtualResonatorsProcessorEditor& e) :
            parent(e),
            _componentDebugger(&parent)
{
	juce::Path path;
	path.addEllipse(50, 200, 50, 50);

	configShapeButton(_buffer_view, "BufferView", path, Colour(128, 0, 0));
	_buffer_view.onClick = [this] { parent._audioProcessor.toggleBufferDebugger(); };

	configShapeButton(_component_view, "ComponentView", path, Colour(0, 128, 0));
	_component_view.onClick = [this] { toggleComponentDebugger(); };

	configShapeButton(_valueTree_view, "ValTreeView", path, Colour(0, 0, 128));
	_valueTree_view.onClick = [this] { openValueTreeDebugger(); };

	_componentDebugger.setVisible(false);
	_valueTreeDebugger.setVisible(false);
};

void VirtualResonatorsProcessorEditor::DebugPanel::toggleComponentDebugger()
{
	if (_componentDebugger.isVisible()) _componentDebugger.closeButtonPressed();
	else _componentDebugger.setVisible(true);
}

void VirtualResonatorsProcessorEditor::DebugPanel::openValueTreeDebugger()
{
	if (_valueTreeDebugger.isVisible()) {
		_valueTreeDebugger.closeButtonPressed();
	}
	else {
		MemoryBlock stateInfo;
		parent._audioProcessor.getStateInformation(stateInfo);

		String xml_str = String((char*) stateInfo.getData(), stateInfo.getSize());
		ValueTree tree = ValueTree::fromXml(*parseXML(xml_str));

		_valueTreeDebugger.setSource(tree);
		_valueTreeDebugger.setVisible(true);
	}
}

void VirtualResonatorsProcessorEditor::DebugPanel::resized()
{
	FlexBox debug_sidebar;
	auto debug_width = 50;

	debug_sidebar.flexWrap = FlexBox::Wrap::wrap;
	debug_sidebar.flexDirection = FlexBox::Direction::column;

	debug_sidebar.items.add(FlexItem(_buffer_view).withHeight(debug_width).withWidth(debug_width));
	debug_sidebar.items.add(FlexItem(_component_view).withHeight(debug_width).withWidth(debug_width));
	debug_sidebar.items.add(FlexItem(_valueTree_view).withHeight(debug_width).withWidth(debug_width));
	debug_sidebar.performLayout(getLocalBounds());
}
#endif
