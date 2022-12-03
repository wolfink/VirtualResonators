/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StringModel.h"
#include "VirtualResonators.h"

#define NUM_RESONATORS 8

// Names and IDs for parameters
#define    NOTEVAL_ID(index) "note value "          + std::to_string(index)
#define  NOTEVAL_NAME(index) "Note->Resonator "     + std::to_string(index)
#define   REGISTER_ID(index) "note register "       + std::to_string(index)
#define REGISTER_NAME(index) "Register->Resonator " + std::to_string(index)
#define      DECAY_ID(index) "decay "               + std::to_string(index)
#define    DECAY_NAME(index) "Decay->Resonator "    + std::to_string(index)
#define     VOLUME_ID(index) "resonator volume "    + std::to_string(index)
#define   VOLUME_NAME(index) "Volume->Resonator "   + std::to_string(index)
#define      INPUT_ID        "input volume"
#define    INPUT_NAME        "Input"
#define        WET_ID        "wet"
#define      WET_NAME        "Wet %"
#define     OUTPUT_ID        "output"
#define   OUTPUT_NAME        "Output"

//==============================================================================
/**
*/
class ResonatorProjectAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================
    ResonatorProjectAudioProcessor();
    ~ResonatorProjectAudioProcessor() override;

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
    //void updateFilter() { resonator->setCoefficients(juce::IIRCoefficients::makeBandPass(sr, resonatorFrequency, resonatorFeedback)); }
#if(_DEBUG)
    void toggleBufferDebugger();
#endif
    void setOutputVolume(double newInputVolume);
    void setFrequency(int index, double newFrequency);
    void setDecay(int index, double newTension);
    void setVolume(double newVolume);
    void pluckResonator(int index);

    AudioProcessorValueTreeState parameters;
    
private:
    //==============================================================================

#if(_DEBUG)
    jcf::BufferDebugger* bufferDebugger;
    bool bufferDebuggerOn = false;
    bool _plucked          = false;
#endif

    std::vector<StringModel<float>> synths;

    // For DC Blocking
    float _xm1;
    float _ym1;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessor)
};
