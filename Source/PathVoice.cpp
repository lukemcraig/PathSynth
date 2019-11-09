/*
  ==============================================================================

    PathVoice.cpp
    Created: 9 Nov 2019 11:58:51am
    Author:  Luke

  ==============================================================================
*/

#include "PathVoice.h"
#include "PathSound.h"

PathVoice::PathVoice(AudioProcessorValueTreeState& apvts, Path& pp) : parameters(apvts), processorPath(pp)
{
}

bool PathVoice::canPlaySound(SynthesiserSound* sound)
{
    return dynamic_cast<PathSound*>(sound) != nullptr;
}

void PathVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    auto frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    phaseIncrement = frequency / getSampleRate();//todo * oversampleFactor
}

void PathVoice::stopNote(float velocity, bool allowTailOff)
{
}

void PathVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void PathVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void PathVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    DBG("renderNextBlock");
    const auto length = processorPath.getLength();

    const auto direction = *parameters.getRawParameterValue("direction");

    //const auto frequency = *parameters.getRawParameterValue("frequency");
    //const auto phaseIncrement = frequency / getSampleRate(); 

    auto* channelData = outputBuffer.getWritePointer(0);
    for (auto sample = 0; sample < outputBuffer.getNumSamples(); ++sample)
    {
        const auto point = processorPath.getPointAlongPath(length * t);

        float value;
        if (direction == 0)
            value = point.getX();
        else
            value = point.getY();

        channelData[sample] += value;

        t += phaseIncrement;

        if (t >= 1.0f)
        {
            t -= 1.0f;
            if (t >= 1.0f) { jassertfalse; }
        }
    }
}
