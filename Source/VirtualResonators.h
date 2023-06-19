/*
  ==============================================================================

    ResonatorPlugin.h
    Created: 2 Dec 2022 3:15:39pm
    Author:  wolfi

  ==============================================================================
*/

#pragma once

template<typename T>
using uptr = std::unique_ptr<T>;

using namespace juce;

#define LOOP(num) for(int _ = 0; _ < num; _++)

#ifdef _WIN32
    #define SLASH "\\"
#else
    #define SLASH "/"
#endif
