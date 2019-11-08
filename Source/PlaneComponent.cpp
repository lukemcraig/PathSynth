/*
  ==============================================================================

    PlaneComponent.cpp
    Created: 8 Nov 2019 3:13:50pm
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlaneComponent.h"

//==============================================================================
PlaneComponent::PlaneComponent(AudioProcessorValueTreeState& parameters) : parameters(parameters)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    for (auto i = 0; i < 8; ++i)
    {
        controlPoints.emplace_back(std::make_unique<ControlPointComponent>(parameters, i));
    }
    for (auto&& controlPoint : controlPoints)
    {
        addAndMakeVisible(controlPoint.get());
    }
}

PlaneComponent::~PlaneComponent()
{
}

void PlaneComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)); // clear the background

    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1); // draw an outline around the component

    // draw axes
    g.drawLine(0, getHeight() * 0.5f, getWidth(), getHeight() * 0.5f);
    g.drawLine(getWidth() * 0.5f, 0, getWidth() * 0.5f, getHeight());

    // draw paths
    g.strokePath(straightPath, PathStrokeType(1.0));

    g.setColour(Colours::white);

    g.strokePath(smoothPath, PathStrokeType(1.0));
}

void PlaneComponent::resized()
{
    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = *parameters.getRawParameterValue("point" + String(i) + "x");
        auto y = *parameters.getRawParameterValue("point" + String(i) + "y");

        x += 1.0f;
        y += 1.0f;

        x *= 0.5f;
        y *= 0.5f;

        x *= getWidth();
        y *= getHeight();

        controlPoints[i]->setBounds(x, y, 10.0f, 10.0f);
    }
}

Path PlaneComponent::update()
{
    const auto smoothing = *parameters.getRawParameterValue("smoothing") * 200.0f;

    straightPath.clear();

    straightPath.startNewSubPath(controlPoints[0]->getPosition().toFloat());

    for (auto i = 1; i < controlPoints.size(); ++i)
    {
        straightPath.lineTo(controlPoints[i]->getPosition().toFloat());
    }
    straightPath.closeSubPath();

    smoothPath = straightPath.createPathWithRoundedCorners(smoothing);

    float dashes[] = {2.0f, 3.0f, 4.0f, 5.0f};
    PathStrokeType(1.0).createDashedStroke(straightPath, straightPath, dashes, 4);
    return smoothPath;
}
