
class StringModelTest : public UnitTest {
public:
	StringModelTest() : UnitTest("StringModel") {}

	void runTest()
	{
		testInitialization();
		testProcessing();
		testPluck();
	}

	void testInitialization()
	{
		beginTest("Test initialization");

		try {
			string = uptr<StringModel>(new StringModel(spec.sampleRate));
			AudioBuffer<float> buffer(spec.numChannels, spec.maximumBlockSize);

			string->prepare(spec);
			buffer.clear();
			buffer.getWritePointer(0)[0] = 1.0;

			// Check if something is returned
			string->process(buffer.getWritePointer(0), 0, spec.maximumBlockSize);
			expect(true);
		}
		catch (std::exception& e) {
			expect(false, "StringModel failed in initialization");
		}
	}

	void testProcessing() {} // TODO: implement

	void testPluck()
	{
		beginTest("Test pluck method");

		AudioBuffer<float> buffer(1, spec.maximumBlockSize);
		dsp::FFT fft(fft_order);
		int num_buckets = spec.maximumBlockSize;

		double frequency = 440.0;
		int target_bucket = frequency * (num_buckets / 2) / spec.sampleRate;

		string->setFrequency(frequency);
		string->setDecay(0.5);
		string->setVolume(1.0);
		string->prepare(spec);
		for (int _ = 0; _ < 3; _++) {
			float* data = buffer.getWritePointer(0);
			buffer.clear();
			//string->clear();
			string->pluck(0);
			for (int _ = 0; _ < 10; _++) {
				string->process(data, 0, num_buckets);
			}
			fft.performFrequencyOnlyForwardTransform(data);

			double max  = 0.0;
			int arg_max = 0;
			for (int i = 1; i < num_buckets / 2; i++) {
				if (data[i] > max) {
					max = data[i];
					arg_max = i;
				}
			}

			expectWithinAbsoluteError(arg_max, target_bucket, 1, "Plucked frequency is different than expected");
		}
	}
private:
	const int fft_order         = 12;
	const dsp::ProcessSpec spec = { 44100.0, uint32(1 << (fft_order + 1)), 1 };
	uptr<StringModel> string;
};
StringModelTest StringModel_test;