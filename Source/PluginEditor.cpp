/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


double INITIAL_SCALE[8]    = { 0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0, 0.0 };
double INITIAL_REGISTER[8] = { 4.0, 4.0, 4.0, 4.0, 4.0, 4.0,  4.0, 5.0 };

//==============================================================================
ResonatorProjectAudioProcessorEditor::ResonatorProjectAudioProcessorEditor (ResonatorProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audio_processor (p)
{

    // Useful variables
	juce::Path path;
	path.addEllipse(50, 200, 50, 50);
	//path.addRectangle(50, 200, 50, 50);

#if(_DEBUG)
	buffer_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(128, 0, 0), 
				juce::Colour(150, 0, 0), 
				juce::Colour(90, 0, 0)
		);
	component_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(0, 128, 0), 
				juce::Colour(0, 150, 0), 
				juce::Colour(0, 90, 0)
		);
	valueTree_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(0, 0, 128), 
				juce::Colour(0, 0, 150), 
				juce::Colour(0, 0, 90)
		);
	fontAndColour_view = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(128, 0, 128), 
				juce::Colour(150, 0, 150), 
				juce::Colour(90, 0, 90)
		);

	buffer_view->setShape(path, false, false, false);
	buffer_view->onClick = [this] { audio_processor.toggleBufferDebugger(); };
	addAndMakeVisible(buffer_view);

	component_view->setShape(path, false, false, false);
	component_view->onClick = [this] { toggleComponentDebugger(); };
	addAndMakeVisible(component_view);
	componentDebugger_on = false;

	valueTree_view->setShape(path, false, false, false);
	valueTree_view->onClick = [this] { openValueTreeDebugger(); };
	addAndMakeVisible(valueTree_view);

	fontAndColour_view->setShape(path, false, false, false);
	fontAndColour_view->onClick = [this] { toggleFontAndColourDesigner(); };
	addAndMakeVisible(fontAndColour_view);
	fontAndColourDesigner_on = false;
#endif

	// Attach sliders to audio processer state tree
	for (size_t i = 0; i < NUM_RESONATORS; i++) {
		note_attachments.push_back   (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			audio_processor.parameters,  NOTEVAL_ID(i), resonator_note_values[i]));
		octave_attachments.push_back (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			audio_processor.parameters, REGISTER_ID(i),     resonator_octaves[i]));
		decay_attachments.push_back  (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			audio_processor.parameters,    DECAY_ID(i),      resonator_decays[i]));
		volume_attachments.push_back (std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
			audio_processor.parameters,   VOLUME_ID(i),     resonator_volumes[i]));
	}

	input_attachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		audio_processor.parameters, INPUT_ID , input_volume_slider);
	output_attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		audio_processor.parameters, OUTPUT_ID, output_volume_slider);
	wet_attachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
		audio_processor.parameters, WET_ID   , wet_slider);

    // Set parameters for resonator controls
    for (int i = 0; i < NUM_RESONATORS; i++) {

		resonator_note_values[i].setPopupDisplayEnabled (true, false, this);
		resonator_note_values[i].setTextBoxStyle        (juce::Slider::NoTextBox, false, 90, 0);

		resonator_octaves[i].setSliderStyle             (juce::Slider::RotaryHorizontalVerticalDrag);
		resonator_octaves[i].setRange                   (0.0, 8.0, 1.0);
		resonator_octaves[i].setPopupDisplayEnabled     (true, false, this);
		resonator_octaves[i].setTextBoxStyle            (juce::Slider::NoTextBox, false, 90, 0);
		
		resonator_number_labels[i].setText              (std::to_string(i+1), juce::NotificationType::dontSendNotification);
		resonator_number_labels[i].attachToComponent    (&resonator_note_values[i], false);

		resonator_decays[i].setSliderStyle              (juce::Slider::RotaryHorizontalVerticalDrag);
		resonator_decays[i].setRange                    (0., 100.0, 100.0 / 127.0);
		resonator_decays[i].setTextBoxStyle             (juce::Slider::NoTextBox, false, 90, 0);
		resonator_decays[i].setPopupDisplayEnabled      (true, false, this);

		pluck_buttons[i] = std::make_unique<ShapeButton>(String("Pulse"), 
			Colour(128, 128, 128), 
			Colour(150, 150, 150), 
			Colour(90, 90, 90));
		pluck_buttons[i]->setShape                      (path, false, false, false);
		pluck_buttons[i]->onClick = [this, i] { audio_processor.pluckResonator(i); };

		resonator_volumes[i].setSliderStyle            (juce::Slider::RotaryHorizontalVerticalDrag);
		resonator_volumes[i].setRange                  (0.0, 10.0, 10.0 / 127.0);
		resonator_volumes[i].setTextBoxStyle           (juce::Slider::NoTextBox, false, 90, 0);
		resonator_volumes[i].setPopupDisplayEnabled    (true, false, this);

		addAndMakeVisible(&resonator_note_values[i]);
		addAndMakeVisible(&resonator_octaves[i]);
		addAndMakeVisible(&resonator_decays[i]);
		addAndMakeVisible(&resonator_volumes[i]);
		addAndMakeVisible(pluck_buttons[i].get());
    }

    // Set parameters for output volume knob
    output_volume_slider.setSliderStyle            (juce::Slider::RotaryHorizontalVerticalDrag);
    output_volume_slider.setRange                  (-100.0, 12.0, 0.1);
    output_volume_slider.setSkewFactorFromMidPoint (0.0);
    output_volume_slider.setTextBoxStyle           (juce::Slider::NoTextBox, false, 90, 0);
    output_volume_slider.setPopupDisplayEnabled    (true, false, this);

    input_volume_slider.setSliderStyle             (juce::Slider::RotaryHorizontalVerticalDrag);
    input_volume_slider.setRange                   (-100.0, 12.0, 0.1);
    input_volume_slider.setSkewFactorFromMidPoint  (0.0);
    input_volume_slider.setTextBoxStyle            (juce::Slider::NoTextBox, false, 90, 0);
    input_volume_slider.setPopupDisplayEnabled     (true, false, this);

    wet_slider.setSliderStyle                      (juce::Slider::RotaryHorizontalVerticalDrag);
    wet_slider.setRange                            (0.0, 1.0, 1.0 / 127.0);
    wet_slider.setTextBoxStyle                     (juce::Slider::NoTextBox, false, 90, 0);
    wet_slider.setPopupDisplayEnabled              (true, false, this);

    addAndMakeVisible(&output_volume_slider);
    addAndMakeVisible(&input_volume_slider);
    addAndMakeVisible(&wet_slider);

    // Set parameters for labels
	resonator_note_value_label.setText           ("Note", juce::NotificationType::dontSendNotification);
	resonator_note_value_label.attachToComponent (&resonator_note_values[0], true);

	resonator_octave_label.setText               ("Octave", juce::NotificationType::dontSendNotification);
	resonator_octave_label.attachToComponent     (&resonator_octaves[0], true);

    resonator_decay_label.setText                ("Decay", juce::NotificationType::dontSendNotification);
    resonator_decay_label.attachToComponent      (&resonator_decays[0], true);
    
	volume_slider_label.setText                  ("Volume", juce::NotificationType::dontSendNotification);
	volume_slider_label.attachToComponent        (&resonator_volumes[0], true);

    output_volume_label.setText                  ("Output", juce::NotificationType::dontSendNotification);
    output_volume_label.attachToComponent        (&output_volume_slider, false);

    input_volume_label.setText                   ("Input", juce::NotificationType::dontSendNotification);
    input_volume_label.attachToComponent         (&input_volume_slider, false);

    wet_label.setText                            ("Wet", juce::NotificationType::dontSendNotification);
    wet_label.attachToComponent                  (&wet_slider, false);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
