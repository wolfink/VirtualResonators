/*
  ==============================================================================

    StringModel.cpp
    Created: 15 Jan 2022 4:41:15pm
    Author:  wolfi

  ==============================================================================
*/

#include "Resonator.h"

//==============================================================================
Resonator::Resonator(double sample_rate) : 
    _ks(sample_rate),
    _wg(sample_rate),
    _sample_rate(sample_rate),
    _volume(1)
{
}

Resonator::Resonator(const Resonator& string_model):
    _ks(string_model._ks),
    _wg(string_model._wg),
    _sample_rate(string_model._sample_rate),
    _volume(string_model._volume)
{
}

Resonator::~Resonator()
{
}

void Resonator::setModel(ResonatorModel model)
{
    _model = model;
}

void Resonator::setParameters(const KarplusStrongParameters& params)
{
    _ks.setParameters(params);
}

void Resonator::setParameters(const WaveguidesParameters& params)
{
    _wg.setParameters(params);
}

void Resonator::setVolume(double volume)
{
    _volume = volume;
}

void Resonator::prepare(const juce::dsp::ProcessSpec& process_spec)
{
    switch(_model)
    {
        case KarplusStrong:
            _ks.prepare(process_spec);
            break;
        case Waveguides:
            _wg.prepare(process_spec);
    }
}

void Resonator::process(float* samples, int channel, size_t number_of_samples)
{
    switch(_model)
    {
        case KarplusStrong:
            _ks.process(samples, channel, number_of_samples);
            break;
        case Waveguides:
            _wg.process(samples, channel, number_of_samples);
            break;
    }
    for (int sample = 0; sample < number_of_samples; sample++) samples[sample] *= _volume * GAMMA;
#if(_DEBUG)
    DebugComponent::getInstance()->setNumberBox("first sample", samples[0]);
    DebugComponent::getInstance()->setNumberBox("second sample", samples[1]);
    DebugComponent::getInstance()->audioVisualiserComponent.pushBuffer(&samples, 1, number_of_samples);
#endif
}

void Resonator::pluck()
{
    switch(_model)
    {
        case KarplusStrong:
            _ks.pluck();
            break;
        case Waveguides:
            _wg.pluck();
    }
}

void Resonator::clear()
{
    switch(_model)
    {
        case KarplusStrong:
            _ks.clear();
            break;
        case Waveguides:
            _wg.clear();
    }
}
