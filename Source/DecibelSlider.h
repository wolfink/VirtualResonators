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
#include "VirtualResonators.h"

class DecibelSlider : public Slider
{
public:
    String getTextFromValue(double value) override;
    double getValueFromText(const String& text) override;
};
