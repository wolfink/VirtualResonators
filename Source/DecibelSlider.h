/*
  ==============================================================================

    DecibelSlider.h
    Created: 17 Dec 2021 10:43:38pm
    Author:  wolfi

  ==============================================================================
*/

#pragma once

#include<JuceHeader.h>

class DecibelSlider : public juce::Slider
{
public:
    juce::String getTextFromValue(double value) override;
    double getValueFromText(const juce::String& text) override;
};
