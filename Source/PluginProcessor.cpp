#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PathVoice.h"
#include "PathSound.h"
#include "hiir/PolyphaseIir2Designer.h"
#include "PathSynthConstants.h"

AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>("smoothing",
                                                           "Smoothing",
                                                           NormalisableRange<float>(0.0f,
                                                                                    1.0f,
                                                                                    0.0f,
                                                                                    0.9f,
                                                                                    false),
                                                           100.0f));
    params.push_back(std::make_unique<AudioParameterChoice>("direction",
                                                            "Direction",
                                                            StringArray{"X", "Y"},
                                                            1));
    params.push_back(std::make_unique<AudioParameterFloat>("outgain",
                                                           "Out Gain",
                                                           NormalisableRange<float>(0.0f,
                                                                                    1.0f,
                                                                                    0.0f,
                                                                                    0.5f,
                                                                                    false),
                                                           0.5f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               return String(Decibels::gainToDecibels(value), 2) +
                                                                   " dB";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               return Decibels::decibelsToGain(text.getFloatValue());
                                                           }));
    for (auto i = 0; i < PathSynthConstants::numControlPoints; ++i)
    {
        auto x = std::cos((static_cast<float>(i) / PathSynthConstants::numControlPoints)
            * MathConstants<float>::twoPi) * 0.9f;
        auto y = std::sin((static_cast<float>(i) / PathSynthConstants::numControlPoints)
            * MathConstants<float>::twoPi) * 0.9f;

        params.push_back(std::make_unique<AudioParameterFloat>("point" + String(i) + "x",
                                                               "Point" + String(i) + "_X",
                                                               NormalisableRange<float>(-1.0f,
                                                                                        1.0f,
                                                                                        0.0f,
                                                                                        1.0f,
                                                                                        false),
                                                               x));

        params.push_back(std::make_unique<AudioParameterFloat>("point" + String(i) + "y",
                                                               "Point" + String(i) + "_Y",
                                                               NormalisableRange<float>(-1.0f,
                                                                                        1.0f,
                                                                                        0.0f,
                                                                                        1.0f,
                                                                                        false),
                                                               y));
    }
    params.push_back(std::make_unique<AudioParameterFloat>("attack",
                                                           "Attack",
                                                           NormalisableRange<float>(0.0f,
                                                                                    20000.0f,
                                                                                    0.0f,
                                                                                    0.2f,
                                                                                    false),
                                                           1.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               if (value >= 1000.0f)
                                                                   return String(value / 1000.0f, 2) + " s";
                                                               return String(value, 2) + " ms";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               if (text.endsWith(" s") ||
                                                                   (text.endsWith("s") && !text.endsWith("ms")))
                                                               {
                                                                   return text.getFloatValue() * 1000.0f;
                                                               }
                                                               return text.getFloatValue();
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>("decay",
                                                           "Decay",
                                                           NormalisableRange<float>(1.0f,
                                                                                    60000.0f,
                                                                                    0.0f,
                                                                                    0.2f,
                                                                                    false),
                                                           600.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               if (value >= 1000.0f)
                                                                   return String(value / 1000.0f, 2) + " s";
                                                               return String(value, 2) + " ms";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               if (text.endsWith(" s") ||
                                                                   (text.endsWith("s") && !text.endsWith("ms")))
                                                               {
                                                                   return text.getFloatValue() * 1000.0f;
                                                               }
                                                               return text.getFloatValue();
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>("sustain",
                                                           "Sustain",
                                                           NormalisableRange<float>(0.0f,
                                                                                    1.0,
                                                                                    0.0f,
                                                                                    1.0f,
                                                                                    false),
                                                           1.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               return String(Decibels::gainToDecibels(value), 2) +
                                                                   " dB";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               return Decibels::decibelsToGain(text.getFloatValue());
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>("release",
                                                           "Release",
                                                           NormalisableRange<float>(1.0f,
                                                                                    60000.0f,
                                                                                    0.0f,
                                                                                    0.2f,
                                                                                    false),
                                                           50.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](const float value, int /*maximumStringLength*/)
                                                           {
                                                               if (value >= 1000.0f)
                                                                   return String(value / 1000.0f, 2) + " s";
                                                               return String(value, 2) + " ms";
                                                           },
                                                           [](const String& text)
                                                           {
                                                               if (text.endsWith(" s") ||
                                                                   (text.endsWith("s") && !text.endsWith("ms")))
                                                               {
                                                                   return text.getFloatValue() * 1000.0f;
                                                               }
                                                               return text.getFloatValue();
                                                           }));
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
                                                               createParameterLayout()),
                                                    parameterVtsHelper(parameters)
{
    wavetable.resize(nextWavetableSize);
    for (auto i = 0; i < numVoices; ++i)
    {
        synthesiser.addVoice(new PathVoice(parameters, envParams, wavetable));
    }
    synthesiser.addSound(new PathSound());
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
    synthesiser.setCurrentPlaybackSampleRate(sampleRate * oversampleFactor);

    oversampledBuffer.setSize(1, samplesPerBlock * maxOversampleFactor);
    oversampledBuffer.clear();

    double coefs[numCoeffs]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs, 100.0, 0.1);
    downsampler.set_coefs(coefs);
    downsampler.clear_buffers();

    double coefs2[numCoeffs2]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs2, 100.0, 0.1);
    downsampler2.set_coefs(coefs2);
    downsampler2.clear_buffers();

    double coefs3[numCoeffs3]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs3, 100.0, 0.1);
    downsampler3.set_coefs(coefs3);
    downsampler3.clear_buffers();

    double coefs4[numCoeffs4]{};
    hiir::PolyphaseIir2Designer::compute_coefs(coefs4, 100.0, 0.1);
    downsampler4.set_coefs(coefs4);
    downsampler4.clear_buffers();

    dcBlocker.reset();

    parameterVtsHelper.resetSmoothers(sampleRate);
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

