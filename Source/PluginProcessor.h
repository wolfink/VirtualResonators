/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StringModel.h"

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
    void setOutputVolume(double newInputVolume) { outputVolume = newInputVolume; }
    void setFrequency(double newFrequency) { resonatorFrequency = newFrequency;  synth.setFrequency(newFrequency); };
    void setFeedback(double newFeedback) { resonatorFeedback = newFeedback; };
    void setVolume(double newVolume) { volume = newVolume; }
    void addNoise() { noise = true; }
    
private:
    //==============================================================================
    StringModel<float> synth { juce::dsp::ProcessSpec({ 44100, 0, 1 }) };
    float volume;
    float outputVolume;
    double resonatorFrequency;
    double resonatorFeedback;
    bool noise;
    //float delay1;
    //float feedback1;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResonatorProjectAudioProcessor)
};
