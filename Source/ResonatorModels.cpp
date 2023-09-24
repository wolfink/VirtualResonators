#include "ResonatorModels.h"

KarplusStrongModel::KarplusStrongModel(double sample_rate) :
    _sample_rate(sample_rate),
    _averaging_sample(0.0),
    _damping(0.99999),
    _decay(0.5)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / MIN_FREQ);
}

KarplusStrongModel::KarplusStrongModel(const KarplusStrongModel& ks):
    _sample_rate(ks._sample_rate),
    _averaging_sample(ks._averaging_sample),
    _damping(ks._damping),
    _decay(ks._decay)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / MIN_FREQ);
}

void KarplusStrongModel::setParameters(const KarplusStrongParameters& params)
{
    jassert(params.damping >= 0.0 && params.damping < 1.0);
    _damping = params.damping;
    _decay = params.decay;
    _delay_line.setDelay(_sample_rate / params.frequency);
}

void KarplusStrongModel::prepare(const juce::dsp::ProcessSpec& process_spec)
{
    double frequency = _sample_rate / _delay_line.getDelay();
    _delay_line.setMaximumDelayInSamples(process_spec.sampleRate / MIN_FREQ);
    _delay_line.setDelay(process_spec.sampleRate / frequency);
    _delay_line.prepare(process_spec);
    _sample_rate = process_spec.sampleRate;
    _num_channels = process_spec.numChannels;
    _averaging_sample.resize(process_spec.numChannels);
}

void KarplusStrongModel::process(float* samples, int channel, size_t number_of_samples)
{
    for (int sample = 0; sample < number_of_samples; sample++)
    {
        float delayedSample = _delay_line.popSample(channel);
        samples[sample] += _damping * (_decay * _averaging_sample[channel] + (1 - _decay) * delayedSample);
        _averaging_sample[channel] = delayedSample;
        _delay_line.pushSample(channel, samples[sample]);
    }
}

void KarplusStrongModel::pluck()
{
    auto num_channels = _delay_line;
    auto num_samples = _delay_line.getDelay();
    juce::Random random;
    random.setSeedRandomly();
    clear();

    LOOP(num_samples) {
        auto sample = random.nextFloat() * 2.0 - 1.0;
        for (int channel = 0; channel < _num_channels; channel++) {
            _delay_line.pushSample(channel, sample);
            _delay_line.popSample(channel);
		}
	}
}

void KarplusStrongModel::clear()
{
    _delay_line.reset();
}


WaveguidesModel::WaveguidesModel(double sample_rate) :
    _sample_rate(sample_rate),
    _averaging_sample(0.0),
    _damping(0.99999),
    _decay(0.5)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / MIN_FREQ);
}

WaveguidesModel::WaveguidesModel(const WaveguidesModel& wg):
    _sample_rate(wg._sample_rate),
    _averaging_sample(wg._averaging_sample),
    _damping(wg._damping),
    _decay(wg._decay)
{
    _delay_line.setMaximumDelayInSamples(_sample_rate / MIN_FREQ);
}

void WaveguidesModel::setParameters(const WaveguidesParameters& params)
{
    jassert(params.damping >= 0.0 && params.damping < 1.0);
    _damping = params.damping;
    _decay = params.decay;
    _delay_line.setDelay(_sample_rate / params.frequency);
}

void WaveguidesModel::prepare(const juce::dsp::ProcessSpec& process_spec)
{
    double frequency = _sample_rate / _delay_line.getDelay();
    _delay_line.setMaximumDelayInSamples(process_spec.sampleRate / MIN_FREQ);
    _delay_line.setDelay(process_spec.sampleRate / frequency);
    _delay_line.prepare(process_spec);
    _sample_rate = process_spec.sampleRate;
    _num_channels = process_spec.numChannels;
    _averaging_sample.resize(process_spec.numChannels);
}

void WaveguidesModel::process(float* samples, int channel, size_t number_of_samples)
{
    for (int sample = 0; sample < number_of_samples; sample++)
    {
        float delayedSample = _delay_line.popSample(channel);
        samples[sample] += _damping * (_decay * _averaging_sample[channel] + (1 - _decay) * delayedSample);
        _averaging_sample[channel] = delayedSample;
        _delay_line.pushSample(channel, samples[sample]);
    }
}

void WaveguidesModel::clear()
{
    _delay_line.reset();
}

void WaveguidesModel::pluck()
{
    auto num_channels = _delay_line;
    auto num_samples = _delay_line.getDelay();
    juce::Random random;
    random.setSeedRandomly();
    clear();

    LOOP(num_samples) {
        auto sample = random.nextFloat() * 2.0 - 1.0;
        for (int channel = 0; channel < _num_channels; channel++) {
            _delay_line.pushSample(channel, sample);
            _delay_line.popSample(channel);
		}
	}
}
