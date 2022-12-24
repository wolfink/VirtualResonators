/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout createParamLayout() {
    AudioProcessorValueTreeState::ParameterLayout layout;

    // Important constant variables
	const int            default_note_indexes[NUM_RESONATORS] = {   0,   2,   4,   5,   7,   9,  11,   0 };
	const int         default_register_values[NUM_RESONATORS] = {   4,   4,   4,   4,   4,   4,   4,   5 };
	const float         default_detune_values[NUM_RESONATORS] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	const float          default_decay_values[NUM_RESONATORS] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
	const float         default_volume_values[NUM_RESONATORS] = {  10,  10,  10,  10,  10,  10,  10,  10 };
    const float        default_damping_values[NUM_RESONATORS] = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 };
	const float          default_input_volume                 = 0.0;
	const float                   default_wet                 = 0.5;
	const float         default_output_volume                 = 0.0;
	const StringArray                notevals                 = { "C", "C#/Db", "D", "D#/Eb",
                                                                  "E", "F", "F#/Gb", "G", 
                                                                  "G#/Ab", "A", "A#/Gb", "B" };
    for (size_t i = 0; i < NUM_RESONATORS; i++)
    {
        layout.add(std::make_unique<AudioParameterChoice>(
            NOTEVAL_ID(i), NOTEVAL_NAME(i),
            notevals, default_note_indexes[i]));
        layout.add(std::make_unique<AudioParameterInt>(
            REGISTER_ID(i), REGISTER_NAME(i),
            0, 8, default_register_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            DETUNE_ID(i), DETUNE_NAME(i),
            NormalisableRange<float>(-100.0, 100.0, 1.0), default_detune_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            DECAY_ID(i), DECAY_NAME(i),
            NormalisableRange<float>(0.0, 100.0, 100.0 / 127.0), default_decay_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            DAMPING_ID(i), DAMPING_NAME(i),
            NormalisableRange<float>(0.0, 4.0, 4.0 / 127.0), default_damping_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            VOLUME_ID(i), VOLUME_NAME(i),
            NormalisableRange<float>(0.0, 10.0, 10.0 / 127.0), default_volume_values[i]));
    }
	layout.add(std::make_unique<AudioParameterFloat>(
        INPUT_ID, INPUT_NAME,
		NormalisableRange<float>(-100.0, 12.0, 0.1), default_input_volume));
	layout.add(std::make_unique<AudioParameterFloat>(WET_ID, WET_NAME,
		NormalisableRange<float>(0.0, 1.0, 0.008), default_wet));
	layout.add(std::make_unique<AudioParameterFloat>(OUTPUT_ID, OUTPUT_NAME,
		NormalisableRange<float>(-100.0, 12.0, 0.1), default_output_volume));
    return layout;
}

