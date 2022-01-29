/*
  ==============================================================================

    StringModel.h
    Created: 15 Jan 2022 4:41:15pm
    Author:  wolfi

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Physical Modeling of a sympathetic string using Karplus-Strong Algorithm
    y[n] = x[n] + feedback * (x[n-L] + x[n-L-1]) / 2
    with L = sample rate / frequency (delay in samples)
*/
template <typename SampleType>
class StringModel
{
public:
    StringModel(const juce::dsp::ProcessSpec& processSpec);
    ~StringModel();

    void setFrequency(double frequency);
    void prepare(const juce::dsp::ProcessSpec& processSpec);
    void process(SampleType* samples, int channel, size_t numberOfSamples);
private:
    juce::dsp::DelayLine<SampleType> delayLine;
    juce::dsp::ProcessSpec processSpec;
    //double frequency;
    SampleType averagingSample;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringModel)
};
