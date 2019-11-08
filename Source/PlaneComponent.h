/*
  ==============================================================================

    PlaneComponent.h
    Created: 8 Nov 2019 3:13:50pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class PlaneComponent : public Component
{
public:
    PlaneComponent();

    ~PlaneComponent();

    void paint(Graphics&) override;

    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaneComponent)
};
