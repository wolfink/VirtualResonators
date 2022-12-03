/*
  ==============================================================================

    ResonatorPlugin.h
    Created: 2 Dec 2022 3:15:39pm
    Author:  wolfi

  ==============================================================================
*/

#pragma once
using namespace juce;

template<typename T>
using uptr = std::unique_ptr<T>;

#define NUM_IN_CHAN  1
#define NUM_OUT_CHAN 2