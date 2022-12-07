/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

  name:             VirtualResonatorsTest

  dependencies: jcf_debug,juce_audio_basics,juce_audio_devices,juce_audio_formats,juce_audio_processors,juce_audio_utils,juce_core,juce_data_structures,juce_dsp,juce_events,juce_graphics,juce_gui_basics,juce_gui_extra,source_code
  exporters:        VS2019

  moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

  type:             Console

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

class PluginProcessorTest : public UnitTest
{
public:
    PluginProcessorTest() : UnitTest("PluginProcessor") {}

    void runTest() override
    {
        beginTest("Test initilization");

        MessageManager* message_manager = MessageManager::getInstance();

        ResonatorProjectAudioProcessor audio_processor;

        beginTest("Test getStateInformation and setStateInformation");

        MemoryBlock mem(sizeof(ValueTree));
        audio_processor.getStateInformation(mem);
        ValueTree stateInfo = *(ValueTree*)mem.getData();
        std::cout << stateInfo.toXmlString() << std::endl;
        stateInfo.getChildWithProperty("id", "decay 0").setProperty("value", 1.0, nullptr);
        audio_processor.setStateInformation((void *) &stateInfo, sizeof(stateInfo));
        audio_processor.getStateInformation(mem);
        stateInfo = *(ValueTree*)mem.getData();

        double test_val = stateInfo.getChildWithProperty("id", "decay 0").getProperty("value");
        String failure_message = "Expected: 1.0\nActual: " + Value(test_val).toString();

        expectEquals(test_val, 1.0, failure_message);
    }
};

static PluginProcessorTest pluginProcessor_test;

//==============================================================================
int main (int argc, char* argv[])
{

    UnitTestRunner runner;
    runner.setAssertOnFailure(false);

    std::cout << "========== BEGIN UNIT TESTS ==========" << std::endl;

    runner.runAllTests();

    std::cout << "=========== END UNIT TESTS ===========" << std::endl;
    for (int i = 0; i < runner.getNumResults(); i++) {
        if (runner.getResult(i)->failures > 0) {
            std::cout << "Some tests failed!" << std::endl;
            goto LEAVE;
        }
    }
    std::cout << "All tests passed!" << std::endl;
LEAVE:
    system("pause");
    return 0;
}