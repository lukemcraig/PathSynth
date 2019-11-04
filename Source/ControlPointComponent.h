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
    ControlPointComponent();

    ~ControlPointComponent();

    void paint(Graphics&) override;

    void resized() override;

       void mouseDown(const MouseEvent& event) override
   { 
        dragger.startDraggingComponent(this, event);
   }
   void mouseDrag(const MouseEvent& event) override
   {
        dragger.dragComponent(this, event, nullptr);
   }

private:
    ComponentDragger dragger;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlPointComponent)
};