#if(_DEBUG)
	setSize(850, 600);
#else
    setSize (800, 600);
#endif
}

ResonatorProjectAudioProcessorEditor::~ResonatorProjectAudioProcessorEditor()
{
}

//==============================================================================
void ResonatorProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText("Resonators", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void ResonatorProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
#if(_DEBUG)
	buffer_view->setBounds(800, 0, 50, 50);
	component_view->setBounds(800, 50, 50, 50);
	valueTree_view->setBounds(800, 100, 50, 50);
	fontAndColour_view->setBounds(800, 150, 50, 50);
    int knobWidth = (getWidth()-200) / NUM_RESONATORS * 0.8;
    int knobPadding = knobWidth * 0.25;
#else
    int knobWidth = (getWidth()-150) / NUM_RESONATORS * 0.8;
    int knobPadding = knobWidth * 0.25;
#endif
	for (int i = 0; i < NUM_RESONATORS; i++) {
		//resonatorFrequency[i].setBounds(100 + knobPadding*(i + 1) + knobWidth*i, 50, knobWidth, knobWidth);
		resonator_note_values[i].setBounds (100 + knobPadding*(i + 1) + knobWidth*i, 50                              , knobWidth, knobWidth);
		resonator_octaves[i].setBounds    (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 1*(knobWidth + knobPadding), knobWidth, knobWidth);
		resonator_decays[i].setBounds     (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 2*(knobWidth + knobPadding), knobWidth, knobWidth);
		resonator_volumes[i].setBounds       (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 3*(knobWidth + knobPadding), knobWidth, knobWidth);
		pluck_buttons[i]->setBounds      (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 4*(knobWidth + knobPadding), knobWidth, knobWidth);
	}
	input_volume_slider.setBounds  (350, 500, 100, 100);
	wet_slider.setBounds       (500, 500, 100, 100);
    output_volume_slider.setBounds (650, 500, 100, 100);
}

#if(_DEBUG)
void ResonatorProjectAudioProcessorEditor::toggleComponentDebugger()
{
    if (!componentDebugger_on) componentDebugger = new jcf::ComponentDebugger(this);
    else delete componentDebugger;
    componentDebugger_on = !componentDebugger_on;
}

void ResonatorProjectAudioProcessorEditor::openValueTreeDebugger()
{
	juce::MemoryBlock stateInfo(sizeof(juce::ValueTree));
	audio_processor.getStateInformation(stateInfo);
	juce::ValueTree tree = *(juce::ValueTree *) stateInfo.getData();
    valueTreeDebugger = new jcf::ValueTreeDebugger(tree);
}

void ResonatorProjectAudioProcessorEditor::toggleFontAndColourDesigner()
{
	/*
    if (!fontAndColourDesignerOn) fontAndColourDesigner = new jcf::FontAndColourDesigner();
    else delete fontAndColourDesigner;
    fontAndColourDesignerOn = !fontAndColourDesignerOn;
	*/
}
#endif
