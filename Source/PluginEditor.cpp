/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResonatorProjectAudioProcessorEditor::ResonatorProjectAudioProcessorEditor (ResonatorProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);

    // Set parameters for resonator frequency knobs
    for (int i = 0; i < NUM_RESONATORS; i++) {
		resonatorFrequency[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		resonatorFrequency[i].setRange(20.0, 10000.0, 0.7);
		resonatorFrequency[i].setSkewFactor(.5);
		resonatorFrequency[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
		resonatorFrequency[i].setPopupDisplayEnabled(true, false, this);
		resonatorFrequency[i].setTextValueSuffix(" Hz");
		resonatorFrequency[i].onValueChange = [this, i] { (&audioProcessor)->setFrequency(i, resonatorFrequency[i].getValue()); };
		resonatorFrequency[i].setValue(440.0);
		
		resonatorFrequencyLabel[i].setText("Frequency", juce::NotificationType::dontSendNotification);
		resonatorFrequencyLabel[i].attachToComponent(&resonatorFrequency[i], false);
		addAndMakeVisible(&resonatorFrequency[i]);
    }

    // Set parameters for resonator feedback knobs
    resonatorFeedback.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    resonatorFeedback.setRange(0., 1., 0.01);
    resonatorFeedback.setSkewFactorFromMidPoint(.9);
    resonatorFeedback.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    resonatorFeedback.setPopupDisplayEnabled(true, false, this);
    resonatorFeedback.onValueChange = [this] { (&audioProcessor)->setFeedback(resonatorFeedback.getValue()); };
    resonatorFeedback.setValue(0.99);

    resonatorFeedbackLabel.setText("Feedback", juce::NotificationType::dontSendNotification);
    resonatorFeedbackLabel.attachToComponent(&resonatorFeedback, false);

    // Set parameters for pulse button
    juce::Path path;
    path.addRectangle(50, 200, 50, 50);
    pulseButton.setShape(path, false, false, false);
    pulseButton.onClick = [this] { audioProcessor.addNoise(); };

    // Set parameters for volume knob
    volumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volumeSlider.setRange(0.0, 10.0, 0.01);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    volumeSlider.setPopupDisplayEnabled(true, false, this);
    volumeSlider.onValueChange = [this] { (&audioProcessor)->setVolume(volumeSlider.getValue() / 10.); };
    volumeSlider.setValue(10.0);

    volumeSliderLabel.setText("Volume", juce::NotificationType::dontSendNotification);
    volumeSliderLabel.attachToComponent(&volumeSlider, false);

    // Set parameters for output volume knob
    outputVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputVolumeSlider.setRange(-100.0, 12.0, 0.1);
    outputVolumeSlider.setSkewFactorFromMidPoint(0.0);
    outputVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    outputVolumeSlider.setPopupDisplayEnabled(true, false, this);
    outputVolumeSlider.onValueChange = [this] { (&audioProcessor)->setOutputVolume(juce::Decibels::decibelsToGain(outputVolumeSlider.getValue())); };
    outputVolumeSlider.setValue(0.0);

    outputVolumeLabel.setText("Output", juce::NotificationType::dontSendNotification);
    outputVolumeLabel.attachToComponent(&outputVolumeSlider, false);

    //addAndMakeVisible(&resonatorFeedback);
    //addAndMakeVisible(&volumeSlider);
    addAndMakeVisible(&outputVolumeSlider);
    addAndMakeVisible(&pulseButton);

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
    int knobWidth = getWidth() / NUM_RESONATORS * 0.8;
    int knobPadding = knobWidth * 0.25;
    for (int i = 0; i < NUM_RESONATORS; i++)
        resonatorFrequency[i].setBounds(knobPadding*(i + 1) + knobWidth*i, 50, knobWidth, knobWidth);
    //resonatorFeedback.setBounds(150, 50, 100, 100);
    volumeSlider.setBounds(250, 50, 100, 100);
    outputVolumeSlider.setBounds(250, 150, 100, 100);
    pulseButton.setBounds(50, 200, 50, 50);
}
