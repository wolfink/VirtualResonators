/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResonatorProjectAudioProcessorEditor::ResonatorProjectAudioProcessorEditor (ResonatorProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), _audioProcessor (p)
{

    // Useful variables
	juce::Path path;
	path.addEllipse(50, 200, 50, 50);

#if(_DEBUG)
	_buffer_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(128, 0, 0), 
				juce::Colour(150, 0, 0), 
				juce::Colour(90, 0, 0)
		);
	_component_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(0, 128, 0), 
				juce::Colour(0, 150, 0), 
				juce::Colour(0, 90, 0)
		);
	_valueTree_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(0, 0, 128), 
				juce::Colour(0, 0, 150), 
				juce::Colour(0, 0, 90)
		);
	_fontAndColour_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(128, 0, 128), 
				juce::Colour(150, 0, 150), 
				juce::Colour(90, 0, 90)
		);

	_buffer_view->setShape(path, false, false, false);
	_buffer_view->onClick = [this] { _audioProcessor.toggleBufferDebugger(); };
	addAndMakeVisible(_buffer_view);

	_component_view->setShape(path, false, false, false);
	_component_view->onClick = [this] { toggleComponentDebugger(); };
	addAndMakeVisible(_component_view);

	_valueTree_view->setShape(path, false, false, false);
	_valueTree_view->onClick = [this] { openValueTreeDebugger(); };
	addAndMakeVisible(_valueTree_view);

	_fontAndColour_view->setShape(path, false, false, false);
	_fontAndColour_view->onClick = [this] { toggleFontAndColourDesigner(); };
	addAndMakeVisible(_fontAndColour_view);
#endif

    // Set parameters for resonator components
    for (int i = 0; i < NUM_RESONATORS; i++) {
		ResonatorControl& res = _res_controls[i];

		for (int j = 0; j < notevals.size(); j++)
			res._noteval_cmb.addItem(notevals[j], j + 1);

		res._lbl.setText              (std::to_string(i + 1), NotificationType::dontSendNotification);
		res._lbl.attachToComponent    (&res._noteval_cmb, false);

		for (int j = 0; j < 9; j++)
			res._octave_cmb.addItem(std::to_string(j), j + 1);

		res._detune_sld.setSliderStyle           (Slider::RotaryHorizontalVerticalDrag);
		res._detune_sld.setRange                 (-100.0, 100.0, 1.0);
		res._detune_sld.setPopupDisplayEnabled   (true, false, this);
		res._detune_sld.setTextBoxStyle          (Slider::NoTextBox, false, 90, 0);

		res._decay_sld.setSliderStyle              (Slider::RotaryHorizontalVerticalDrag);
		res._decay_sld.setRange                    (0., 100.0, 0.1);
		res._decay_sld.setTextBoxStyle             (Slider::NoTextBox, false, 90, 0);
		res._decay_sld.setPopupDisplayEnabled      (true, false, this);

		res._damping_sld.setSliderStyle            (Slider::RotaryHorizontalVerticalDrag);
		res._damping_sld.setRange                  (0.0, 100.0, 0.1);
		res._damping_sld.setTextBoxStyle           (Slider::NoTextBox, false, 90, 0);
		res._damping_sld.setPopupDisplayEnabled    (true, false, this);

		res._volume_sld.setSliderStyle             (Slider::RotaryHorizontalVerticalDrag);
		res._volume_sld.setRange                   (0.0, 10.0, 0.01);
		res._volume_sld.setTextBoxStyle            (Slider::NoTextBox, false, 90, 0);
		res._volume_sld.setPopupDisplayEnabled     (true, false, this);

		res._pluck_btn = std::make_unique<ShapeButton>(String("Pluck"), 
			Colour(128, 128, 128), 
			Colour(150, 150, 150), 
			Colour(90 ,  90,  90));
		res._pluck_btn->setShape                      (path, false, false, false);
		res._pluck_btn->onClick = [this, i] { _audioProcessor.pluckResonator(i); };

		res._toggle_btn = std::make_unique<ShapeButton>(String("On/Off"), 
			Colour(30, 0, 0), 
			Colour(30, 0, 0), 
			Colour(30, 0, 0));
		res._toggle_btn->setOnColours(
			Colour(128, 0, 0),
			Colour(128, 0, 0),
			Colour(128, 0, 0)
		);
		res._toggle_btn->shouldUseOnColours         (true);
		res._toggle_btn->setShape                   (path, false, false, false);
		res._toggle_btn->setToggleable              (true);
		res._toggle_btn->setClickingTogglesState    (true);
		res._toggle_btn->setToggleState             (true, NotificationType::dontSendNotification);


		addAndMakeVisible(&res._noteval_cmb);
		addAndMakeVisible(&res._octave_cmb);
		addAndMakeVisible(&res._detune_sld);
		addAndMakeVisible(&res._decay_sld);
		addAndMakeVisible(&res._damping_sld);
		addAndMakeVisible(&res._volume_sld);
		addAndMakeVisible(res._pluck_btn.get());
		addAndMakeVisible(res._pluck_btn.get());
    }

    // Set parameters for volume and mix sliders
    _out_sld.setSliderStyle            (Slider::LinearVertical);
    _out_sld.setRange                  (-100.0, 12.0, 0.1);
    _out_sld.setSkewFactorFromMidPoint (0.0);
    _out_sld.setTextBoxStyle           (Slider::NoTextBox, false, 90, 0);
    _out_sld.setPopupDisplayEnabled    (true, false, this);

    _in_sld.setSliderStyle             (Slider::LinearVertical);
    _in_sld.setRange                   (-100.0, 12.0, 0.1);
    _in_sld.setSkewFactorFromMidPoint  (0.0);
    _in_sld.setTextBoxStyle            (Slider::NoTextBox, false, 90, 0);
    _in_sld.setPopupDisplayEnabled     (true, false, this);

    _wet_sld.setSliderStyle                      (Slider::LinearVertical);
    _wet_sld.setRange                            (0.0, 100.0, 0.1);
    _wet_sld.setTextBoxStyle                     (Slider::NoTextBox, false, 90, 0);
	_wet_sld.setTextValueSuffix                  ("%");
    _wet_sld.setPopupDisplayEnabled              (true, false, this);

	_mono_stereo_cmb.addItem("Mono"  , 1);
	_mono_stereo_cmb.addItem("Stereo", 2);
	_mono_stereo_cmb.setSelectedId(2);
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
		_preset_control._left_btn = std::make_unique<ShapeButton>(String("Left Pre"),
			Colour(128, 128, 128), 
			Colour(150, 150, 150), 
			Colour(90 ,  90,  90));
		_preset_control._left_btn->setShape(preset_btn_shape, false, false, false);
		preset_btn_shape.applyTransform(AffineTransform::rotation(MathConstants<float>::pi));
		_preset_control._right_btn = std::make_unique<ShapeButton>(String("Right Pre"),
			Colour(128, 128, 128), 
			Colour(150, 150, 150), 
			Colour(90 ,  90,  90));
		_preset_control._right_btn->setShape(preset_btn_shape, false, false, false);
		addAndMakeVisible(_preset_control._left_btn.get());
		addAndMakeVisible(_preset_control._right_btn.get());
		addAndMakeVisible(&_preset_control._preset_name);
	}

    addAndMakeVisible(&_out_sld);
    addAndMakeVisible(&_in_sld);
    addAndMakeVisible(&_wet_sld);
	addAndMakeVisible(&_mono_stereo_cmb);

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
			_audioProcessor._parameters,   TOGGLE_ID(i), *res._toggle_btn.get()));
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

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
#if(_DEBUG)
	setSize(1090, 780);
