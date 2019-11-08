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
    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = (*parameters.getRawParameterValue("point" + String(i) + "x") * 512) + 256.0f;
        auto y = (*parameters.getRawParameterValue("point" + String(i) + "y") * 512) + 256.0f;
        controlPoints[i]->setBounds(x, y, 10.0f, 10.0f);
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

    g.drawLine(0, getHeight() * 0.5f, getWidth(), getHeight() * 0.5f);
    g.drawLine(getWidth() * 0.5f, 0, getWidth() * 0.5f, getHeight());

    g.strokePath(straightPath, PathStrokeType(1.0));

    g.setColour(Colours::white);

    g.strokePath(smoothPath, PathStrokeType(1.0));
}

void PlaneComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

Path PlaneComponent::update()
{
    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = (*parameters.getRawParameterValue("point" + String(i) + "x") * 512.0f);
        auto y = (*parameters.getRawParameterValue("point" + String(i) + "y") * 512.0f);
        controlPoints[i]->setBounds(x, y, 10.0f, 10.0f);
    }
    
    const auto smoothing = *parameters.getRawParameterValue("smoothing") * 200.0f;
  

    straightPath.clear();

    // TODO maybe just make this use the control points instead of the parameters?
    const Point<float> firstPointPos{
        *parameters.getRawParameterValue("point0x") * 512.0f, *parameters.getRawParameterValue("point0y") * 512.0f
    };
    straightPath.startNewSubPath(firstPointPos);

    for (auto i = 1; i < 8; ++i)
    {
        const Point<float> pointPos{
            *parameters.getRawParameterValue("point" + String(i) + "x") * 512.0f,
            *parameters.getRawParameterValue("point" + String(i) + "y") * 512.0f
        };
        straightPath.lineTo(pointPos);
    }
    straightPath.closeSubPath();

    smoothPath = straightPath.createPathWithRoundedCorners(smoothing);

    float dashes[] = {2.0f, 3.0f, 4.0f, 5.0f};
    PathStrokeType(1.0).createDashedStroke(straightPath, straightPath, dashes, 4);
    return smoothPath;
}
