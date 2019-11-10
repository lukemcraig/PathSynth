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
    envelope.setParameters(envParams);
}

bool PathVoice::canPlaySound(SynthesiserSound* sound)
{
    return dynamic_cast<PathSound*>(sound) != nullptr;
}

void PathVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    DBG("startNote");
    auto frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    phaseIncrement = frequency / getSampleRate(); //todo * oversampleFactor
    level = velocity * 0.15f;
    //DBG(level);
    envelope.noteOn();
}

void PathVoice::stopNote(float velocity, bool allowTailOff)
{
    DBG(velocity);
    if (allowTailOff)
    {
        DBG("stopNote allowTailOff");
        envelope.noteOff();
    }
    else
    {
        DBG("stopNote");
        clearCurrentNote();
        phaseIncrement = 0.0f;
        envelope.reset();
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

        for (auto sample = startSample; sample < numSamples; ++sample)
        {
            if (envelope.isActive())
            {
                const auto envValue = envelope.getNextSample();
                if (envValue != 0.0f)
                {
                    const auto value = getNextSample(length, direction);
                    channelData[sample] += value * level * envValue;
                }
            }
            else
            {
                DBG("renderNextBlock clearCurrentNote");
                clearCurrentNote();
                phaseIncrement = 0.0f;
                break;
            }
        }
    }
}

void PathVoice::setCurrentPlaybackSampleRate(double newRate)
{
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    if (newRate != 0)
        envelope.setSampleRate(newRate);
}
