/*
  ==============================================================================

    WaveDisplayComponent.h
    Created: 8 Nov 2019 3:15:33pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class WaveDisplayComponent : public Component
{
public:
    WaveDisplayComponent();

    ~WaveDisplayComponent();

    void paint(Graphics&) override;

    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveDisplayComponent)
};
