#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>("frequency",
                                                           "Frequency",
                                                           NormalisableRange<float>(1.0f,
                                                                                    20000.0f,
                                                                                    0.0f,
                                                                                    0.5f,
                                                                                    false),
                                                           100.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("smoothing",
                                                           "Smoothing",
                                                           NormalisableRange<float>(0.0f,
                                                                                    200.0f,
                                                                                    0.0f,
                                                                                    0.9f,
                                                                                    false),
                                                           100.0f));
    params.push_back(std::make_unique<AudioParameterChoice>("direction",
                                                            "Direction",
                                                            StringArray{"X", "Y"},
                                                            0));
    return {params.begin(), params.end()};
}

//==============================================================================
PathSynthAudioProcessor::PathSynthAudioProcessor(): AudioProcessor(
                                                        BusesProperties().withOutput("Output",
                                                                                     AudioChannelSet::stereo(),
                                                                                     true)),
                                                    parameters(*this,
                                                               nullptr,
                                                               "PathSynth",
                                                               createParameterLayout())

{
}

PathSynthAudioProcessor::~PathSynthAudioProcessor()
{
}

//==============================================================================
const String PathSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PathSynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PathSynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PathSynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PathSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PathSynthAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int PathSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PathSynthAudioProcessor::setCurrentProgram(int index)
{
}

const String PathSynthAudioProcessor::getProgramName(int index)
{
    return {};
}

void PathSynthAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void PathSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PathSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PathSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void PathSynthAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const auto frequency = *parameters.getRawParameterValue("frequency");
    const auto phaseIncrement = frequency / getSampleRate();

    auto* channelData = buffer.getWritePointer(0);
    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (!guiUpdatingPath)
        {
            processorUpdatingPath = true;
            if (processorNeedNewPath)
            {
                processorPath = nextProcessorPath;
                processorNeedNewPath = false;
            }
            processorUpdatingPath = false;
        }

        const auto length = processorPath.getLength();

        const auto point = processorPath.getPointAlongPath(length * t);

        channelData[sample] = point.getX();
        t += phaseIncrement;

        if (t >= 1.0f)
        {
            t -= 1.0f;
        }
    }

    for (auto i = 1; i < totalNumOutputChannels; ++i)
        buffer.copyFrom(i, 0, buffer, 0, 0, buffer.getNumSamples());
}

//==============================================================================
bool PathSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PathSynthAudioProcessor::createEditor()
{
    return new PathSynthAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void PathSynthAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PathSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

bool PathSynthAudioProcessor::setPath(const Path& path)
{
    // if the audio thread is copying from the last nextProcessorPath, this new path will get skipped but that's ok.
    if (!processorUpdatingPath)
    {
        guiUpdatingPath = true;
        nextProcessorPath = path;
        processorNeedNewPath = true;
        guiUpdatingPath = false;
        return true;
    }
    return false;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PathSynthAudioProcessor();
}
