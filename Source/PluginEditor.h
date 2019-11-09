#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PlaneComponent.h"
#include "WaveDisplayComponent.h"

//==============================================================================
/**
*/
class PathSynthAudioProcessorEditor : public AudioProcessorEditor,
                                      public Timer
{
public:
    PathSynthAudioProcessorEditor(PathSynthAudioProcessor&, AudioProcessorValueTreeState&, MidiKeyboardState&);

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

    MidiKeyboardState& keyboardState;
    MidiKeyboardComponent keyboardComponent;

    WaveDisplayComponent waveDisplayComponent;
    PlaneComponent planeComponent;

    Slider frequencySlider;
    std::unique_ptr<SliderAttachment> frequencyAttachment;

    Slider smoothSlider;
    std::unique_ptr<SliderAttachment> smoothAttachment;

    ComboBox directionBox;
    std::unique_ptr<ComboBoxAttachment> directionAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathSynthAudioProcessorEditor)
};
