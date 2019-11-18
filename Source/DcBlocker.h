/*
  ==============================================================================

    DcBlocker.h
    Created: 18 Nov 2019 3:47:41pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DcBlocker
{
public:
    DcBlocker(float R = 0.995);

    ~DcBlocker() = default;

    float pushSample(float x);

    void reset();

private:
    float R;
    float previousInput{};
    float previousOutput{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DcBlocker)
};
