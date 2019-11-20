/*
  ==============================================================================

    PathVoice.cpp
    Created: 9 Nov 2019 11:58:51am
    Author:  Luke

  ==============================================================================
*/

#include "PathVoice.h"
#include "PathSound.h"

PathVoice::PathVoice(AudioProcessorValueTreeState& apvts, Path& pp, ADSR::Parameters& envParams,
                     std::array<float, 128>& wavetable) : parameters(apvts),
                                                          processorPath(pp),
                                                          wavetable(wavetable),
                                                          envParams(envParams)
{
    envelope.setParameters(envParams);
}

bool PathVoice::canPlaySound(SynthesiserSound* sound)
{
    return dynamic_cast<PathSound*>(sound) != nullptr;
}

void PathVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    currentNoteNumber = midiNoteNumber;
    updatePitchBend(currentPitchWheelPosition);

    updatePhaseIncrement();
    level = velocity;
    envelope.noteOn();
}

void PathVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        envelope.noteOff();
    }
    else
    {
        clearCurrentNote();
        phaseIncrement = 0.0f;
        prevValue = 0.0f;
        t = 0.0f;
        envelope.reset();
    }
}

void PathVoice::updatePitchBend(int newPitchWheelValue)
{
    if (newPitchWheelValue == 16383)
        pitchBend = 1.0f;
    else if (newPitchWheelValue == 8192)
        pitchBend = 0.0f;
    else if (newPitchWheelValue == 0)
        pitchBend = -1.0f;
    else
        pitchBend = ((newPitchWheelValue / 16383.0f) * 2.0f) - 1.0f;
}

void PathVoice::updatePhaseIncrement()
{
    const auto frequency = frequencyOfA * std::pow(2.0f, (currentNoteNumber + pitchBend - 69.0f) / 12.0f);
    phaseIncrement = frequency / getSampleRate();
}

void PathVoice::pitchWheelMoved(int newPitchWheelValue)
{
    updatePitchBend(newPitchWheelValue);
    updatePhaseIncrement();
}

void PathVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

float PathVoice::getNextSample(const float length, const float direction)
{
    /*const auto point = processorPath.getPointAlongPath(length * t);

    float value;
    if (direction == 0)
        value = point.getX();
    else
        value = point.getY();*/
    //todo interpolation
    auto waveTablesize = wavetable.size();
    const int i = std::floor(t * waveTablesize);
    auto value = wavetable[i];

    // if the path has duplicate points it sometimes returns nans
    if (std::isnan(value))
    {
        //DBG("nan");
        value = prevValue;
    }
    prevValue = value;

    //float value = std::sin(t * MathConstants<float>::pi);

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
        envelope.setParameters(envParams);

        // todo don't need to do this for every voice
        const auto length = processorPath.getLength();

        const auto direction = *parameters.getRawParameterValue("direction");

        auto* channelData = outputBuffer.getWritePointer(0);

        for (auto sample = startSample; sample < startSample + numSamples; ++sample)
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
                clearCurrentNote();
                phaseIncrement = 0.0f;
                prevValue = 0.0f;
                t = 0.0f;
                envelope.reset();
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
