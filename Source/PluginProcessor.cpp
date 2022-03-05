/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ResonatorProjectAudioProcessor::ResonatorProjectAudioProcessor() :
synths(std::vector<StringModel<float>>(NUM_RESONATORS, StringModel<float>(44100))),
stateInfo("root")
#ifndef JucePlugin_PreferredChannelConfigurations
     , AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
}

ResonatorProjectAudioProcessor::~ResonatorProjectAudioProcessor()
{
}

//==============================================================================
const juce::String ResonatorProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ResonatorProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ResonatorProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ResonatorProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ResonatorProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ResonatorProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ResonatorProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ResonatorProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ResonatorProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void ResonatorProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ResonatorProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
#if(_DEBUG)
    bufferDebuggerOn = false;
#endif
    _xm1 = 0.0;
    _ym1 = 0.0;
    juce::ValueTree node;
    for (int i = 0; i < NUM_RESONATORS; i++) {
        node = juce::ValueTree("resonator");
		synths[i].setFrequency(440.0);
		synths[i].prepare(juce::dsp::ProcessSpec({ sampleRate, (juce::uint32) samplesPerBlock, 1 }));
		resonatorFrequency[i] = 440.0;
		noise[i] = false;
        node.setProperty("frequency", 440.0, nullptr);
        node.setProperty("decay time", 1.0, nullptr);
        node.setProperty("volume", 1.0, nullptr);
        node.setProperty("add noise", false, nullptr);
        stateInfo.addChild(node, -1, nullptr);
    }
    stateInfo.addChild(node, -1, nullptr);
#if (_DEBUG)
	node = juce::ValueTree("buffer debugger");
	node.setProperty("on", false, nullptr);
	stateInfo.addChild(node, -1, nullptr);
#endif
    stateInfo.setProperty("gamma", 0.2, nullptr);
    stateInfo.setProperty("input", 1.0, nullptr);
    stateInfo.setProperty("output", 1.0, nullptr);
    stateInfo.setProperty("wet", 0.5, nullptr);
}

void ResonatorProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ResonatorProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ResonatorProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Process the audio data
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        float *channelDataCopy[NUM_RESONATORS];
        float volume[NUM_RESONATORS];

		buffer.applyGain(stateInfo.getProperty("input"));

        // Update synthesizer parameters, and add noise, before processing audio
        for (int i = 0; i < NUM_RESONATORS; i++) {
			channelDataCopy[i] = new float[buffer.getNumSamples()]{ 0 };
            synths[i].setFrequency(stateInfo.getChild(i).getProperty("frequency"));
            synths[i].setDecay(stateInfo.getChild(i).getProperty("decay time"));
            volume[i] = stateInfo.getChild(i).getProperty("volume");
			if (stateInfo.getChild(i).getProperty("add noise")) {
				juce::Random random;
				random.setSeedRandomly();
				for (int j = 0; j < (int)(getSampleRate() / resonatorFrequency[i]); j++) channelDataCopy[i][j] = random.nextFloat() * 2.0 - 1.0;
                stateInfo.getChild(i).setProperty("add noise", false, nullptr);
			}
        }


        float gamma = stateInfo.getProperty("gamma");
        float wet = stateInfo.getProperty("wet");
        float* proccessedChannelData = new float[buffer.getNumSamples()]{ 0 };
#if(_DEBUG)
		bufferDebugger->capture("Pre FX", channelData, buffer.getNumSamples(), -1.0, 1.0);
#endif
        for (int i = 0; i < NUM_RESONATORS; i++) {
            for (int j = 0; j < buffer.getNumSamples(); j++) channelDataCopy[i][j] += channelData[j] / NUM_RESONATORS;
			synths[i].process(channelDataCopy[i], channel, buffer.getNumSamples());
            for (int j = 0; j < buffer.getNumSamples(); j++) proccessedChannelData[j] += gamma * volume[i] * channelDataCopy[i][j];
#if(_DEBUG)
			bufferDebugger->capture("Post FX #" + std::to_string(i), proccessedChannelData, buffer.getNumSamples(), -1.0, 1.0);
#endif
            delete channelDataCopy[i];
        }
        // Copy proccessed data into buffer through a DC blocking filter to prevent value drift
        for (int j = 0; j < buffer.getNumSamples(); j++) {
            float x = proccessedChannelData[j];
            float y = x - _xm1 + 0.995 * _ym1;
            channelData[j] = channelData[j] * (1 - wet)  + y * wet; // Wet signal + Dry signal
            _xm1 = x; _ym1 = y;
        }
#if(_DEBUG)
        bufferDebugger->capture("channelData", channelData, buffer.getNumSamples(), -1.0, 1.0);
#endif
    }

	buffer.applyGain(stateInfo.getProperty("output"));
}

//==============================================================================
bool ResonatorProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ResonatorProjectAudioProcessor::createEditor()
{
    return new ResonatorProjectAudioProcessorEditor (*this);
}

//==============================================================================
void ResonatorProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    destData.copyFrom((void *) &stateInfo, 0, sizeof(juce::ValueTree));
}

void ResonatorProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    jassert(sizeInBytes == sizeof(juce::ValueTree));
    stateInfo = *(juce::ValueTree*) data;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ResonatorProjectAudioProcessor();
}

//==============================================================================
void ResonatorProjectAudioProcessor::setOutputVolume(double newInputVolume)
{
    outputVolume = newInputVolume;
}

void ResonatorProjectAudioProcessor::setFrequency(int index, double newFrequency)
{
    jassert(index < NUM_RESONATORS);
    resonatorFrequency[index] = newFrequency;
    synths[index].setFrequency(newFrequency);
}

void ResonatorProjectAudioProcessor::setDecay(int index, double newTension)
{
    //resonatorTension = newTension;
    synths[index].setDecay(newTension);
}

void ResonatorProjectAudioProcessor::setVolume(double newVolume)
{
    volume = newVolume;
}

void ResonatorProjectAudioProcessor::addNoise(int index)
{
    noise[index] = true;
}

#if(_DEBUG)
void ResonatorProjectAudioProcessor::toggleBufferDebugger()
{
    if (!bufferDebuggerOn) bufferDebugger = new jcf::BufferDebugger();
    else delete bufferDebugger;
    bufferDebuggerOn = !bufferDebuggerOn;
}
#endif
