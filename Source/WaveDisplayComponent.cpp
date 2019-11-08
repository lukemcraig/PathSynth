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
}

void WaveDisplayComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}
