#include <juce_dsp/juce_dsp.h>
#include "VirtualResonators.h"

#define MIN_FREQ 10.0

struct KarplusStrongParameters
{
    double damping;
    double decay;
    double frequency;
};

class KarplusStrongModel
{
    double _sample_rate;
    int    _num_channels;

    std::vector<float> _averaging_sample;
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Thiran> _delay_line;

    double _decay;
    double _damping;
public:

    KarplusStrongModel(double sample_rate);
    KarplusStrongModel(const KarplusStrongModel&);

    void clear();
    void pluck();
    void prepare(const juce::dsp::ProcessSpec& process_spec);
    void process(float* samples, int channel, size_t number_of_samples);
    void setParameters(const KarplusStrongParameters&);
};

struct WaveguidesParameters
{
    double damping;
    double decay;
    double frequency;
};

class WaveguidesModel
{
    double _sample_rate;
    int    _num_channels;

    std::vector<float> _averaging_sample;
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Thiran> _delay_line;

    double _decay;
    double _damping;
public:

    WaveguidesModel(double sample_rate);
    WaveguidesModel(const WaveguidesModel&);

    void clear();
    void pluck();
    void prepare(const juce::dsp::ProcessSpec& process_spec);
    void process(float* samples, int channel, size_t number_of_samples);
    void setParameters(const WaveguidesParameters&);
};
