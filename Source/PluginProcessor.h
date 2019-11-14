#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "hiir/Downsampler2xFpu.h"

//==============================================================================
/**
*/
class PathSynthAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================
    PathSynthAudioProcessor();

    ~PathSynthAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const String getProgramName(int index) override;

    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    void setNumVoices(int newNumVoices);

    int getNumVoices();

    void setOversampleFactor(int newOversampleFactor);

    int getOversampleFactor() const { return oversampleFactor; }

    //==============================================================================
private:
    AudioProcessorValueTreeState parameters;
    MidiKeyboardState keyboardState;

    Path straightPath{};
    Path processorPath{};

    static constexpr int numCoeffs{6};

    hiir::Downsampler2xFpu<numCoeffs> downsampler;
    hiir::Downsampler2xFpu<numCoeffs> downsampler2;
    static constexpr int maxOversampleFactor{4};
    int oversampleFactor{4};
    AudioBuffer<float> oversampledBuffer;

    int numVoices{10};
    ADSR::Parameters envParams;
    Synthesiser synthesiser;

    //==============================================================================
    void setPath();

    void updateEnvParams();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathSynthAudioProcessor)
};
