/*
  ==============================================================================

    PlaneComponent.cpp
    Created: 8 Nov 2019 3:13:50pm
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlaneComponent.h"
#include "PathSynthConstants.h"

//==============================================================================
PlaneComponent::PlaneComponent(AudioProcessorValueTreeState& parameters) : parameters(parameters)
{
    for (auto i = 0; i < PathSynthConstants::numControlPoints; ++i)
    {
        controlPoints.emplace_back(std::make_unique<ControlPointComponent>(parameters, i, controlPointRadius));
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

    g.setColour(Colour(0xff513a1d));
    g.drawRect(getLocalBounds(), 1); // draw an outline around the component

    const auto width = static_cast<float>(getWidth());
    const auto height = static_cast<float>(getHeight());

    const auto halfWidth = width * 0.5f;
    const auto halfHeight = height * 0.5f;

    // draw x axis
    g.drawLine(0, halfHeight, width, halfHeight);

    // draw y axis
    g.drawLine(halfWidth, 0, halfWidth, height);

    // draw dashed straight path
    g.strokePath(straightPath, PathStrokeType(1.0));

    // draw solid smooth path
    g.setColour(Colour(0xfff4e9d8));
    g.strokePath(smoothPath, PathStrokeType(1.0));
}

void PlaneComponent::resized()
{
    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = *parameters.getRawParameterValue("point" + String(i) + "x");
        auto y = *parameters.getRawParameterValue("point" + String(i) + "y");

        x = (x + 1.0f) * 0.5f;
        y = (y + 1.0f) * 0.5f;

        x *= static_cast<float>(getWidth());
        y *= static_cast<float>(getHeight());

        controlPoints[i]->setBounds(x - controlPointRadius,
                                    y - controlPointRadius,
                                    controlPointRadius * 2,
                                    controlPointRadius * 2);
    }
}

Path PlaneComponent::update()
{
    const auto smoothing = *parameters.getRawParameterValue("smoothing") * 200.0f;

    straightPath.clear();

    const auto firstPointBounds = controlPoints[0]->getBounds();
    auto firstPointPosition = firstPointBounds.getPosition().toFloat();
    firstPointPosition.setX(firstPointPosition.getX() + (firstPointBounds.getWidth() * 0.5f));
    firstPointPosition.setY(firstPointPosition.getY() + (firstPointBounds.getHeight() * 0.5f));
    straightPath.startNewSubPath(firstPointPosition);

    for (auto i = 1; i < controlPoints.size(); ++i)
    {
        const auto pointBounds = controlPoints[i]->getBounds();
        auto pointPosition = pointBounds.getPosition().toFloat();
        pointPosition.setX(pointPosition.getX() + (firstPointBounds.getWidth() * 0.5f));
        pointPosition.setY(pointPosition.getY() + (firstPointBounds.getHeight() * 0.5f));
        straightPath.lineTo(pointPosition);
    }
    straightPath.closeSubPath();

    smoothPath = straightPath.createPathWithRoundedCorners(smoothing);

    float dashes[] = {2.0f, 3.0f, 4.0f, 5.0f};
    PathStrokeType(1.0).createDashedStroke(straightPath, straightPath, dashes, 4);
    return smoothPath;
}
