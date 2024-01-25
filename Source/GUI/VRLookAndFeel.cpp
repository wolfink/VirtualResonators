#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "VRLookAndFeel.h"

void VRLookAndFeel::drawRotarySlider(
                      Graphics &g,
                      int x,
                      int y,
                      int width,
                      int height,
                      float sliderPosProportional,
                      float rotaryStartAngle,
                      float rotaryEndAngle,
                      Slider &s)
{
    auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width  * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto widgetBackground = getCurrentColourScheme().getUIColour(ColourScheme::UIColour::widgetBackground);


    // fill
    g.setColour (widgetBackground);
    g.fillEllipse (rx, ry, rw, rw);

    // outline
    g.setColour (juce::Colours::linen);
    g.drawEllipse (rx, ry, rw, rw, 4.0f);

    Path p;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 4.0f;
    p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));

    // pointer
    g.setColour (Colours::linen);
    g.fillPath (p);
}

void VRLookAndFeel::drawLinearSlider(
                      Graphics &g,
                      int x,
                      int y,
                      int width,
                      int height,
                      float sliderPos,
                      float minSliderPos,
                      float maxSliderPos,
                      const Slider::SliderStyle style,
                      Slider &s)
{
    // Colors
    auto widgetBackground = getCurrentColourScheme().getUIColour(ColourScheme::UIColour::widgetBackground);

    auto slider_padding = 0.25 * width;
    auto slider_width = width - 2 * slider_padding;
    auto slider_x = x + slider_padding;
    auto slider_percent = (sliderPos - y) / height;

    Path slider_path;
    slider_path.addRoundedRectangle (slider_x, y, slider_width, height, 20.0);

    auto thumb_padding = 0.05 * width;
    auto thumb_width  = slider_width - 2 * thumb_padding;
    auto thumb_x      = slider_x + thumb_padding;
    auto thumb_height = height * 0.075;
    auto thumb_min_y = y + thumb_padding;
    auto thumb_max_y = y + height - thumb_padding - thumb_height;
    auto thumb_y = slider_percent * (thumb_max_y - thumb_min_y) + thumb_min_y;
    auto thumb_bar_thickness = 2.0;

    Path thumb_path;
    thumb_path.addRoundedRectangle (thumb_x, thumb_y, thumb_width, thumb_height, 15.0);

    Path thumb_bar;
    thumb_bar.addRectangle(thumb_x + 3.0, thumb_y + thumb_height * 0.5, thumb_width - 6.0, thumb_bar_thickness);

    auto bar_padding = 0.2 * width;
    auto bar_width   = slider_width - 2 * bar_padding;
    auto bar_x       = slider_x + bar_padding;
    auto bar_height  = height - thumb_y;
    auto bar_y       = thumb_y;

    Path bar_path;
    bar_path.addRoundedRectangle(bar_x, bar_y, bar_width, bar_height, 5.0);

    g.setColour(Colours::linen);
    g.fillPath(slider_path);

    //g.setColour(Colours::lightslategrey);
    ColourGradient cg(
    Colours::peru, bar_x, 0,
    Colours::palegoldenrod, bar_x, thumb_max_y,
    false);
    g.setGradientFill(cg);
    g.fillPath(bar_path);

    g.setColour(widgetBackground);
    g.fillPath(thumb_path);

    g.setColour(Colours::linen);
    g.fillPath(thumb_bar);
}

void VRLookAndFeel::drawComboBox(
                  Graphics &g,
                  int width,
                  int height,
                  bool isButtonDown,
                  int buttonX,
                  int buttonY,
                  int buttonW,
                  int buttonH,
                  ComboBox &c)
{
    // Colors
    auto widgetBackground = getCurrentColourScheme().getUIColour(ColourScheme::UIColour::widgetBackground);

    Path box;
    auto outln_w   = 2.0;
    auto box_x     = buttonX + buttonW - width + outln_w;
    auto box_y     = buttonY + outln_w;
    auto box_w     = width - 2 * outln_w;
    auto box_h     = height - 2 * outln_w;
    auto box_round = 10.0;
    box.addRoundedRectangle(box_x, box_y, box_w, box_h, box_round);

    // box outline
    g.setColour (juce::Colours::linen);
    g.drawRoundedRectangle (box_x, box_y, box_w, box_h, box_round, outln_w);

    Path icon;
    auto icon_width = 8.0;
    auto icon_height = 8.0;
    auto icon_center_x = buttonX + buttonW * 0.5;
    auto icon_center_y = buttonY + buttonH * 0.5;
    auto icon_x1 = icon_center_x - icon_width * 0.5;
    auto icon_y1 = icon_center_y - icon_height * 0.5;
    auto icon_x2 = icon_center_x + icon_height * 0.5;
    auto icon_y2 = icon_center_y - icon_height * 0.5;
    auto icon_x3 = icon_center_x;
    auto icon_y3 = icon_center_y + icon_height * 0.5;
    icon.addTriangle(icon_x1, icon_y1, icon_x2, icon_y2, icon_x3, icon_y3);

    g.setColour(widgetBackground);
    g.fillPath(box);

    g.setColour(Colours::linen);
    g.fillPath(icon);

}