#else
    setSize (1040, 780);
#endif
}

ResonatorProjectAudioProcessorEditor::~ResonatorProjectAudioProcessorEditor()
{
#if(_DEBUG)
	if (_componentDebugger != nullptr)     delete _componentDebugger;
	if (_valueTreeDebugger != nullptr)     delete _valueTreeDebugger;
	if (_fontAndColourDesigner != nullptr) delete _fontAndColourDesigner;
#endif
}

//==============================================================================
void ResonatorProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (25.0f);
    g.drawFittedText("Virtual Resonators", 0, 0, getWidth(), 40, juce::Justification::centred, 1);
}

void ResonatorProjectAudioProcessorEditor::resized()
{
	auto area = getLocalBounds();

#if(_DEBUG)
	FlexBox debug_sidebar;
	auto debug_width = 50;

	debug_sidebar.flexWrap = FlexBox::Wrap::wrap;
	debug_sidebar.flexDirection = FlexBox::Direction::column;

	debug_sidebar.items.add(FlexItem(*_buffer_view).withMaxHeight(debug_width).withMinWidth(debug_width).withFlex(1));
	debug_sidebar.items.add(FlexItem(*_component_view).withMaxHeight(debug_width).withMinWidth(debug_width).withFlex(1));
	debug_sidebar.items.add(FlexItem(*_valueTree_view).withMaxHeight(debug_width).withMinWidth(debug_width).withFlex(1));
	debug_sidebar.items.add(FlexItem(*_fontAndColour_view).withMaxHeight(debug_width).withMinWidth(debug_width).withFlex(1));
	debug_sidebar.performLayout(area.removeFromRight(debug_width));
	//_mono_stereo_cmb.setBounds(getWidth() - section_width - debug_width, getHeight() - 60, section_width, 40);
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
	preset_bar.items.add(FlexItem(*_preset_control._left_btn)
		.withWidth(preset_height)
		.withHeight(preset_height));
	preset_bar.items.add(FlexItem(_preset_control._preset_name)
		.withFlex(1)
		.withHeight(preset_height));
	preset_bar.items.add(FlexItem(*_preset_control._right_btn)
		.withWidth(preset_height)
		.withHeight(preset_height));	
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
		resonator_sections[i].items.add(FlexItem(*res._pluck_btn)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].items.add(FlexItem(*res._toggle_btn)
			.withWidth(section_width)
			.withHeight(section_width));
		resonator_sections[i].performLayout(area.removeFromLeft(section_width));

		if (i < NUM_RESONATORS - 1) area.removeFromLeft(section_pad);
	}
}

#if(_DEBUG)
using namespace jcf;
void ResonatorProjectAudioProcessorEditor::toggleComponentDebugger()
{
    if (_componentDebugger == nullptr)
		_componentDebugger = new ComponentDebugger(this);
    else delete _componentDebugger;
}

void ResonatorProjectAudioProcessorEditor::openValueTreeDebugger()
{
	MemoryBlock stateInfo;
	_audioProcessor.getStateInformation(stateInfo);
	String xml_str = String((char*) stateInfo.getData(), stateInfo.getSize());
	ValueTree tree = ValueTree::fromXml(*parseXML(xml_str));
	if (_valueTreeDebugger != nullptr) delete _valueTreeDebugger;
    _valueTreeDebugger = new jcf::ValueTreeDebugger(tree);
}

void ResonatorProjectAudioProcessorEditor::toggleFontAndColourDesigner()
{
}
#endif
