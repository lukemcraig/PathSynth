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
    PathVoice(AudioProcessorValueTreeState&, ADSR::Parameters&, std::vector<float>&);

    ~PathVoice() = default;

    bool canPlaySound(SynthesiserSound*) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;

    void stopNote(float velocity, bool allowTailOff) override;

    void updatePitchBend(int newPitchWheelValue);

    void updatePhaseIncrement();

    void pitchWheelMoved(int newPitchWheelValue) override;

    void controllerMoved(int controllerNumber, int newControllerValue) override;

    float getNextSample();

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setCurrentPlaybackSampleRate(double newRate) override;

private:
    AudioProcessorValueTreeState& parameters;
    std::vector<float>& wavetable;

    int currentNoteNumber{};
    float pitchBend{};
    float phaseIncrement{};
    float t{};

    float level{};

    ADSR::Parameters& envParams;
    ADSR envelope;

    float prevValue{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathVoice)
};
