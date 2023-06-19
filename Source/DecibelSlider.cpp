/*
  ==============================================================================

    DecibelSlider.cpp
    Created: 17 Dec 2021 10:43:38pm
    Author:  wolfi

  ==============================================================================
*/

#include "DecibelSlider.h"
#include <juce_audio_basics/juce_audio_basics.h>


juce::String DecibelSlider::getTextFromValue(double value)
{
	return Decibels::toString(value);
}

double DecibelSlider::getValueFromText(const juce::String& text)
{
    auto minusInfinitydB = -100.;
    auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();
    return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB
                                                : decibelText.getDoubleValue();
}
