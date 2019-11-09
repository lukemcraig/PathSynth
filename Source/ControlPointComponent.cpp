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
ControlPointComponent::ControlPointComponent(AudioProcessorValueTreeState& parameters, const int index, int radius) :
    parameters(parameters), index(index), radius(radius)
{
    constrainer.setMinimumOnscreenAmounts(radius, radius,
                                          radius, radius);
}

ControlPointComponent::~ControlPointComponent()
{
}

void ControlPointComponent::paint(Graphics& g)
{
    g.fillAll(Colour(0xaaffffff));
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
    dragger.dragComponent(this, event, &constrainer);

    const auto parentComponent = getParentComponent();
    const auto newPosition = event.getEventRelativeTo(parentComponent).getPosition();
    const auto newPositionX = static_cast<float>(newPosition.getX()) / static_cast<float>(parentComponent->getWidth());
    const auto newPositionY = static_cast<float>(newPosition.getY()) / static_cast<float>(parentComponent->getHeight());

    if (auto* p = parameters.getParameter("point" + String(index) + "x"))
    {
        if (p->getValue() != newPositionX)
            p->setValueNotifyingHost(newPositionX);
    }
    if (auto* p = parameters.getParameter("point" + String(index) + "y"))
    {
        if (p->getValue() != newPositionY)
            p->setValueNotifyingHost(newPositionY);
    }
}
