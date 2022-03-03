/*
  ==============================================================================

    NoteSlider.cpp
    Created: 2 Mar 2022 12:07:20am
    Author:  wolfi

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NoteSlider.h"

//==============================================================================


NoteSlider::NoteSlider() {
	setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	setRange(0.0, 12.0, 1.0);
}

juce::String NoteSlider::getTextFromValue(double value)
{
	switch ((int)value) {
	case 0:
		return "C";
	case 1:
		return "C#/Db";
	case 2:
		return "D";
	case 3:
		return "D#/Eb";
	case 4:
		return "E";
	case 5:
		return "F";
	case 6:
		return "F#/Gb";
	case 7:
		return "G";
	case 8:
		return "G#/Ab";
	case 9:
		return "A";
	case 10:
		return "A#/Bb";
	case 11:
		return "B";
	default:
		return "";
	};
}

double NoteSlider::getValueFromText(const juce::String& text)
{
	double value = 0.0;
	switch (text.length() > 1) {
	case 1:
		value += 1.0;
	case 0:
		switch (text[0]) {
		case 'C':
			return value;
		case 'D':
			return value + 2.0;
		case 'E':
			return value + 4.0;
		case 'F':
			return value + 5.0;
		case 'G':
			return value + 7.0;
		case 'A':
			return value + 9.0;
		case 'B':
			return value + 11.0;
		}
	}
}
