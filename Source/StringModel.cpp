/*
  ==============================================================================

    StringModel.cpp
    Created: 15 Jan 2022 4:41:15pm
    Author:  wolfi

  ==============================================================================
*/

#include "StringModel.h"

//==============================================================================
StringModel::StringModel(double sampleRate) : 
    _sample_rate(sampleRate),
    _averaging_sample(0.0),
    _damping(0.99999),
    _decay(0.5),
    _volume(1)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / MIN_FREQ);
}

StringModel::StringModel(const StringModel& string_model):
_sample_rate(string_model._sample_rate),
_averaging_sample(string_model._averaging_sample),
_damping(string_model._damping),
_decay(string_model._decay),
_volume(string_model._volume)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / MIN_FREQ);
}

StringModel::~StringModel()
{
}

void StringModel::setFrequency(double frequency)
{
    _delay_line.setDelay(_sample_rate / frequency);
}

void StringModel::setDecay(double decay)
{
    _decay = decay;
}

void StringModel::setDamping(double damping)
{
    jassert(damping >= 0.0 && damping < 1.0);
    _damping = damping;
}

void StringModel::setVolume(double volume)
{
    _volume = volume;
}

void StringModel::prepare(const juce::dsp::ProcessSpec& process_spec)
{
    double frequency = _sample_rate / _delay_line.getDelay();
    _delay_line.setMaximumDelayInSamples(process_spec.sampleRate / MIN_FREQ);
    _delay_line.setDelay(process_spec.sampleRate / frequency);
    _delay_line.prepare(process_spec);
    _sample_rate = process_spec.sampleRate;
    _averaging_sample.resize(process_spec.numChannels);
}

void StringModel::pluck(int channel)
{
    size_t num_samples = _delay_line.getDelay();
	juce::Random random;
	random.setSeedRandomly();
    clear();

	for (int j = 0; j < num_samples; j++) {
		_delay_line.pushSample(channel, random.nextFloat() * 2.0 - 1.0);
		_delay_line.popSample(channel);
	}

	_delay_line.popSample(channel);
}

void StringModel::process(float* samples, int channel, size_t numberOfSamples)
{
    for (int i = 0; i < numberOfSamples; i++)
    {
        float delayedSample = _delay_line.popSample(channel);
        samples[i] += _damping * (_decay * _averaging_sample[channel] + (1 - _decay) * delayedSample);
        _averaging_sample[channel] = delayedSample;
        _delay_line.pushSample(channel, samples[i]);
        samples[i] *= _volume * GAMMA;
    }
}
