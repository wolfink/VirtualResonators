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
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    // Useful variables
	juce::Path path;
	path.addEllipse(50, 200, 50, 50);
	//path.addRectangle(50, 200, 50, 50);

#if(_DEBUG)
	bufferView = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(128, 0, 0), 
				juce::Colour(150, 0, 0), 
				juce::Colour(90, 0, 0)
		);
	componentView = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(0, 128, 0), 
				juce::Colour(0, 150, 0), 
				juce::Colour(0, 90, 0)
		);
	valueTreeView = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(0, 0, 128), 
				juce::Colour(0, 0, 150), 
				juce::Colour(0, 0, 90)
		);
	fontAndColourView = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(128, 0, 128), 
				juce::Colour(150, 0, 150), 
				juce::Colour(90, 0, 90)
		);

	bufferView->setShape(path, false, false, false);
	bufferView->onClick = [this] { audioProcessor.toggleBufferDebugger(); };
	addAndMakeVisible(bufferView);

	componentView->setShape(path, false, false, false);
	componentView->onClick = [this] { toggleComponentDebugger(); };
	addAndMakeVisible(componentView);
	componentDebuggerOn = false;

	valueTreeView->setShape(path, false, false, false);
	valueTreeView->onClick = [this] { openValueTreeDebugger(); };
	addAndMakeVisible(valueTreeView);

	fontAndColourView->setShape(path, false, false, false);
	fontAndColourView->onClick = [this] { toggleFontAndColourDesigner(); };
	addAndMakeVisible(fontAndColourView);
	fontAndColourDesignerOn = false;
