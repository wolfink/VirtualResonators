/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define SET_AND_ATTACH_LABEL(label, component, text, left) {\
	label.setText           (text, NotificationType::dontSendNotification);\
	label.attachToComponent (component, left);\
}

//==============================================================================
VirtualResonatorsProcessorEditor::VirtualResonatorsProcessorEditor(VirtualResonatorsAudioProcessor& p)
	: VirtualResonatorsComponent(&p), _audioProcessor(p), _debug(*this), _preset_control(*this)
{
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
		_res_controls[i].initialize(this, i);
		addAndMakeVisible(_res_controls[i]);
	}
	addAndMakeVisible(_preset_control);
#if(_DEBUG)
	addAndMakeVisible(_debug);
#endif

	SET_AND_ATTACH_LABEL(_noteval_lbl, &_res_controls[0]._noteval_cmb, "Note"   , true);
	SET_AND_ATTACH_LABEL(_octave_lbl , &_res_controls[0]._octave_cmb , "Octave" , true);
	SET_AND_ATTACH_LABEL(_detune_lbl , &_res_controls[0]._detune_sld , "Detune" , true);
	SET_AND_ATTACH_LABEL(_decay_lbl  , &_res_controls[0]._decay_sld  , "Decay"  , true);
	SET_AND_ATTACH_LABEL(_damping_lbl, &_res_controls[0]._damping_sld, "Damping", true);
	SET_AND_ATTACH_LABEL(_volume_lbl , &_res_controls[0]._volume_sld , "Volume" , true);
	SET_AND_ATTACH_LABEL(_out_lbl, &_out_sld, "Out", false);
	SET_AND_ATTACH_LABEL(_in_lbl , &_in_sld , "In" , false);
	SET_AND_ATTACH_LABEL(_wet_lbl, &_wet_sld, "Mix", false);

#if(_DEBUG)
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
	auto header_height = 80;
	auto footer_height = 80;
	auto section_width = 80;
	auto section_pad   = 10;
	auto combo_height  = 20;

#if(_DEBUG)
	_debug.setBounds(area.removeFromRight(50));
#endif

	area.removeFromTop(header_height);

	//_mono_stereo_cmb.setBounds(getWidth() - section_width, getHeight() - 60, section_width, 40);

	FlexBox mono_stereo_section;
	auto footer_area = area.removeFromBottom(footer_height);

	footer_area.removeFromLeft(2 * section_width);

	mono_stereo_section.justifyContent = FlexBox::JustifyContent::center;
	mono_stereo_section.alignItems     = FlexBox::AlignItems::center;
	mono_stereo_section.items.add(FlexItem(_mono_stereo_cmb).withHeight(50).withWidth(section_width));
	mono_stereo_section.performLayout(footer_area.removeFromRight(2 * section_width));

	_preset_control.setBounds(footer_area);

	_in_sld.setBounds(area.removeFromLeft(section_width));
	_out_sld.setBounds(area.removeFromRight(section_width));
	_wet_sld.setBounds(area.removeFromRight(section_width));

	for (int i = 0; i < NUM_RESONATORS; i++) {
		if (i == 0) _res_controls[i].setBounds(area.removeFromLeft(2 * section_width));
		else        _res_controls[i].setBounds(area.removeFromLeft(section_width));
		if (i < NUM_RESONATORS - 1) area.removeFromLeft(section_pad);
	}
}

VirtualResonatorsProcessorEditor::ResonatorControl::ResonatorControl()
{
	juce::Path path;
	path.addEllipse(50, 200, 50, 50);

	configComboBox(_noteval_cmb, notevals);
	configComboBox(_octave_cmb, { "0", "1", "2", "3", "4", "5", "6", "7", "8" });
	configRotarySlider(_detune_sld, -100.0, 100.0, 1.0);
	configRotarySlider(_decay_sld, 0.0, 100.0, 0.1);
	configRotarySlider(_damping_sld, 0.0, 100.0, 0.1);
	configRotarySlider(_volume_sld, 0.0, 10.0, 0.01);
	configShapeButton(_pluck_btn , "Pluck" , path, Colour(128, 128, 128));
	configShapeButton(_toggle_btn, "On/Off", path, Colour(30, 0, 0), false);
	_toggle_btn.setOnColours(
		Colour(128, 0, 0),
		Colour(128, 0, 0),
		Colour(128, 0, 0)
	);
	_toggle_btn.shouldUseOnColours     (true);
	_toggle_btn.setShape               (path, false, false, false);
	_toggle_btn.setToggleable          (true);
	_toggle_btn.setClickingTogglesState(true);
	_toggle_btn.setToggleState         (true, NotificationType::dontSendNotification);
}