void PathSynthAudioProcessor::updateEnvParams()
{
    envParams.attack = *parameters.getRawParameterValue("attack") / 1000.0f;
    envParams.decay = *parameters.getRawParameterValue("decay") / 1000.0f;
    envParams.sustain = *parameters.getRawParameterValue("sustain");
    envParams.release = *parameters.getRawParameterValue("release") / 1000.0f;
}

void PathSynthAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    parameterVtsHelper.updateSmoothers();

    updateEnvParams();

    buffer.clear(0, 0, buffer.getNumSamples());

    oversampledBuffer.clear(0, 0, oversampledBuffer.getNumSamples());

    {
        int numSamples = samplesPerSubBlock * oversampleFactor;
        int totalSamples = buffer.getNumSamples() * oversampleFactor;
        for (int startSample = 0; startSample < totalSamples; startSample += numSamples)
        {
            setPath(numSamples);

            keyboardState.processNextMidiBuffer(midiMessages, startSample, numSamples, true);
            synthesiser.renderNextBlock(oversampledBuffer, midiMessages, startSample, numSamples);
        }
        // in case the buffer didn't divide evenly. TODO double check this
        const int numSamplesLeft = totalSamples % numSamples;
        if (numSamplesLeft > 0)
        {
            const int startSample = totalSamples - numSamplesLeft;

            //setPath(numSamplesLeft);

            keyboardState.processNextMidiBuffer(midiMessages, startSample, numSamplesLeft, true);
            synthesiser.renderNextBlock(oversampledBuffer, midiMessages, startSample, numSamplesLeft);
        }
    }

    auto bufferWrite = buffer.getWritePointer(0);
    auto oversampleWrite = oversampledBuffer.getWritePointer(0);
    if (oversampleFactor >= 16)
    {
        //16x to 8x
        downsampler4.process_block(oversampleWrite,
                                   oversampleWrite,
                                   buffer.getNumSamples() * 8);
    }
    if (oversampleFactor >= 8)
    {
        //8x to 4x
        downsampler3.process_block(oversampleWrite,
                                   oversampleWrite,
                                   buffer.getNumSamples() * 4);
    }
    if (oversampleFactor >= 4)
    {
        //4x to 2x
        downsampler2.process_block(oversampleWrite,
                                   oversampleWrite,
                                   buffer.getNumSamples() * 2);
    }
    if (oversampleFactor >= 2)
    {
        //2x to 1x
        downsampler.process_block(bufferWrite, // out
                                  oversampleWrite, // in
                                  buffer.getNumSamples());
    }
    else
    {
        //1x to 1x
        buffer.copyFrom(0, 0, oversampledBuffer, 0, 0, buffer.getNumSamples());
    }

    // filter dc offset
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        bufferWrite[i] = dcBlocker.pushSample(bufferWrite[i]);
    }

    // todo smoothing
    buffer.applyGain(0, 0, buffer.getNumSamples(), *parameters.getRawParameterValue("outgain"));

    // copy the processed channel to all the other channels
    for (auto i = 1; i < getTotalNumOutputChannels(); ++i)
        buffer.copyFrom(i, 0, buffer, 0, 0, buffer.getNumSamples());

    midiMessages.clear();
}

//==============================================================================
bool PathSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PathSynthAudioProcessor::createEditor()
{
    return new PathSynthAudioProcessorEditor(*this, parameters, keyboardState);
}

//==============================================================================
void PathSynthAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    const auto state = parameters.copyState();
    const auto xml(state.createXml());
    xml->setAttribute("maxVoices", numVoices);
    xml->setAttribute("oversampleFactor", oversampleFactor);
    xml->setAttribute("wavetableSize", static_cast<int>(wavetable.size()));
    copyXmlToBinary(*xml, destData);
}

void PathSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    const auto xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
    {
        if (xmlState->hasAttribute("maxVoices"))
        {
            setNumVoices(xmlState->getIntAttribute("maxVoices", 10));
            xmlState->removeAttribute("maxVoices");
        }
        if (xmlState->hasAttribute("oversampleFactor"))
        {
            setOversampleFactor(xmlState->getIntAttribute("oversampleFactor", 4));
            xmlState->removeAttribute("oversampleFactor");
        }
        if (xmlState->hasAttribute("wavetableSize"))
        {
            setWavetableSize(xmlState->getIntAttribute("wavetableSize", 1024));
            xmlState->removeAttribute("wavetableSize");
        }
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(ValueTree::fromXml(*xmlState));
            parameterVtsHelper.instantlyUpdateSmoothers();
        }
    }
}

void PathSynthAudioProcessor::setNumVoices(int newNumVoices)
{
    numVoices = newNumVoices;
    const auto synthNumVoices = synthesiser.getNumVoices();
    if (synthNumVoices == numVoices)
    {
        return;
    }
    if (synthNumVoices < numVoices)
    {
        for (auto i = synthNumVoices; i < numVoices; ++i)
        {
            synthesiser.addVoice(new PathVoice(parameters, envParams, wavetable));
        }
        jassert(numVoices==synthesiser.getNumVoices());
        return;
    }
    // synthNumVoices > numVoices
    for (auto i = synthNumVoices; i > numVoices; --i)
    {
        synthesiser.removeVoice(i - 1);
    }
    jassert(numVoices==synthesiser.getNumVoices());
}

int PathSynthAudioProcessor::getNumVoices()
{
    jassert(numVoices==synthesiser.getNumVoices());
    return numVoices;
}

void PathSynthAudioProcessor::setOversampleFactor(int newOversampleFactor)
{
    oversampleFactor = newOversampleFactor;
    synthesiser.setCurrentPlaybackSampleRate(getSampleRate() * oversampleFactor);
}

//==============================================================================
void PathSynthAudioProcessor::setWavetableSize(int newWavetableSize)
{
    nextWavetableSize = newWavetableSize;
}

//==============================================================================
void PathSynthAudioProcessor::setPath(int numSamples)
{
    if (nextWavetableSize != wavetable.size())
        wavetable.resize(nextWavetableSize);

    //todo check if it changed
    straightPath.clear();

    const Point<float> firstPointPos{
        parameterVtsHelper.getPointXSkip(0, numSamples),
        parameterVtsHelper.getPointYSkip(0, numSamples)
    };
    straightPath.startNewSubPath(firstPointPos);

    for (auto i = 1; i < PathSynthConstants::numControlPoints; ++i)
    {
        const Point<float> pointPos{
            parameterVtsHelper.getPointXSkip(i, numSamples),
            parameterVtsHelper.getPointYSkip(i, numSamples)
        };
        straightPath.lineTo(pointPos);
    }
    straightPath.closeSubPath();

    processorPath = straightPath;
    //const auto smoothPathBounds = processorPath.getBounds();
    /*processorPath.applyTransform(
        AffineTransform::translation(
            -smoothPathBounds.getCentreX(),
            -smoothPathBounds.getCentreY()).followedBy(AffineTransform::scale(1.0f / smoothPathBounds.getWidth(),
                                                                              1.0f / smoothPathBounds.getHeight())));*/
    const auto direction = *parameters.getRawParameterValue("direction");

    const auto smoothing = *parameters.getRawParameterValue("smoothing");
    processorPath = processorPath.createPathWithRoundedCorners(smoothing);

    auto pathLengthOverWaveLength = processorPath.getLength() / wavetable.size();

    PathFlatteningIterator iterator(processorPath);
    iterator.next();

    auto accumulatedDistance = 0.0f;
    for (auto i = 0; i < wavetable.size(); ++i)
    {
        auto distanceFromStart = static_cast<float>(i) * pathLengthOverWaveLength;
        if (i == 0)
            distanceFromStart = (static_cast<float>(i) + 0.1f) * pathLengthOverWaveLength;

        bool filledValue = false;
        while (!filledValue)
        {
            const Line<float> line(iterator.x1, iterator.y1, iterator.x2, iterator.y2);
            auto lineLength = line.getLength();
            if (distanceFromStart <= lineLength + accumulatedDistance)
            {
                if (direction == 0)
                    wavetable[i] = line.getPointAlongLine(distanceFromStart - accumulatedDistance).getX();
                else
                    wavetable[i] = line.getPointAlongLine(distanceFromStart - accumulatedDistance).getY();

                jassert(!std::isnan(wavetable[i]));
                filledValue = true;
            }
            else
            {
                accumulatedDistance += lineLength;
                const auto moreToIterate = iterator.next();
                if (!moreToIterate)
                    break;
            }
        }
        // if we reached the end of the path without finishing, use the final value
        if (!filledValue)
            if (direction == 0)
                wavetable[i] = iterator.x2;
            else
                wavetable[i] = iterator.y2;
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PathSynthAudioProcessor();
}
