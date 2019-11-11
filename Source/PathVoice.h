/*
  ==============================================================================

    PathVoice.h
    Created: 9 Nov 2019 11:58:51am
    Author:  Luke

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class PathVoice : public SynthesiserVoice
{
public:
    PathVoice(AudioProcessorValueTreeState&, Path&, ADSR::Parameters&);

    ~PathVoice() = default;

    bool canPlaySound(SynthesiserSound*) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    float getNextSample(float length, float direction);

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setCurrentPlaybackSampleRate(double newRate) override;

private:
    AudioProcessorValueTreeState& parameters;
    Path& processorPath;

    float phaseIncrement{};
    float t{};
    float level{};

    ADSR::Parameters& envParams;
    ADSR envelope;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathVoice)
};