ResonatorProjectAudioProcessor::ResonatorProjectAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::mono(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    synths(std::vector<StringModel>(NUM_RESONATORS, StringModel(44100))),
    parameters(*this, nullptr, "PARAMS", createParamLayout()),
    _xm1(0.0),
    _ym1(0.0)
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
void ResonatorProjectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    _xm1 = 0.0;
    _ym1 = 0.0;
    //juce::ValueTree node;
    for (int i = 0; i < NUM_RESONATORS; i++) {
        synths[i].setFrequency(440.0);
        synths[i].prepare(juce::dsp::ProcessSpec({ sampleRate, (juce::uint32)samplesPerBlock, (juce::uint32)getNumOutputChannels() }));
    }
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
    auto num_input_channels  = getTotalNumInputChannels();
    auto num_output_channels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = num_input_channels; i < num_output_channels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int num_samples = buffer.getNumSamples();

    // Process the audio data
    for (int channel = 0; channel < num_input_channels; ++channel)
    {
        auto  channel_data  = buffer.getWritePointer (channel);

        float input_volume  = PARAM_VAL(INPUT_ID);
        input_volume        = Decibels::decibelsToGain(input_volume);

		buffer.applyGain(input_volume);

        // Update synthesizer parameters, and add noise, before processing audio
        for (int i = 0; i < NUM_RESONATORS; i++) {
            float octave    = PARAM_VAL(REGISTER_ID(i));
            float note      = PARAM_VAL(NOTEVAL_ID(i));
            float detune    = PARAM_VAL(DETUNE_ID(i));
            float decay     = PARAM_VAL(DECAY_ID(i)) / 200;
            float damping   = 1 - std::pow(10, PARAM_VAL(DAMPING_ID(i)) - 5);
            float volume    = PARAM_VAL(VOLUME_ID(i)) / 10;
			float frequency = 440.0 * std::pow(2, (octave - 4.0) +  (note - 9.0) / 12.0 + detune / 1200.0);
            synths[i].setFrequency(frequency);
            synths[i].setDecay(decay);
            synths[i].setDamping(damping);
            synths[i].setVolume(volume);
        }


        float  wet                 = PARAM_VAL(WET_ID);
        float* output_channel_data = new float[buffer.getNumSamples()]{ 0 };
#if(_DEBUG)
		bufferDebugger->capture("Pre FX", channel_data, num_samples, -1.0, 1.0);
#endif
        for (int i = 0; i < NUM_RESONATORS; i++) {
			AudioBuffer<float> buffer_copy;
            buffer_copy.makeCopyOf(buffer);
            auto channel_data_copy = buffer_copy.getWritePointer(channel);

			synths[i].process(channel_data_copy, channel, num_samples);

            for (int j = 0; j < num_samples; j++)
                output_channel_data[j] += channel_data_copy[j] / NUM_RESONATORS;
#if(_DEBUG)
			bufferDebugger->capture("Post FX #" + std::to_string(i), output_channel_data, num_samples, -1.0, 1.0);
#endif
        }

        // Copy proccessed data into buffer through a DC blocking filter to prevent value drift
        for (int j = 0; j < num_samples; j++) {
            float x = output_channel_data[j];
            float y = x - _xm1 + 0.995 * _ym1;
            channel_data[j] = channel_data[j] * (1 - wet)  + y * wet; // Wet signal + Dry signal
            _xm1 = x; _ym1 = y;
        }
#if(_DEBUG)
        bufferDebugger->capture("channelData", channel_data, num_samples, -1.0, 1.0);
#endif
    }

    for (int channel = 0; channel < num_output_channels; channel++) {
        // For mono input, copy values into all other channels
        if (num_input_channels == 1 && channel > 0) {
            auto mono_channel_data = buffer.getWritePointer(0);
            auto curr_channel_data = buffer.getWritePointer(channel);
            for (int i = 0; i < num_samples; i++) curr_channel_data[i] = mono_channel_data[i];
        }

		float output_volume = PARAM_VAL(OUTPUT_ID);
		output_volume       = Decibels::decibelsToGain(output_volume);

		buffer.applyGain(output_volume);
    }
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
    String xml = parameters.state.toXmlString();
    destData.setSize(xml.length());
    destData.copyFrom((void *) xml.toRawUTF8(), 0, xml.length());
}

void ResonatorProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ValueTree new_state = ValueTree().fromXml(*parseXML(String((char*)data, sizeInBytes)));
    parameters.state.copyPropertiesAndChildrenFrom(new_state, nullptr);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ResonatorProjectAudioProcessor();
}

//==============================================================================
void ResonatorProjectAudioProcessor::pluckResonator(int index)
{
    for(int channel = 0; channel < getTotalNumInputChannels(); channel++)
		synths[index].pluck(channel);
}

#if(_DEBUG)
void ResonatorProjectAudioProcessor::toggleBufferDebugger()
{
    if (!bufferDebuggerOn) bufferDebugger = new jcf::BufferDebugger();
    else delete bufferDebugger;
    bufferDebuggerOn = !bufferDebuggerOn;
}
#endif
