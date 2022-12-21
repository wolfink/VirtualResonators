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

class StdOutLogger : public Logger
{
    void logMessage(const String& message)
    {
        std::cout << message << std::endl;
    }
};

#include "UnitTests/PluginProcessorTest.h"
#include "UnitTests/StringModelTest.h"

//==============================================================================
int main (int argc, char* argv[])
{

    UnitTestRunner runner;
    StdOutLogger* stdout_logger = new StdOutLogger();
    //MessageManager* message_manager = MessageManager::getInstance();
    runner.setAssertOnFailure(true);

    std::cout << "========== BEGIN UNIT TESTS ==========" << std::endl;


    Logger::setCurrentLogger(stdout_logger);
    runner.runAllTests();

    std::cout << "=========== END UNIT TESTS ===========" << std::endl;
    int num_fail = 0;
    int num_pass = 0;
    for (int i = 0; i < runner.getNumResults(); i++) {
        const UnitTestRunner::TestResult* test_result = runner.getResult(i);
		num_fail += test_result->failures;
		num_pass += test_result->passes;
    }
    if (num_fail == 0)
		std::cout << "All tests passed!" << std::endl;
    else
        std::cout << num_pass << " test(s) passed, but " << num_fail << " failed!" << std::endl;
    system("pause");
    return 0;
}