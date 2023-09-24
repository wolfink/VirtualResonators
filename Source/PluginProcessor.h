/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "Resonator.h"
#include "VirtualResonators.h"

#define NUM_RESONATORS 12

// Names and IDs for parameters
#define    NOTEVAL_ID(index) "note value "          + std::to_string(index)
#define  NOTEVAL_NAME(index) "Note->Resonator "     + std::to_string(index)
#define     OCTAVE_ID(index) "note register "       + std::to_string(index)
#define   OCTAVE_NAME(index) "Register->Resonator " + std::to_string(index)
#define     DETUNE_ID(index) "note detune "         + std::to_string(index)
#define   DETUNE_NAME(index) "Detune->Resonator"    + std::to_string(index)
#define      DECAY_ID(index) "decay "               + std::to_string(index)
#define    DECAY_NAME(index) "Decay->Resonator "    + std::to_string(index)
#define    DAMPING_ID(index) "damping "             + std::to_string(index)
#define  DAMPING_NAME(index) "Damping->Resonator"   + std::to_string(index)
#define     VOLUME_ID(index) "resonator volume "    + std::to_string(index)
#define   VOLUME_NAME(index) "Volume->Resonator "   + std::to_string(index)
#define     TOGGLE_ID(index) "resonator toggle "    + std::to_string(index)
#define   TOGGLE_NAME(index) "On/Off->Resonator "   + std::to_string(index)
#define      INPUT_ID        "input volume"
#define    INPUT_NAME        "Input"
#define        WET_ID        "wet"
#define      WET_NAME        "Wet %"
#define     OUTPUT_ID        "output"
#define   OUTPUT_NAME        "Output"

const StringArray notevals = { "C", "C#/Db", "D", "D#/Eb",
							   "E", "F", "F#/Gb", "G",
							   "G#/Ab", "A", "A#/Gb", "B" };

#define PARAM_VAL(name) *_parameters.getRawParameterValue(name)

//==============================================================================
/**
*/

class DCBlocker;

class VirtualResonatorsAudioProcessor : public AudioProcessor
{
    //==============================================================================
    std::vector<Resonator> _resonators;
    std::vector<DCBlocker>   _dc_blockers;

public:

    AudioProcessorValueTreeState _parameters;
    bool _mono;

    //==============================================================================
    VirtualResonatorsAudioProcessor();
    ~VirtualResonatorsAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    void pluckResonator(int index);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VirtualResonatorsAudioProcessor)
};

class DCBlocker {
    float _prev_x;
    float _prev_y;
public:
    DCBlocker(): _prev_x(0.0f), _prev_y(0.0f) {}
    float process_sample(float sample)
    {
        float retval = sample - _prev_x + 0.995 * _prev_y;
        _prev_x = sample;
        _prev_y = retval;
        return retval;
    }
};
