/*
  ==============================================================================

    PathSound.h
    Created: 9 Nov 2019 12:11:46pm
    Author:  Luke

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class PathSound : public SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
