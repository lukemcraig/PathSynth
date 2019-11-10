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
    phaseIncrement = frequency / getSampleRate(); //todo * oversampleFactor
    level = velocity * 0.15f;
    tailOff = 0.0f;
}

void PathVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        if (tailOff == 0.0f)
            tailOff = 1.0f;
    }
    else
    {
        clearCurrentNote();
        phaseIncrement = 0.0f;
    }
}

void PathVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void PathVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

float PathVoice::getNextSample(const float length, const float direction)
{
    const auto point = processorPath.getPointAlongPath(length * t);
    float value;
    if (direction == 0)
        value = point.getX();
    else
        value = point.getY();

    t += phaseIncrement;

    if (t >= 1.0f)
    {
        t -= 1.0f;
        if (t >= 1.0f) { jassertfalse; }
    }
    return value;
}

void PathVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (phaseIncrement != 0.0f)
    {
        const auto length = processorPath.getLength();

        const auto direction = *parameters.getRawParameterValue("direction");

        auto* channelData = outputBuffer.getWritePointer(0);
        if (tailOff > 0.0f)
        {
            for (auto sample = startSample; sample < numSamples; ++sample)
            {
                const auto value = getNextSample(length, direction);

                channelData[sample] += value * level * tailOff;

                tailOff *= 0.5f;

                if (tailOff <= 0.005f)
                {
                    clearCurrentNote();

                    phaseIncrement = 0.0f;
                    break;
                }
            }
        }
        else
        {
            for (auto sample = startSample; sample < numSamples; ++sample)
            {
                const auto value = getNextSample(length, direction);

                channelData[sample] += value * level;
            }
        }
    }
}
