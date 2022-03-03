/*
  ==============================================================================

    NoteSlider.h
    Created: 2 Mar 2022 12:07:20am
    Author:  wolfi

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

class NoteSlider  : public juce::Slider
{
public:
	NoteSlider();

    juce::String getTextFromValue(double value) override;
    double getValueFromText(const juce::String& text) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteSlider)
};

