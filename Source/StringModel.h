/*
  ==============================================================================

    StringModel.h
    Created: 15 Jan 2022 4:41:15pm
    Author:  wolfi

  ==============================================================================
*/

#pragma once

#include <vector>
#include <juce_dsp/juce_dsp.h>
#include "VirtualResonators.h"

#define MIN_FREQ 10.0
#define GAMMA    0.2

//==============================================================================
/*
    Physical Modeling of a sympathetic string using Karplus-Strong Algorithm
    y[n] = x[n] + feedback * (x[n-L] + x[n-L-1]) / 2
    with L = sample rate / frequency (delay in samples)
*/
class StringModel
{
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Thiran> _delay_line;
    double _sample_rate;
    int    _num_channels;
    double _decay;
    double _damping;
    double _volume;
    std::vector<float> _averaging_sample;
public:
    StringModel(double sample_rate);
    StringModel(const StringModel& string_model);
    ~StringModel();

    void setFrequency(double frequency);
    void setDecay(double decay);
    void setDamping(double damping);
    void setVolume(double volume);
    void prepare(const juce::dsp::ProcessSpec& process_spec);
    void process(float* samples, int channel, size_t number_of_samples);
    void pluck();
    void clear()
    {
      _delay_line.reset();
    }
    JUCE_LEAK_DETECTOR (StringModel)
};
