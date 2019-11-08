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
PlaneComponent::PlaneComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
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
}

void PlaneComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}
