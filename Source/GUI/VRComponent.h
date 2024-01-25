#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"

struct VRShapeButton : public ShapeButton {
    VRShapeButton() : ShapeButton("Noname", Colours::white, Colours::white, Colours::black) { }
};

template<class ComponentClass = Component>
struct VirtualResonatorsComponent : public ComponentClass
{
    VirtualResonatorsComponent() : ComponentClass() {}

    template<typename CC = ComponentClass,
        typename std::enable_if_t<std::is_same<CC, AudioProcessorEditor>::value, bool> = true>
    VirtualResonatorsComponent(AudioProcessor& p) : AudioProcessorEditor(p) {}

    template<typename CC = ComponentClass,
        typename std::enable_if_t<std::is_same<CC, AudioProcessorEditor>::value, bool> = true>
    VirtualResonatorsComponent(AudioProcessor* p) : AudioProcessorEditor(p) {}

	void configComboBox(ComboBox& box, StringArray items) {
        box.addItemList(items, 1);
        this->addAndMakeVisible(box);
	}

	void configRotarySlider(Slider& slider, float low, float high, float step) {
		slider.setSliderStyle           (Slider::RotaryHorizontalVerticalDrag);
        configSlider(slider, low, high, step);
	}

	void configVertSlider(Slider& slider, float low, float high, float step) {
		slider.setSliderStyle           (Slider::LinearVertical);
        configSlider(slider, low, high, step);
	}

	void configShapeButton(VRShapeButton& button, String name, Path path, Colour color, bool change_color=true) {
        int R     = color.getRed();
        int G     = color.getGreen();
        int B     = color.getBlue();
        if (change_color) {
            Colour over_color(std::min(R + 30, 255), std::min(G + 30, 255), std::min(B + 30, 255));
            Colour down_color(std::max(R - 30, 0), std::max(G - 30, 0), std::max(B - 30, 0));
            button.setColours(color, over_color, down_color);
        } else {
            button.setColours(color, color, color);
        }

        button.setName(name);
        button.setShape(path, false, false, false);
        this->addAndMakeVisible(button);
	}

    void configTextButton(TextButton& button, String text) {
        button.setButtonText(text);
        this->addAndMakeVisible(button);
    }

private:
    void configSlider(Slider& slider, float low, float high, float step) {
		slider.setRange                 (low, high, step);
		slider.setPopupDisplayEnabled   (true, false, this);
		slider.setTextBoxStyle          (Slider::NoTextBox, false, 90, 0);
		this->addAndMakeVisible(slider);
    }
};
