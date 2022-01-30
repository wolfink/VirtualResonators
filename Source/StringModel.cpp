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
StringModel<SampleType>::StringModel(double sampleRate) : _sampleRate(sampleRate), averagingSample(0.0)
{
    delayLine.setMaximumDelayInSamples(_sampleRate / 20.0);
}

template StringModel<float>::StringModel(double sampleRate);
template StringModel<double>::StringModel(double sampleRate);

template<typename SampleType>
StringModel<SampleType>::StringModel(const StringModel<SampleType>& stringModel):
_sampleRate(stringModel._sampleRate), averagingSample(stringModel.averagingSample)
{
    delayLine.setMaximumDelayInSamples(_sampleRate / 20.0);
}

template StringModel<float>::StringModel(const StringModel<float>& stringModel);
template StringModel<double>::StringModel(const StringModel<double>& stringModel);

template<typename SampleType>
StringModel<SampleType>::~StringModel()
{
}

template StringModel<float>::~StringModel();
template StringModel<double>::~StringModel();

template<typename SampleType>
void StringModel<SampleType>::setFrequency(double frequency)
{
    delayLine.setDelay(_sampleRate / frequency);
}

template void StringModel<float>::setFrequency(double frequency);
template void StringModel<double>::setFrequency(double frequency);

template<typename SampleType>
void StringModel<SampleType>::prepare(const juce::dsp::ProcessSpec& processSpec)
{
    double frequency = _sampleRate / delayLine.getDelay();
    delayLine.setMaximumDelayInSamples(processSpec.sampleRate / 20.0);
    delayLine.setDelay(processSpec.sampleRate / frequency);
    delayLine.prepare(processSpec);
    _sampleRate = processSpec.sampleRate;
}

template void StringModel<float>::prepare(const juce::dsp::ProcessSpec& ps);
template void StringModel<double>::prepare(const juce::dsp::ProcessSpec& ps);


template<typename SampleType>
void StringModel<SampleType>::process(SampleType* samples, int channel, size_t numberOfSamples)
{
    for (int i = 0; i < numberOfSamples; i++)
    {
        SampleType delayedSample = delayLine.popSample(0);
        samples[i] = samples[i] + (averagingSample + delayedSample) / 2;
        averagingSample = delayedSample;
        delayLine.pushSample(channel, samples[i]);
    }
}

template void StringModel<float>::process(float* input, int channel, size_t numberOfSamples);
template void StringModel<double>::process(double* input, int channel, size_t numberOfSamples);