void VirtualResonatorsProcessorEditor::ResonatorControl::initialize(VirtualResonatorsProcessorEditor* e, int index)
{
	_parent = e;
	_lbl.setText          (std::to_string(index + 1), NotificationType::dontSendNotification);
	_lbl.attachToComponent(&_noteval_cmb, false);

	_pluck_btn.onClick = [this, index] { _parent->_audioProcessor.pluckResonator(index); };

	// Attach GUI controls to parameters
	_noteval_attachments = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
		_parent->_audioProcessor._parameters,  NOTEVAL_ID(index), _noteval_cmb);
	_octave_attachments = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
		_parent->_audioProcessor._parameters, OCTAVE_ID(index), _octave_cmb);
	_detune_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent->_audioProcessor._parameters,   DETUNE_ID(index), _detune_sld);
	_decay_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent->_audioProcessor._parameters,    DECAY_ID(index), _decay_sld);
	_damping_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent->_audioProcessor._parameters,  DAMPING_ID(index), _damping_sld);
	_volume_attachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		_parent->_audioProcessor._parameters,   VOLUME_ID(index), _volume_sld);
	_toggle_attachments = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(
		_parent->_audioProcessor._parameters,   TOGGLE_ID(index), _toggle_btn);
}

void VirtualResonatorsProcessorEditor::ResonatorControl::resized()
{
	auto diameter     = 80;
	auto combo_height = 20;

	FlexBox resonator_flex;

	resonator_flex.flexWrap       = FlexBox::Wrap::wrap;
	resonator_flex.flexDirection  = FlexBox::Direction::column;
	resonator_flex.justifyContent = FlexBox::JustifyContent::spaceBetween;
	resonator_flex.alignItems     = FlexBox::AlignItems::flexEnd;

	resonator_flex.items.add(FlexItem(_noteval_cmb)
		.withWidth(diameter)
		.withHeight(combo_height));
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

VirtualResonatorsProcessorEditor::PresetControl::PresetControl(VirtualResonatorsProcessorEditor& e) :
	_parent(e)
{
	_directory = File::getSpecialLocation(File::SpecialLocationType::currentExecutableFile)
		.getParentDirectory()
		.getParentDirectory()
		.getChildFile("Resources")
		.getChildFile("Presets");

	Path preset_btn_shape;
	preset_btn_shape.addTriangle(0, 10, 20, 20, 20, 0);

	configShapeButton(_left_btn , "LeftPre" , preset_btn_shape, Colour(128, 128, 128));
	_left_btn.onClick = [this] { prev_preset(); };

	preset_btn_shape.applyTransform(AffineTransform::rotation(MathConstants<float>::pi));
	configShapeButton(_right_btn, "RightPre", preset_btn_shape, Colour(128, 128, 128));
	_right_btn.onClick = [this] { next_preset(); };

	configComboBox(_preset_name, get_preset_list());
	_preset_name.setEditableText(true);
	_preset_name.onChange = [this] { load_preset(); };

	configTextButton(_save_btn, "Save");
	_save_btn.onClick = [this] { save_preset(); };
}

void VirtualResonatorsProcessorEditor::PresetControl::resized()
{
	FlexBox preset_bar;
	auto preset_height = 30;

	preset_bar.alignItems = FlexBox::AlignItems::center;
	preset_bar.items.add(FlexItem(_left_btn)
		.withWidth(preset_height)
		.withHeight(preset_height)
		.withMargin(FlexItem::Margin(0, 5, 0, 0)));
	preset_bar.items.add(FlexItem(_preset_name)
		.withFlex(1)
		.withHeight(preset_height)
		.withMargin(FlexItem::Margin(0, 5, 0, 0)));
	preset_bar.items.add(FlexItem(_right_btn)
		.withWidth(preset_height)
		.withHeight(preset_height));
	preset_bar.items.add(FlexItem(_save_btn)
		.withFlex(0.25)
		.withHeight(preset_height)
		.withMargin(FlexItem::Margin(0, 0, 0, 10)));
	preset_bar.performLayout(getLocalBounds());
}

void VirtualResonatorsProcessorEditor::PresetControl::prev_preset()
{
	int prev_id = _preset_name.getSelectedId() - 1;
	prev_id = (prev_id < 1) ? _preset_name.getNumItems() : prev_id;
	_preset_name.setSelectedId(prev_id);
}

void VirtualResonatorsProcessorEditor::PresetControl::next_preset()
{
	int next_id = _preset_name.getSelectedId() + 1;
	next_id = (next_id > _preset_name.getNumItems()) ? 1 : next_id;
	_preset_name.setSelectedId(next_id);
}

void VirtualResonatorsProcessorEditor::PresetControl::load_preset()
{
	File file = File(_directory.getFullPathName() + "\\" + _preset_name.getText() + ".vrpst");
	if (file.exists()) {
		String preset_data = file.loadFileAsString();
		_parent._audioProcessor.setStateInformation((void*) preset_data.toRawUTF8(), preset_data.length());
	}
}

void VirtualResonatorsProcessorEditor::PresetControl::save_preset()
{
	MemoryBlock state_info;
	_parent._audioProcessor.getStateInformation(state_info);
	String preset_data = String((char*)state_info.getData(), state_info.getSize());
	String file_name = _directory.getFullPathName() + "\\" + _preset_name.getText() + ".vrpst";
	File file(file_name);
	file.replaceWithText(preset_data);
	_preset_name.clear(dontSendNotification);
	_preset_name.addItemList(get_preset_list(), 1);
}

StringArray VirtualResonatorsProcessorEditor::PresetControl::get_preset_list()
{
	Array<File> files;
	StringArray file_names;
	_directory.findChildFiles(files, File::TypesOfFileToFind::findFiles, true, "*.vrpst");
	for (File f : files) file_names.add(f.getFileNameWithoutExtension());
	return file_names;
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
