
#define SET_PARAMETER(processor, tree, name, value) \
    tree.getChildWithProperty("id", String(name)).setProperty("value", value, nullptr); \
    processor.setStateInformation((void *) &tree, sizeof(tree))

class PluginProcessorTest : public UnitTest
{
public:
    PluginProcessorTest() : UnitTest("PluginProcessor") {}

    void runTest() override
    {
        beginTest("Test initilization");

        MessageManager* message_manager = MessageManager::getInstance();

        ResonatorProjectAudioProcessor audio_processor;
        expect(audio_processor.getName().isNotEmpty(), "ResonatorProjectAudioProcessor has no name");


        beginTest("Test getStateInformation method");

        MemoryBlock mem(sizeof(ValueTree));
        audio_processor.getStateInformation(mem);
        ValueTree state_info = *(ValueTree*)mem.getData();
        expect(state_info.isValid(), String("State information is not valid"));
        expectGreaterThan(state_info.getNumChildren(), 0, String("State information is empty"));
        expect(state_info.getChildWithName("PARAM").isValid(), String("State information contains no parameters"));

        beginTest("Test setStateInformation method");

        state_info.getChild(0).setProperty("value", 1.0, nullptr);
        audio_processor.setStateInformation((void *) &state_info, sizeof(state_info));
        audio_processor.getStateInformation(mem);
        state_info = *(ValueTree*)mem.getData();

        double test_val        = state_info.getChild(0).getProperty("value");
        String failure_message = "State information not set properly!";
        expectEquals(test_val, 1.0, failure_message);

        int num_input_channels  = audio_processor.getTotalNumInputChannels();
        int num_output_channels = audio_processor.getTotalNumOutputChannels();


        double sample_rate = 44100.0;
        int    block_size  = 256;
        audio_processor.prepareToPlay(sample_rate, block_size);
        expectEquals(audio_processor.getSampleRate(), sample_rate, "Audio processor sample rate not set");
        expectEquals(audio_processor.getBlockSize(), block_size, "Audio processor block size not set");

        beginTest("Test empty buffer");
        {
            AudioBuffer<float> empty_buffer(num_output_channels, block_size);
            MidiBuffer _unused;

            empty_buffer.clear();
            _unused.clear();

            for (int _ = 0; _ < 100; _++)
				audio_processor.processBlock(empty_buffer, _unused);
			expectWithinAbsoluteError((double) empty_buffer.getRMSLevel(0, 0, block_size),
				0.0,
				Decibels::decibelsToGain(-90.0),
				"Empty buffer is not quiet when processed");
        }

		beginTest("Test sin wave");

        // Test only dry signal
        SET_PARAMETER(audio_processor, state_info, WET_ID, 0.0);
		{
            double frequency = 440.0;
            float pi = MathConstants<float>::pi;
			AudioBuffer<float> sine_buffer(
				audio_processor.getTotalNumOutputChannels(),
				block_size);
			MidiBuffer _unused;
            dsp::FFT analysis(12);
            int num_buckets = analysis.getSize() * 2;
            int target_bucket = frequency * num_buckets / sample_rate;
            std::vector<float> processed_sin_wave_data;

            sine_buffer.clear();

            int num_samples_processed = 0;
            while (true) {
				auto* buffer_data = sine_buffer.getWritePointer(0);

				double angle = 0;
				for (int i = 0; i < block_size; i++) {
					buffer_data[i] = std::sin(angle);
					angle += 2 * pi * frequency / sample_rate;
					if (angle >= 2 * pi) angle -= 2 * pi;
				}
				audio_processor.processBlock(sine_buffer, _unused);
                for (int i = 0; i < block_size; i++, num_samples_processed++) {
                    if (num_samples_processed == num_buckets) goto LEAVE_SIN_GEN;
                    processed_sin_wave_data.push_back(buffer_data[i]);
                }
            }
LEAVE_SIN_GEN:
            analysis.performRealOnlyForwardTransform(processed_sin_wave_data.data(), true);

            float max     = 0.0;
            int   arg_max = 0;
            for (int i = 0; i < num_buckets; i++) {
                if (processed_sin_wave_data[i] > max) {
                    max = processed_sin_wave_data[i];
                    arg_max = i;
                }
            }

            expectWithinAbsoluteError(arg_max, target_bucket, 2, "Resonators shift fundemental frequecy");
		}

		//beginTest("Test pluckResonator");
		//audio_processor.pluckResonator(0);
    }
};

static PluginProcessorTest pluginProcessor_test;