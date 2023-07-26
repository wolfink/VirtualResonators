#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include "../VirtualResonators.h"
#include "DebugComponent.h"

class DebugPanel : public DocumentWindow
{
public:
    JUCE_DECLARE_SINGLETON(DebugPanel, true);

    void open() { setVisible(true); }
    void closeButtonPressed() override { setVisible(false); }

private:
    DebugPanel() :
    DocumentWindow("Debug", Colours::black, 0)
    {
        setUsingNativeTitleBar(false);
        setVisible(false);
        setOpaque(true);
        DebugComponent::getInstance()->setSize(200, 300);
        setContentOwned(DebugComponent::getInstance(), false);
        centreWithSize(getWidth(), getHeight());
        setResizable(true, true);
    }
    ~DebugPanel()
    {
        clearSingletonInstance();
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DebugPanel)
};

