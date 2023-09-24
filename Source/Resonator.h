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
#include "ResonatorModels.h"

#if(_DEBUG)
    #include "Debug/DebugPanel.h"
#endif

#define GAMMA    0.2

//==============================================================================
/*
    Physical Modeling of a sympathetic string using Karplus-Strong Algorithm
    y[n] = x[n] + feedback * (x[n-L] + x[n-L-1]) / 2
    with L = sample rate / frequency (delay in samples)
*/
enum ResonatorModel
{
    KarplusStrong,
    Waveguides
};

class Resonator
{
    double _sample_rate;
    int    _num_channels;
    double _volume;
    ResonatorModel _model;

    KarplusStrongModel _ks;
    WaveguidesModel _wg;

public:
    Resonator(double sample_rate);
    Resonator(const Resonator& string_model);
    ~Resonator();

    void clear();
    void pluck();
    void prepare(const juce::dsp::ProcessSpec& process_spec);
    void process(float* samples, int channel, size_t number_of_samples);
    void setModel(ResonatorModel);
    void setParameters(const KarplusStrongParameters&);
    void setParameters(const WaveguidesParameters&);
    void setVolume(double volume);
private:
    JUCE_LEAK_DETECTOR (Resonator)
};
