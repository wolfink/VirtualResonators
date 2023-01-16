# Virtual Resonators
VirtualResonators is a VST3 plugin that models sympathetic strings using the Karplus-Strong algorithm.

## Dependencies
* **JUCE:** version 6.1.4
* **juce-toys**

## Building
1. Install JUCE, make sure to set JUCE modules and User modules path in **Projucer &rarr; Global Paths...**
2. Download juce-toys from https://github.com/jcredland/juce-toys and place in User modules path. **(Mac OS only)** Replace `%User modules%/juce-toys-master/jcf_debug/source/font_and_colour_designer.cpp` with `%VirtualResonators%/Misc/MacOS/font_and_colour_designer.cpp`
3. Open VirtualResonators.jucer file with Projucer
4. Use the Projucer to generate a project in your selected IDE (Visual Studio 2019 or Xcode)
5. Build generated project with your IDE
