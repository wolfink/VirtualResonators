
#define SET_PARAMETER(tree, name, value) \
    tree.getChildWithProperty("id", String(name)).setProperty("value", value, nullptr); \
    setStateInfo(tree)

class PluginProcessorTest : public UnitTest
{
public:
    PluginProcessorTest() : UnitTest("PluginProcessor") {}


    void runTest() override
    {
		testInitialization();
		testStateInfoMethods();
		testProcessBlock();
    }

private:
    void testInitialization()
    {
        beginTest("Test initilization");

        message_manager = std::unique_ptr<MessageManager>(MessageManager::getInstance());
        audio_processor = std::make_unique<ResonatorProjectAudioProcessor>();

        expect(audio_processor->getName().isNotEmpty(), "ResonatorProjectAudioProcessor has no name");
    }

    void testStateInfoMethods()
    {
        beginTest("Test getStateInformation method");

        ValueTree state_info = getStateInfo();

        expect(state_info.isValid(), String("State information is not valid"));
        expectGreaterThan(state_info.getNumChildren(), 0, String("State information is empty"));
        expect(state_info.getChildWithName("PARAM").isValid(), String("State information contains no parameters"));

        beginTest("Test setStateInformation method");

        state_info.getChild(0).setProperty("value", 1.0, nullptr);

        setStateInfo(state_info);

        state_info = getStateInfo();

		double test_val        = state_info.getChild(0).getProperty("value");
		expectEquals(test_val, 1.0, "State information not set properly!");
    }

    void setStateInfo(const ValueTree& tree) {
		String xml = tree.toXmlString();
		audio_processor->setStateInformation((void *) xml.toRawUTF8(), xml.length());
    }

    ValueTree getStateInfo() {

        MemoryBlock mem(0);
		audio_processor->getStateInformation(mem);
		uptr<XmlElement> xml = parseXML(
			String((char*)mem.getData(), mem.getSize())
		);
        return ValueTree().fromXml(*xml);

    }

    void testProcessBlock()
    {

        int num_input_channels  = audio_processor->getTotalNumInputChannels();
        int num_output_channels = audio_processor->getTotalNumOutputChannels();

        double sample_rate = 44100.0;
        int    block_size  = 256;

		AudioBuffer<float> buffer(num_output_channels, block_size);
		MidiBuffer _unused;

        audio_processor->prepareToPlay(sample_rate, block_size);
		buffer.clear();
		_unused.clear();

        beginTest("Test empty buffer");

		for (int _ = 0; _ < 100; _++) audio_processor->processBlock(buffer, _unused);
		expectWithinAbsoluteError((double) buffer.getRMSLevel(0, 0, block_size),
			0.0,
			Decibels::decibelsToGain(-90.0),
			"Empty buffer is not quiet when processed");

		beginTest("Test sine wave");

		float frequency = 440.0;
		float pi = MathConstants<float>::pi;
		dsp::FFT analysis(12);
		int num_buckets = analysis.getSize() * 2;
		std::vector<float> processed_sin_wave_data(num_buckets);

		buffer.clear();

		int sample = 0;
		double angle = 0.0;
		while (true) {
			auto* buffer_data = buffer.getWritePointer(0);

			for (int i = 0; i < block_size; i++) {
				angle += 2 * pi * frequency / sample_rate;
				buffer_data[i] = std::sin(angle);
			}
			audio_processor->processBlock(buffer, _unused);
			for (int i = 0; i < block_size; i++, sample++) {
				if (sample == num_buckets / 2) goto LEAVE_SIN_GEN;
				processed_sin_wave_data[sample] = buffer_data[i];
			}
		}
LEAVE_SIN_GEN:

		float output_volume = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, block_size));
		expectWithinAbsoluteError(output_volume, 0.0f, 3.0f, "Output volume not in acceptable range.");

		analysis.performFrequencyOnlyForwardTransform(processed_sin_wave_data.data());

		float max     = 0.0;
		int   arg_max = 0;
		for (int i = 0; i < num_buckets; i++) {
			if (processed_sin_wave_data[i] > max) {
				max = processed_sin_wave_data[i];
				arg_max = i;
			}
		}

		int target_bucket = frequency * (num_buckets / 2) / sample_rate;
		expectWithinAbsoluteError(arg_max, target_bucket, 2, "Resonators shift fundemental frequecy");

    }

	std::unique_ptr<MessageManager> message_manager;
	std::unique_ptr<ResonatorProjectAudioProcessor> audio_processor;
};

static PluginProcessorTest pluginProcessor_test;