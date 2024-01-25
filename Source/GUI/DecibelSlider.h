/*
  ==============================================================================

    DecibelSlider.h
    Created: 17 Dec 2021 10:43:38pm
    Author:  wolfi

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "../VirtualResonators.h"

class DecibelSlider : public Slider
{
public:
    String getTextFromValue(double value) override
    {
        return Decibels::toString(value);
    }
    double getValueFromText(const String& text) override
    {
        auto minusInfinitydB = -100.;
        auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();
        return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB
                                                    : decibelText.getDoubleValue();
    }
};
