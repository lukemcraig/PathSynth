/*
  ==============================================================================

    ParameterVTSHelper.cpp
    Created: 18 Nov 2019 10:31:40pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#include "ParameterVTSHelper.h"

ParameterVTSHelper::ParameterVTSHelper(AudioProcessorValueTreeState& p) : parameters(p)
{
    instantlyUpdateSmoothers();
}

void ParameterVTSHelper::resetSmoothers(double sampleRate)
{
    for (auto& smoother : smoothPointsX)
        smoother.reset(sampleRate, 0.01);

    for (auto& smoother : smoothPointsY)
        smoother.reset(sampleRate, 0.01);
}

void ParameterVTSHelper::instantlyUpdateSmoothers()
{
    for (auto i = 0; i < PathSynthConstants::numControlPoints; ++i)
    {
        smoothPointsX[i].setCurrentAndTargetValue(*parameters.getRawParameterValue("point" + String(i) + "x"));
        smoothPointsY[i].setCurrentAndTargetValue(*parameters.getRawParameterValue("point" + String(i) + "y"));
    }
}

void ParameterVTSHelper::updateSmoothers()
{
    for (auto i = 0; i < PathSynthConstants::numControlPoints; ++i)
    {
        smoothPointsX[i].setTargetValue(*parameters.getRawParameterValue("point" + String(i) + "x"));
        smoothPointsY[i].setTargetValue(*parameters.getRawParameterValue("point" + String(i) + "y"));
    }
}
