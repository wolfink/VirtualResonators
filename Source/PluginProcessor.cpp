/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Resonator.h"

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout createParamLayout() {
    AudioProcessorValueTreeState::ParameterLayout layout;

    // Important constant variables
	const int            default_note_indexes[NUM_RESONATORS] = {    0,    2,    4,    5,    7,    9,
                                                                    11,    0,    2,    4,    5,    7 };
	const int         default_register_values[NUM_RESONATORS] = {    4,    4,    4,    4,    4,    4,
                                                                     4,    5,    5,    5,    5,    5 };
	const float         default_detune_values[NUM_RESONATORS] = {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
                                                                   0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };
	const float          default_decay_values[NUM_RESONATORS] = { 50.0, 50.0, 50.0, 50.0, 50.0, 50.0,
                                                                  50.0, 50.0, 50.0, 50.0, 50.0, 50.0 };
	const float        default_damping_values[NUM_RESONATORS] = { 50.0, 50.0, 50.0, 50.0, 50.0, 50.0,
                                                                  50.0, 50.0, 50.0, 50.0, 50.0, 50.0 };
	const float         default_volume_values[NUM_RESONATORS] = {   10,   10,   10,   10,   10,   10,
                                                                    10,   10,   10,   10,   10,   10 };
	const float          default_input_volume                 = 0.0;
	const float                   default_wet                 = 50.0;
	const float         default_output_volume                 = 0.0;

    for (size_t i = 0; i < NUM_RESONATORS; i++)
    {
        layout.add(std::make_unique<AudioParameterChoice>(
            NOTEVAL_ID(i), NOTEVAL_NAME(i),
            notevals, default_note_indexes[i]));
        layout.add(std::make_unique<AudioParameterInt>(
            OCTAVE_ID(i), OCTAVE_NAME(i),
            0, 8, default_register_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            DETUNE_ID(i), DETUNE_NAME(i),
            NormalisableRange<float>(-100.0, 100.0, 1.0), default_detune_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            DECAY_ID(i), DECAY_NAME(i),
            NormalisableRange<float>(0.0, 100.0, 0.1), default_decay_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            DAMPING_ID(i), DAMPING_NAME(i),
            NormalisableRange<float>(0.0, 100.0, 0.1), default_damping_values[i]));
        layout.add(std::make_unique<AudioParameterFloat>(
            VOLUME_ID(i), VOLUME_NAME(i),
            NormalisableRange<float>(0.0, 10.0, 0.01), default_volume_values[i]));
        layout.add(std::make_unique<AudioParameterBool>(TOGGLE_ID(i), TOGGLE_NAME(i), true));
    }
	layout.add(std::make_unique<AudioParameterFloat>(
        INPUT_ID, INPUT_NAME,
		NormalisableRange<float>(-100.0, 12.0, 0.1), default_input_volume));
	layout.add(std::make_unique<AudioParameterFloat>(WET_ID, WET_NAME,
		NormalisableRange<float>(0.0, 100.0, 0.1), default_wet));
	layout.add(std::make_unique<AudioParameterFloat>(OUTPUT_ID, OUTPUT_NAME,
		NormalisableRange<float>(-100.0, 12.0, 0.1), default_output_volume));
    return layout;
}

VirtualResonatorsAudioProcessor::VirtualResonatorsAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    _resonators(std::vector<Resonator>(NUM_RESONATORS, Resonator(44100))),
    _parameters(*this, nullptr, "PARAMS", createParamLayout()),
    _mono(false)
{
}

VirtualResonatorsAudioProcessor::~VirtualResonatorsAudioProcessor()
{
}

//==============================================================================
const juce::String VirtualResonatorsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VirtualResonatorsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VirtualResonatorsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VirtualResonatorsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VirtualResonatorsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VirtualResonatorsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VirtualResonatorsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VirtualResonatorsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VirtualResonatorsAudioProcessor::getProgramName (int index)
{
    return {};
}

void VirtualResonatorsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VirtualResonatorsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    for (int index = 0; index < NUM_RESONATORS; index++) {
        _resonators[index].setModel(ResonatorModel::KarplusStrong);
        _resonators[index].setParameters(KarplusStrongParameters {0.0, 0.0, 440.0});
        _resonators[index].prepare(juce::dsp::ProcessSpec({ sampleRate, (juce::uint32)samplesPerBlock, (juce::uint32)getTotalNumOutputChannels() }));
    }
    for (int channel = 0; channel < getTotalNumInputChannels(); channel++) {
        _dc_blockers.push_back(DCBlocker());
    }
}

void VirtualResonatorsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VirtualResonatorsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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


void VirtualResonatorsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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


        float  wet                 = PARAM_VAL(WET_ID) / 100.0;
        float* output_channel_data = new float[buffer.getNumSamples()]{ 0 };
        for (int index = 0; index < NUM_RESONATORS; index++) {
			AudioBuffer<float> buffer_copy;
            float octave    = PARAM_VAL(OCTAVE_ID(index));
            float note      = PARAM_VAL(NOTEVAL_ID(index));
            float detune    = PARAM_VAL(DETUNE_ID(index));
            float decay     = (100 - PARAM_VAL(DECAY_ID(index))) / 200;
            float damping   = 1 - std::pow(10, PARAM_VAL(DAMPING_ID(index)) / 25.0 - 5);
            float volume    = PARAM_VAL(TOGGLE_ID(index)) * PARAM_VAL(VOLUME_ID(index)) / 10;
			float frequency = 440.0 * std::pow(2, (octave - 4.0) +  (note - 9.0) / 12.0 + detune / 1200.0);
            KarplusStrongParameters ks;
            ks.damping = damping;
            ks.decay = decay;
            ks.frequency = frequency;

            buffer_copy.makeCopyOf(buffer);
            auto channel_data_copy = buffer_copy.getWritePointer(channel);

            //_resonators[index].setFrequency(frequency);
            //_resonators[index].setDecay(decay);
            //_resonators[index].setDamping(damping);
            _resonators[index].setModel(ResonatorModel::KarplusStrong);
            _resonators[index].setParameters(ks);
            _resonators[index].setVolume(volume);
			_resonators[index].process(channel_data_copy, channel, num_samples);

            for (int i = 0; i < num_samples; i++)
                output_channel_data[i] += channel_data_copy[i] / NUM_RESONATORS;
        }

        // Copy proccessed data into buffer through a DC blocking filter to prevent value drift
        for (int sample = 0; sample < num_samples; sample++) {
            float y = _dc_blockers[channel].process_sample(output_channel_data[sample]);
            channel_data[sample] = channel_data[sample] * (1 - wet)  + y * wet; // Wet signal + Dry signal
        }
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

    if (_mono) {
        buffer.addFrom (0, 0, buffer, 1, 0, num_samples);
        buffer.copyFrom(1, 0, buffer, 0, 0, num_samples);
        buffer.applyGain(0.5f);
    }
}

//==============================================================================
bool VirtualResonatorsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VirtualResonatorsAudioProcessor::createEditor()
{
    return new VirtualResonatorsProcessorEditor (*this);
}

//==============================================================================
void VirtualResonatorsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    String xml = _parameters.state.toXmlString();
    destData.setSize(xml.length());
    destData.copyFrom((void *) xml.toRawUTF8(), 0, xml.length());
}

void VirtualResonatorsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ValueTree new_state = ValueTree().fromXml(*parseXML(String((char*)data, sizeInBytes)));

    for (ValueTree child : new_state) {
        auto id    = child.getProperty("id");
        auto value = child.getProperty("value");

        _parameters.state
            .getChildWithProperty("id", id)
            .setProperty("value", value, nullptr);
    }
    //_parameters.state.copyPropertiesAndChildrenFrom(new_state, nullptr);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VirtualResonatorsAudioProcessor();
}

//==============================================================================
void VirtualResonatorsAudioProcessor::pluckResonator(int index)
{
	_resonators[index].pluck();
}
