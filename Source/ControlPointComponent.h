/*
  ==============================================================================

    ControlPointComponent.h
    Created: 4 Nov 2019 10:53:48am
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class ControlPointComponent : public Component
{
public:
    ControlPointComponent(AudioProcessorValueTreeState&, int, int);

    ~ControlPointComponent();

    void paint(Graphics&) override;

    void resized() override;

    void mouseDown(const MouseEvent& event) override;

    void mouseDrag(const MouseEvent& event) override;

private:
    AudioProcessorValueTreeState& parameters;

    int index;
    int radius;

    ComponentDragger dragger;
    ComponentBoundsConstrainer constrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlPointComponent)
};
