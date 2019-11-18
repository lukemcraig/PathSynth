/*
  ==============================================================================

    DcBlocker.cpp
    Created: 18 Nov 2019 3:47:41pm
    Author:  Luke

  ==============================================================================
*/

#include "DcBlocker.h"

DcBlocker::DcBlocker(float R): R(R)
{
}

/**
 * \see https://ccrma.stanford.edu/~jos/filters/DC_Blocker.html
 * \param x the new sample
 * \return the filtered sample
 */
float DcBlocker::pushSample(float x)
{
    const float y = x - previousInput + (R * previousOutput);
    previousInput = x;
    previousOutput = y;
    return y;
}

void DcBlocker::reset()
{
    previousInput = 0.0f;
    previousOutput = 0.0f;
}
