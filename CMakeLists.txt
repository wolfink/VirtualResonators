cmake_minimum_required(VERSION 3.16)
project(VirtualResonators)

#find_package(JUCE CONFIG REQUIRED)

set(VIRTUALRESONATORS_VERSION "0.4.0")
set(VIRTUALRESONATORS_COMPANY_NAME "wolfink")
set(VIRTUALRESONATORS_COMPANY_WEBSITE "github.com/wolfink/VirtualResonators")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source)

#include submodule libraries
add_subdirectory(Modules)

file(GLOB VirtualResonatorSources
  ${SRC_DIR}/Debug/*.h
  ${SRC_DIR}/Debug/*.cpp
  ${SRC_DIR}/*.cpp
  ${SRC_DIR}/*.h
)

juce_add_plugin(VirtualResonators
  PRODUCT_NAME    "Virtual Resonators"
  VERSION         ${VIRTUALRESONATORS_VERSION}
  FORMATS         VST3
  VST3_CATEGORIES Modulation,Reverb)

target_sources(VirtualResonators PRIVATE ${VirtualResonatorSources})

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX "Source Files" FILES ${VirutalResonatorSources})

set(libs
  juce::juce_audio_basics
  juce::juce_audio_devices
  juce::juce_audio_formats
  juce::juce_audio_plugin_client
  juce::juce_audio_processors
  juce::juce_audio_utils
  juce::juce_core
  juce::juce_data_structures
  juce::juce_events
  juce::juce_graphics
  juce::juce_gui_basics
  juce::juce_gui_extra
  juce::juce_dsp)

set(PLUGIN_COMPILE_DEFINITIONS
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0)

list(APPEND libs)

target_compile_definitions(VirtualResonators PUBLIC ${PLUGIN_COMPILE_DEFINITIONS})
target_link_libraries(VirtualResonators PRIVATE ${libs} juce::juce_recommended_config_flags)

execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
)
