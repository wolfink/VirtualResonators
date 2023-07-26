#include "DebugComponent.h"

JUCE_IMPLEMENT_SINGLETON(DebugComponent)

DebugComponent::DebugComponent()
    : audioVisualiserComponent(1)
{
    addAndMakeVisible(audioVisualiserComponent);
}

void DebugComponent::paint (Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (24.0f);
    g.drawFittedText("Debug Panel", 0, 0, getWidth(), _header_height, juce::Justification::centred, 1);
}

void DebugComponent::resized()
{
    int left_margin = 15;
    auto area = getLocalBounds();
    FlexItem::Margin tbx_margin(0, 0, 10, 0);

    area.removeFromTop(_header_height);
    area.removeFromLeft(left_margin);
    auto left_area = area.removeFromLeft(250);
    FlexBox flexBox;
    flexBox.justifyContent = FlexBox::JustifyContent::flexStart;
    flexBox.flexDirection = FlexBox::Direction::column;
    for (auto& item : _display_txt_map) {
      flexBox.items.add(FlexItem(*item.second)
          .withHeight(30)
          .withMargin(tbx_margin));
    }
    flexBox.performLayout(left_area);
    area.removeFromLeft(left_margin);
    audioVisualiserComponent.setBounds(area);
}

void DebugComponent::setNumberBox(String id, double value)
{
    const MessageManagerLock mml;
    if (!mml.lockWasGained()) return;
    if (_display_txt_map.count(id) == 0)
    {
        NumberBox* nb = new NumberBox(id);
        nb->textEditor.setFont(24.0f);
        nb->textEditor.setJustification(Justification::right);
        nb->textEditor.setReadOnly(true);
        addAndMakeVisible(nb);
        _display_txt_map[id] = nb;
    }
    _display_txt_map[id]->textEditor.setText(String(value));
}
