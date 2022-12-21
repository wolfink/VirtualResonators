/*
  ==============================================================================

    StringModel.cpp
    Created: 15 Jan 2022 4:41:15pm
    Author:  wolfi

  ==============================================================================
*/

//#include <JuceHeader.h>
#include "StringModel.h"

//==============================================================================
template<typename SampleType>
StringModel<SampleType>::StringModel(double sampleRate) : _sample_rate(sampleRate), _averaging_sample(0.0), _damping(0.99999)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / 20.0);
}

template StringModel<float>::StringModel(double sampleRate);
template StringModel<double>::StringModel(double sampleRate);

template<typename SampleType>
StringModel<SampleType>::StringModel(const StringModel<SampleType>& string_model):
_sample_rate(string_model._sample_rate), _averaging_sample(string_model._averaging_sample)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / 20.0);
}

template StringModel<float>::StringModel(const StringModel<float>& string_model);
template StringModel<double>::StringModel(const StringModel<double>& string_model);

template<typename SampleType>
StringModel<SampleType>::~StringModel()
{
}

template StringModel<float>::~StringModel();
template StringModel<double>::~StringModel();

template<typename SampleType>
void StringModel<SampleType>::setFrequency(double frequency)
{
    _delay_line.setDelay(_sample_rate / frequency);
    _damping = std::pow(10, -5 / frequency / STRING_MAX_DUR);
}

template void StringModel<float>::setFrequency(double frequency);
template void StringModel<double>::setFrequency(double frequency);

template<typename SampleType>
void StringModel<SampleType>::setDecay(double decay)
{
    _decay = decay;
}

template void StringModel<float>::setDecay(double frequency);
template void StringModel<double>::setDecay(double frequency);

template<typename SampleType>
void StringModel<SampleType>::setVolume(double volume)
{
    _volume = volume;
}

template void StringModel<float>::setVolume(double volume);
template void StringModel<double>::setVolume(double volume);

template<typename SampleType>
void StringModel<SampleType>::prepare(const juce::dsp::ProcessSpec& process_spec)
{
    double frequency = _sample_rate / _delay_line.getDelay();
    _delay_line.setMaximumDelayInSamples(process_spec.sampleRate / 20.0);
    _delay_line.setDelay(process_spec.sampleRate / frequency);
    _delay_line.prepare(process_spec);
    _sample_rate = process_spec.sampleRate;
    _averaging_sample.resize(process_spec.numChannels);
}

template void StringModel<float>::prepare(const juce::dsp::ProcessSpec& ps);
template void StringModel<double>::prepare(const juce::dsp::ProcessSpec& ps);

template <typename SampleType>
void StringModel<SampleType>::pluck(int channel)
{
    size_t num_samples = _delay_line.getDelay();
    clear();
    for (int i = 0; i < NUM_IN_CHAN; i++) {
		juce::Random random;
		random.setSeedRandomly();
        for (int j = 0; j < num_samples; j++) {
			_delay_line.pushSample(channel, random.nextFloat() * 2.0 - 1.0);
            _delay_line.popSample(channel);
        }
    }
	_delay_line.popSample(channel);
}

template void StringModel<float>::pluck(int channel);
template void StringModel<double>::pluck(int channel);


template<typename SampleType>
void StringModel<SampleType>::process(SampleType* samples, int channel, size_t numberOfSamples)
{
    for (int i = 0; i < numberOfSamples; i++)
    {
        SampleType delayedSample = _delay_line.popSample(channel);
        samples[i] += _damping * (_decay * _averaging_sample[channel] + (1 - _decay) * delayedSample);
        _averaging_sample[channel] = delayedSample;
        _delay_line.pushSample(channel, samples[i]);
        samples[i] *= _volume;
    }
}

template void StringModel<float>::process(float* input, int channel, size_t numberOfSamples);
template void StringModel<double>::process(double* input, int channel, size_t numberOfSamples);
