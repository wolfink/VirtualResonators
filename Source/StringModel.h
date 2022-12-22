/*
  ==============================================================================

    StringModel.h
    Created: 15 Jan 2022 4:41:15pm
    Author:  wolfi

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VirtualResonators.h"

#define STRING_MAX_DUR 20.0
#define GAMMA          0.2

//==============================================================================
/*
    Physical Modeling of a sympathetic string using Karplus-Strong Algorithm
    y[n] = x[n] + feedback * (x[n-L] + x[n-L-1]) / 2
    with L = sample rate / frequency (delay in samples)
*/
class StringModel
{
public:
    StringModel(double sample_rate);
    StringModel(const StringModel& string_model);
    //StringModel(const juce::dsp::ProcessSpec& processSpec);
    ~StringModel();

    void setFrequency(double frequency);
    void setDecay(double decay);
    void setVolume(double volume);
    void prepare(const juce::dsp::ProcessSpec& process_spec);
    void process(float* samples, int channel, size_t number_of_samples);
    void pluck(int channel);
    void clear()
    {
      _delay_line.reset();
    }
private:
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Thiran> _delay_line;
    //juce::dsp::ProcessSpec processSpec;
    double _sample_rate;
    double _decay;
    double _damping;
    double _volume;
    //double frequency;
    std::vector<float> _averaging_sample;
    JUCE_LEAK_DETECTOR (StringModel)
};
