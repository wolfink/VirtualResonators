#pragma once

#include "VRComponent.h"
#include "VRLookAndFeel.h"
#include "../VirtualResonators.h"

class ResonatorToggle : public ShapeButton
{
    //uptr<ShapeButton> _btn;
    VRLookAndFeel _look_and_feel;

public:
    ResonatorToggle(int index) :
        _look_and_feel(),
        ShapeButton(
            TOGGLE_NAME(index),
            Colours::linen,
            Colours::linen,
            Colours::linen)
    {
        Path btn_shape;
        btn_shape.addEllipse(40, 40, 40, 40);
        setShape(btn_shape, false, false, false);
        // setOnColours(Colours::peru, Colours::peru, Colours::peru);
        // shouldUseOnColours     (true);
        setToggleable          (true);
        setClickingTogglesState(true);
        setToggleState         (true, NotificationType::dontSendNotification);
    }

    void paintButton(Graphics& g,
                     bool shouldDrawButtonAsHighlighted,
                     bool shouldDrawButtonAsDown) override
    {
        auto x = getLocalBounds().getX();
        auto y = getLocalBounds().getY();
        auto w = getLocalBounds().getWidth();
        auto h = getLocalBounds().getHeight();

        auto icon_color    = _look_and_feel
            .getCurrentColourScheme()
            .getUIColour(VRLookAndFeel::ColourScheme::UIColour::outline);
        auto btn_on_color  = Colours::peru;
        auto btn_off_color = _look_and_feel
            .getCurrentColourScheme()
            .getUIColour(VRLookAndFeel::ColourScheme::UIColour::widgetBackground);

        Path icon;
        auto line_thickness = 2.0;
        auto btn_radius = 50;
        auto icon_radius = 10;
        auto center_x = (x + w) * 0.5;
        auto center_y = (y + h) * 0.5;
        auto start_angle = -MathConstants<double>::pi / 3;
        auto end_angle   = 4 * MathConstants<double>::pi / 3;

        for (int i = 0; i < 401; i++)
        {
            auto start_x = center_x + icon_radius * cos(((float) i / 400) * (end_angle - start_angle) + start_angle);
            auto start_y = center_y + icon_radius * sin(((float) i / 400) * (end_angle - start_angle) + start_angle);
            auto end_x = center_x + icon_radius * cos(((float) (i + 1) / 400) * (end_angle - start_angle) + start_angle);
            auto end_y = center_y + icon_radius * sin(((float) (i + 1) / 400) * (end_angle - start_angle) + start_angle);
            icon.addLineSegment(Line<float>(start_x, start_y, end_x, end_y), line_thickness);
        }
        icon.addLineSegment(Line<float>(center_x, center_y - icon_radius, center_x, center_y), line_thickness);

        if (getToggleState()) g.setColour(btn_on_color);
        else g.setColour(btn_off_color);

        g.fillEllipse(center_x - btn_radius * 0.5, center_y - btn_radius * 0.5, btn_radius, btn_radius);
        g.setColour(icon_color);
        g.fillPath(icon);
    }
    void resized() override
    {
    }
};
