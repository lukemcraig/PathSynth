/*
  ==============================================================================

    WaveDisplayComponent.cpp
    Created: 8 Nov 2019 3:15:33pm
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveDisplayComponent.h"

//==============================================================================
WaveDisplayComponent::WaveDisplayComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}

WaveDisplayComponent::~WaveDisplayComponent()
{
}

void WaveDisplayComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)); // clear the background

    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1); // draw an outline around the component

    g.drawLine(0, getHeight() * 0.5f, getWidth(), getHeight() * 0.5f);

    g.setColour(Colours::white);
    g.strokePath(signalPath, PathStrokeType(1.0));
}

void WaveDisplayComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void WaveDisplayComponent::update(Path smoothPath, const int direction)
{
    const auto newPathBounds = smoothPath.getBounds();

    smoothPath.applyTransform(
        AffineTransform::translation(-newPathBounds.getCentreX(), -newPathBounds.getCentreY()).followedBy(
            AffineTransform::scale(1.0f / newPathBounds.getWidth(), 1.0f / newPathBounds.getHeight())));

    const auto length = smoothPath.getLength();
    float t = 0.0f;
    signalPath.clear();

    for (int i = 1; i <= getWidth(); ++i)
    {
        t = static_cast<float>(i) / getWidth();
        const auto point = smoothPath.getPointAlongPath(length * t);
        float pointValue;
        if (direction == 0)
            pointValue = point.getX();
        else
            pointValue = point.getY();
        pointValue = (pointValue * getHeight() * 0.5f) + (getHeight() * 0.5f);
        if (i == 1)
            signalPath.startNewSubPath(0, pointValue);
        else
            signalPath.lineTo(i, pointValue);
    }
}
