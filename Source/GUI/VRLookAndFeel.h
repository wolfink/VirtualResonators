#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../VirtualResonators.h"

class VRLookAndFeel : public LookAndFeel_V4
{
public:
  VRLookAndFeel() {
    ColourScheme cs(
      Colour(0x76, 0x32, 0x3f), // windowBackground
      Colour(0x33, 0x33, 0x33), // widgetBackground
      Colour(0x2A, 0x60, 0x3F), // menuBackground
      Colours::linen,           // outline
      Colours::linen,           // defaultText
      Colour(0x33, 0x33, 0x33), // defaultFill
      Colours::linen,           // highlightedText
      Colours::peru,            // highlightedFill
      Colours::linen            // menuText
    );
  
    setColourScheme(cs);
  }
  void drawRotarySlider(Graphics &,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPosProportional,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        Slider &) override;

  void drawLinearSlider(Graphics &,
                        int x,
                        int y,
                        int width,
                        int height,
                        float sliderPos,
                        float minSliderPos,
                        float maxSliderPos,
                        const Slider::SliderStyle,
                        Slider &) override;

  void drawComboBox(Graphics &,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    ComboBox &) override;
};
