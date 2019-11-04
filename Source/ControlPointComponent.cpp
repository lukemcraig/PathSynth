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
ControlPointComponent::ControlPointComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    DBG("Hi");
}

ControlPointComponent::~ControlPointComponent()
{
}

void ControlPointComponent::paint(Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)); // clear the background
     g.fillAll(Colours::white); 
    //g.setColour (Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour(Colours::white);
    //g.setFont(14.0f);
    //g.drawText("ControlPointComponent", getLocalBounds(),
    //           Justification::centred, true); // draw some placeholder text
}

void ControlPointComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
