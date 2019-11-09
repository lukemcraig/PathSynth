/*
  ==============================================================================

    ControlPointComponent.cpp
    Created: 4 Nov 2019 10:53:48am
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ControlPointComponent.h"

//==============================================================================
ControlPointComponent::
ControlPointComponent(bool& pathChanged, AudioProcessorValueTreeState& parameters, int index) :
    pathChanged(pathChanged), parameters(parameters), index(index)
{
}

ControlPointComponent::~ControlPointComponent()
{
}

void ControlPointComponent::paint(Graphics& g)
{
    g.fillAll(Colours::white);
}

void ControlPointComponent::resized()
{
}

void ControlPointComponent::mouseDown(const MouseEvent& event)
{
    dragger.startDraggingComponent(this, event);
}

void ControlPointComponent::mouseDrag(const MouseEvent& event)
{
    dragger.dragComponent(this, event, nullptr);
    pathChanged = true;

    const auto parentComponent = getParentComponent();
    const auto newPosition = event.getEventRelativeTo(parentComponent).getPosition();
    const auto newPositionX = static_cast<float>(newPosition.getX()) / (parentComponent->getWidth() * 0.5f);
    const auto newPositionY = static_cast<float>(newPosition.getY()) / parentComponent->getHeight();
    //DBG(String(index)+" "+String(newPositionX)+" "+String(newPositionY));
    if (auto* p = parameters.getParameter("point" + String(index) + "x"))
    {
        const float newValue = parameters.getParameterRange("point" + String(index) + "x").convertTo0to1(newPositionX);

        if (p->getValue() != newValue)
            p->setValueNotifyingHost(newValue);
    }
    if (auto* p = parameters.getParameter("point" + String(index) + "y"))
    {
        const float newValue = parameters.getParameterRange("point" + String(index) + "y").convertTo0to1(newPositionY);

        if (p->getValue() != newValue)
            p->setValueNotifyingHost(newValue);
    }
    //*parameters.getRawParameterValue("point" + String(index) + "x");
}