#endif

    // Set parameters for resonator controls
    for (int i = 0; i < NUM_RESONATORS; i++) {
		/*
		resonatorFrequency[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		resonatorFrequency[i].setRange(20.0, 10000.0, 0.7);
		resonatorFrequency[i].setSkewFactor(.5);
		resonatorFrequency[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		resonatorFrequency[i].setPopupDisplayEnabled(true, false, this);
		resonatorFrequency[i].setTextValueSuffix(" Hz");
		resonatorFrequency[i].onValueChange = [this, i] { setParameter(Parameter:: Frequency, juce::var(resonatorFrequency[i].getValue()), i); };
		resonatorFrequency[i].setValue(440.0);
		*/

		resonatorNoteValue[i].setPopupDisplayEnabled(true, false, this);
		resonatorNoteValue[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		resonatorNoteValue[i].onValueChange = [this, i] {
			resonatorBaseFrequency[i] = 16.35 * pow(pow(2.0, resonatorNoteValue[i].getValue()), 1.0 / 12.0);
			setParameter(Parameter::Frequency, juce::var(resonatorBaseFrequency[i] * resonatorMultiplier[i]), i);
		};
		resonatorNoteValue[i].setValue(INITIAL_SCALE[i]);
		resonatorNoteValue[i].onValueChange();

		resonatorOctave[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		resonatorOctave[i].setRange(0.0, 8.0, 1.0);
		resonatorOctave[i].setPopupDisplayEnabled(true, false, this);
		resonatorOctave[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		resonatorOctave[i].onValueChange = [this, i] {
			resonatorMultiplier[i] = pow(2.0, resonatorOctave[i].getValue());
			setParameter(Parameter::Frequency, juce::var(resonatorBaseFrequency[i] * resonatorMultiplier[i]), i);
		};
		resonatorOctave[i].setValue(INITIAL_REGISTER[i]);
		resonatorOctave[i].onValueChange();
		
		resonatorNumberLabel[i].setText(std::to_string(i+1), juce::NotificationType::dontSendNotification);
		resonatorNumberLabel[i].attachToComponent(&resonatorNoteValue[i], false);

		resonatorDecay[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		resonatorDecay[i].setRange(0., 100., 0.787);
		resonatorDecay[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		resonatorDecay[i].setPopupDisplayEnabled(true, false, this);
		resonatorDecay[i].onValueChange = [this, i] { setParameter(Parameter::DecayTime, juce::var(0.5 - std::pow(resonatorDecay[i].getValue() / 100.0, 2.0) / 2.0), i); };
		resonatorDecay[i].setValue(50.0);

		pulseButtons[i] = new juce::ShapeButton(
				juce::String("Pulse"), 
				juce::Colour(128, 128, 128), 
				juce::Colour(150, 150, 150), 
				juce::Colour(90, 90, 90)
		);
		pulseButtons[i]->setShape(path, false, false, false);
		pulseButtons[i]->onClick = [this, i] { setParameter(Parameter::AddNoise, juce::var(true), i); };

		volumeSlider[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		volumeSlider[i].setRange(0.0, 10.0, 0.01);
		volumeSlider[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		volumeSlider[i].setPopupDisplayEnabled(true, false, this);
		volumeSlider[i].onValueChange = [this, i] { setParameter(Parameter::ResonatorVolume, juce::var(volumeSlider[i].getValue() / 10.0), i); };
		volumeSlider[i].setValue(10.0);

		//addAndMakeVisible(&resonatorFrequency[i]);
		addAndMakeVisible(&resonatorNoteValue[i]);
		addAndMakeVisible(&resonatorOctave[i]);
		addAndMakeVisible(&resonatorDecay[i]);
		addAndMakeVisible(&volumeSlider[i]);
		addAndMakeVisible(pulseButtons[i]);
    }

    // Set parameters for output volume knob
    outputVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputVolumeSlider.setRange(-100.0, 12.0, 0.1);
    outputVolumeSlider.setSkewFactorFromMidPoint(0.0);
    outputVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    outputVolumeSlider.setPopupDisplayEnabled(true, false, this);
    outputVolumeSlider.onValueChange = [this] { setParameter(Parameter::OutputVolume, juce::var(juce::Decibels::decibelsToGain(outputVolumeSlider.getValue()))); };
    outputVolumeSlider.setValue(0.0);

    inputVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputVolumeSlider.setRange(-100.0, 12.0, 0.1);
    inputVolumeSlider.setSkewFactorFromMidPoint(0.0);
    inputVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    inputVolumeSlider.setPopupDisplayEnabled(true, false, this);
    inputVolumeSlider.onValueChange = [this] { setParameter(Parameter::InputVolume, juce::var(juce::Decibels::decibelsToGain(inputVolumeSlider.getValue()))); };
    inputVolumeSlider.setValue(0.0);

    wetDrySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    wetDrySlider.setRange(0.0, 1.0, 1.0 / 127.0);
    wetDrySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    wetDrySlider.setPopupDisplayEnabled(true, false, this);
    wetDrySlider.onValueChange = [this] { setParameter(Parameter::WetDry, juce::var(wetDrySlider.getValue())); };
    wetDrySlider.setValue(0.5);

    addAndMakeVisible(&outputVolumeSlider);
    addAndMakeVisible(&inputVolumeSlider);
    addAndMakeVisible(&wetDrySlider);

    // Set parameters for labels
	resonatorFrequencyLabel.setText("Frequency", juce::NotificationType::dontSendNotification);
	resonatorFrequencyLabel.attachToComponent(&resonatorFrequency[0], true);
	
	resonatorNoteValueLabel.setText("Note", juce::NotificationType::dontSendNotification);
	resonatorNoteValueLabel.attachToComponent(&resonatorNoteValue[0], true);

	resonatorOctaveLabel.setText("Octave", juce::NotificationType::dontSendNotification);
	resonatorOctaveLabel.attachToComponent(&resonatorOctave[0], true);

    resonatorDecayLabel.setText("Decay", juce::NotificationType::dontSendNotification);
    resonatorDecayLabel.attachToComponent(&resonatorDecay[0], true);
    
	volumeSliderLabel.setText("Volume", juce::NotificationType::dontSendNotification);
	volumeSliderLabel.attachToComponent(&volumeSlider[0], true);

    outputVolumeLabel.setText("Output", juce::NotificationType::dontSendNotification);
    outputVolumeLabel.attachToComponent(&outputVolumeSlider, false);

    inputVolumeLabel.setText("Input", juce::NotificationType::dontSendNotification);
    inputVolumeLabel.attachToComponent(&inputVolumeSlider, false);

    wetDryLabel.setText("Wet", juce::NotificationType::dontSendNotification);
    wetDryLabel.attachToComponent(&wetDrySlider, false);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
#if(_DEBUG)
	setSize(850, 600);
#endif
#if(_RELEASE)
    setSize (800, 600);
#endif
}

ResonatorProjectAudioProcessorEditor::~ResonatorProjectAudioProcessorEditor()
{
	for (int i = 0; i < NUM_RESONATORS; i++) delete pulseButtons[i];
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
	bufferView->setBounds(800, 0, 50, 50);
	componentView->setBounds(800, 50, 50, 50);
	valueTreeView->setBounds(800, 100, 50, 50);
	fontAndColourView->setBounds(800, 150, 50, 50);
    int knobWidth = (getWidth()-150) / NUM_RESONATORS * 0.8;
    int knobPadding = knobWidth * 0.25;
#endif
#if(_RELEASE)
    int knobWidth = (getWidth()-100) / NUM_RESONATORS * 0.8;
    int knobPadding = knobWidth * 0.25;
#endif
	for (int i = 0; i < NUM_RESONATORS; i++) {
        /*
		resonatorFrequency[i].setBounds(100 + knobPadding*(i + 1) + knobWidth*i, 50, knobWidth, knobWidth);
		*/
		resonatorNoteValue[i].setBounds (100 + knobPadding*(i + 1) + knobWidth*i, 50                              , knobWidth, knobWidth);
		resonatorOctave[i].setBounds    (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 1*(knobWidth + knobPadding), knobWidth, knobWidth);
		resonatorDecay[i].setBounds     (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 2*(knobWidth + knobPadding), knobWidth, knobWidth);
		volumeSlider[i].setBounds       (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 3*(knobWidth + knobPadding), knobWidth, knobWidth);
		pulseButtons[i]->setBounds      (100 + knobPadding*(i + 1) + knobWidth*i, 50 + 4*(knobWidth + knobPadding), knobWidth, knobWidth);
	}
	inputVolumeSlider.setBounds  (350, 500, 100, 100);
	wetDrySlider.setBounds       (500, 500, 100, 100);
    outputVolumeSlider.setBounds (650, 500, 100, 100);
}

void ResonatorProjectAudioProcessorEditor::setParameter(Parameter parameter, juce::var& value, int index)
{
	juce::MemoryBlock memory(sizeof(juce::ValueTree));
	audioProcessor.getStateInformation(memory);
	juce::ValueTree* tree = (juce::ValueTree*) memory.getData();
	switch (parameter) {
	case Parameter::Frequency:
		tree->getChild(index).setProperty("frequency", value, nullptr);
		break;
	case Parameter::DecayTime:
		tree->getChild(index).setProperty("decay time", value, nullptr);
		break;
	case Parameter::ResonatorVolume:
		tree->getChild(index).setProperty("volume", value, nullptr);
		break;
	case Parameter::AddNoise:
		tree->getChild(index).setProperty("add noise", value, nullptr);
		break;
	case Parameter::BufferToggle:
		tree->getChildWithName("buffer debugger").setProperty("on", value, nullptr);
		break;
	case Parameter::OutputVolume:
		tree->setProperty("output", value, nullptr);
		break;
	case Parameter::InputVolume:
		tree->setProperty("input", value, nullptr);
		break;
	case Parameter::WetDry:
		tree->setProperty("wet", value, nullptr);
	}
	audioProcessor.setStateInformation((void*) tree, sizeof(juce::ValueTree));
}

#if(_DEBUG)
void ResonatorProjectAudioProcessorEditor::toggleComponentDebugger()
{
    if (!componentDebuggerOn) componentDebugger = new jcf::ComponentDebugger(this);
    else delete componentDebugger;
    componentDebuggerOn = !componentDebuggerOn;
}

void ResonatorProjectAudioProcessorEditor::openValueTreeDebugger()
{
	juce::MemoryBlock stateInfo(sizeof(juce::ValueTree));
	audioProcessor.getStateInformation(stateInfo);
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
