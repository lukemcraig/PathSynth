#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "ControlPointComponent.h"

//==============================================================================
/**
*/
class PathSynthAudioProcessorEditor : public AudioProcessorEditor,
                                      public Timer
{
public:
    PathSynthAudioProcessorEditor(PathSynthAudioProcessor&, AudioProcessorValueTreeState&);

    ~PathSynthAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

    void timerCallback() override;

private:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PathSynthAudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;

    Slider frequencySlider;
    std::unique_ptr<SliderAttachment> frequencyAttachment;

    Slider smoothSlider;
    std::unique_ptr<SliderAttachment> smoothAttachment;

    std::vector<std::unique_ptr<ControlPointComponent>> controlPoints;
    std::vector<Point<float>> points;
    Path straightPath;
    Path smoothPath;
    Path signalPath;
    bool pathChanged = true;
    float lastSmoothing = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathSynthAudioProcessorEditor)
};
