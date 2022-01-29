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
StringModel<SampleType>::StringModel(const juce::dsp::ProcessSpec& ps) : averagingSample(0.0)
{
    processSpec = ps;
    delayLine.setMaximumDelayInSamples(processSpec.sampleRate / 20.0);
}

template StringModel<float>::StringModel(const juce::dsp::ProcessSpec& processSpec);
template StringModel<double>::StringModel(const juce::dsp::ProcessSpec& processSpec);

template<typename SampleType>
StringModel<SampleType>::~StringModel()
{
}

template StringModel<float>::~StringModel();
template StringModel<double>::~StringModel();

template<typename SampleType>
void StringModel<SampleType>::setFrequency(double frequency)
{
    delayLine.setDelay(processSpec.sampleRate / frequency);
}

template void StringModel<float>::setFrequency(double frequency);
template void StringModel<double>::setFrequency(double frequency);

template<typename SampleType>
void StringModel<SampleType>::prepare(const juce::dsp::ProcessSpec& ps)
{
    double frequency = processSpec.sampleRate / delayLine.getDelay();
    delayLine.setMaximumDelayInSamples(ps.sampleRate / 20.0);
    delayLine.setDelay(ps.sampleRate / frequency);
    delayLine.prepare(ps);
    processSpec = ps;
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
