/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StringModel.h"

#define NUM_RESONATORS 8

//==============================================================================
/**
*/
class ResonatorProjectAudioProcessor : public juce::AudioProcessor
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

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
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
    void addNoise(int index);
    
private:
    //==============================================================================
#if(_DEBUG)
    jcf::BufferDebugger* bufferDebugger;
    bool bufferDebuggerOn;
#endif
    juce::ValueTree stateInfo;
    std::vector<StringModel<float>> synths;
    float volume;
    float outputVolume;
    double resonatorFrequency[NUM_RESONATORS];
    //double resonatorTension;
    bool noise[NUM_RESONATORS];
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessor)
};
