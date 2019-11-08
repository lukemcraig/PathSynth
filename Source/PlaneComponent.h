/*
  ==============================================================================

    PlaneComponent.h
    Created: 8 Nov 2019 3:13:50pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ControlPointComponent.h"

//==============================================================================
/*
*/
class PlaneComponent : public Component
{
public:
    PlaneComponent(AudioProcessorValueTreeState&);

    ~PlaneComponent();

    void paint(Graphics&) override;

    void resized() override;

    Path update();

private:
    AudioProcessorValueTreeState& parameters;

    std::vector<std::unique_ptr<ControlPointComponent>> controlPoints;
    Path straightPath;
    Path smoothPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaneComponent)
};
