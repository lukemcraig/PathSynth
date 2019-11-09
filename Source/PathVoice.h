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
    PathVoice(AudioProcessorValueTreeState&, Path&);

    ~PathVoice() = default;

    bool canPlaySound(SynthesiserSound*) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    AudioProcessorValueTreeState& parameters;
    Path& processorPath;

    float t{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathVoice)
};
