#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../VirtualResonators.h"
#include "VRLookAndFeel.h"

class PluckButton : public ShapeButton
{

    VRLookAndFeel _look_and_feel;
    int frame;

public:
    PluckButton(int index) :
        ShapeButton(
            "Pluck" + std::to_string(index),
            Colours::black,
            Colours::black,
            Colours::black)
    {
        frame = 0;
    }

    void paintButton(Graphics& g,
                     bool shouldDrawButtonAsHighlighted,
                     bool shouldDrawButtonAsDown) override
    {
        const auto w = getBounds().getWidth();
        const auto h = getBounds().getHeight();
        const auto line_thickness = 3.0;
        const auto line_len = 40.0;

        const auto btn_width = 80.0;
        const auto btn_height = 60.0;

        const auto line_color = _look_and_feel
            .getCurrentColourScheme()
            .getUIColour(VRLookAndFeel::ColourScheme::UIColour::defaultFill);
        const auto btn_color  = Colours::linen;

        Path line_path;
        line_path.addRectangle(
            (w - line_thickness) * 0.5,
            (h - line_len) * 0.5,
            line_thickness,
            line_len
        );

        auto pick_w = 10.0;
        auto pick_h = 14.0;
        auto pick_dist = 15.0;
        if (shouldDrawButtonAsHighlighted) pick_dist = 25.0;
        if (shouldDrawButtonAsDown)   pick_dist = -15.0;

        auto pick_x = w * 0.5 - pick_dist;
        auto pick_center_x = pick_x + pick_w * 0.5;
        auto pick_y = h * 0.5 - pick_h * 0.5;
        auto pick_top_size = pick_h * 0.3;


        Path pick_path;
        // pick_path.addEllipse(pick_x, pick_y, pick_size, pick_top_size);
        pick_path.addArc(pick_x,
                         pick_y + pick_top_size,
                         pick_w,
                         pick_top_size,
                         -MathConstants<double>::pi * 0.5,
                         MathConstants<double>::pi * 0.5,
                         true);
        pick_path.lineTo(pick_x + pick_w * 0.5, pick_y + pick_h);
        // pick_path.lineTo(pick_x + pick_size, pick_y + pick_top_size);
        pick_path.closeSubPath();
        // pick_path.addTriangle(pick_x, pick_y + pick_top_size,
        //                       pick_center_x, pick_y + pick_size,
        //                       pick_x + pick_size, pick_y + pick_top_size);

        Path btn_path;
        btn_path.addRoundedRectangle(
            (w - btn_width) * 0.5,
            (h - btn_height) * 0.5,
            btn_width,
            btn_height,
            25.0
        );

        g.setColour(btn_color);
        g.fillPath(btn_path);

        g.setColour(line_color);
        g.fillPath(line_path);

        g.setColour(line_color);
        g.fillPath(pick_path);
    }
};
