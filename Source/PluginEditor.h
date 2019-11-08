#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "ControlPointComponent.h"
#include "PlaneComponent.h"
#include "WaveDisplayComponent.h"

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
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    PathSynthAudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;

    WaveDisplayComponent waveDisplayComponent;
    PlaneComponent planeComponent;
    std::vector<std::unique_ptr<ControlPointComponent>> controlPoints;

    Slider frequencySlider;
    std::unique_ptr<SliderAttachment> frequencyAttachment;

    Slider smoothSlider;
    std::unique_ptr<SliderAttachment> smoothAttachment;

    ComboBox directionBox;
    std::unique_ptr<ComboBoxAttachment> directionAttachment;

    Path straightPath;
    Path smoothPath;
    Path signalPath;
    bool pathChanged = true;
    float lastSmoothing = 0.0f;
    float lastDirection = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathSynthAudioProcessorEditor)
};
