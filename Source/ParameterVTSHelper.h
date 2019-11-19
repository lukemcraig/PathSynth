/*
  ==============================================================================

    ParameterVTSHelper.h
    Created: 18 Nov 2019 10:31:40pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PathSynthConstants.h"

class ParameterVTSHelper
{
public:
    ParameterVTSHelper(AudioProcessorValueTreeState&);

    ~ParameterVTSHelper() = default;

    //==============================================================================
    void resetSmoothers(double sampleRate);

    /**
    * \brief setCurrentAndTargetValue for all the smoothers. 
    * Called from the constructor and from setStateInformation
    */
    void instantlyUpdateSmoothers();

    void updateSmoothers();

    //==============================================================================

    float getPointX(int pointNum) { return smoothPointsX[pointNum].getNextValue(); }

    float getPointY(int pointNum) { return smoothPointsY[pointNum].getNextValue(); }

    //==============================================================================

    float getPointXSkip(int pointNum, int numSamples) { return smoothPointsX[pointNum].skip(numSamples); }

    float getPointYSkip(int pointNum, int numSamples) { return smoothPointsY[pointNum].skip(numSamples); }

private:
    AudioProcessorValueTreeState& parameters;

    typedef SmoothedValue<float, ValueSmoothingTypes::Linear> SmoothFloat;
    std::array<SmoothFloat, PathSynthConstants::numControlPoints> smoothPointsX{};
    std::array<SmoothFloat, PathSynthConstants::numControlPoints> smoothPointsY{};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterVTSHelper)
};
