#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "../VirtualResonators.h"

class DebugComponent : public Component
{
private:
    class NumberBox : public Component
    {
    public:
        Label label;
        TextEditor textEditor;
        NumberBox(String name) : label(name, name)
        {
            addAndMakeVisible(label);
            addAndMakeVisible(textEditor);
        }
        void resized() override
        {
            FlexBox flexBox;
            flexBox.items.add(FlexItem(label).withFlex(1));
            flexBox.items.add(FlexItem(textEditor).withWidth(150));
            flexBox.performLayout(getLocalBounds());
        }
    };

    std::map<String, NumberBox*> _display_txt_map;
    const int _header_height = 40;

public:
    AudioVisualiserComponent audioVisualiserComponent;

    // Inherited from Component
    void paint (Graphics &g) override;
    void resized() override;

    void pushBuffer(const float* const* channelData, int numChannels, int numSamples);
    void setNumberBox(String key, double value);

    JUCE_DECLARE_SINGLETON(DebugComponent, true)

private:
    DebugComponent();
    ~DebugComponent() { clearSingletonInstance(); }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DebugComponent)
};
