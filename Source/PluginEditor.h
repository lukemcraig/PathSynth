#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "PlaneComponent.h"
#include "WaveDisplayComponent.h"

//==============================================================================
/**
*/
class PathSynthAudioProcessorEditor : public AudioProcessorEditor,
                                      public Timer,
                                      public Slider::Listener,
                                      public ComboBox::Listener
{
public:

    PathSynthAudioProcessorEditor(PathSynthAudioProcessor&, AudioProcessorValueTreeState&, MidiKeyboardState&);

    ~PathSynthAudioProcessorEditor();

    void makeLabelUpperCase(Label& label);

    void setupAdsrControl(Label& label, Slider& slider,
                          std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>& attachment,
                          const String& labelText, const String& parameterId);

    //==============================================================================
    void paint(Graphics&) override;

    static void setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea);

    static int getLabelWidth(Label& label);

    void resized() override;

    void timerCallback() override;

    void sliderValueChanged(Slider* slider) override;

    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

private:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    PathSynthAudioProcessor& processor;
    AudioProcessorValueTreeState& parameters;

    MidiKeyboardState& keyboardState;
    MidiKeyboardComponent keyboardComponent;

    WaveDisplayComponent waveDisplayComponent;
    PlaneComponent planeComponent;

    Label voicesLabel;
    Slider voicesSlider;

    Label pitchStandardLabel;
    Slider pitchStandardSlider;
    std::unique_ptr<SliderAttachment> pitchStandardAttachment;

    Label oversamplingLabel;
    ComboBox oversamplingBox;

    Label wavetableLabel;
    ComboBox wavetableBox;

    Label smoothLabel;
    Slider smoothSlider;
    std::unique_ptr<SliderAttachment> smoothAttachment;

    Label directionLabel;
    ComboBox directionBox;
    std::unique_ptr<ComboBoxAttachment> directionAttachment;

    Label attackLabel;
    Slider attackSlider;
    std::unique_ptr<SliderAttachment> attackAttachment;

    Label decayLabel;
    Slider decaySlider;
    std::unique_ptr<SliderAttachment> decayAttachment;

    Label sustainLabel;
    Slider sustainSlider;
    std::unique_ptr<SliderAttachment> sustainAttachment;

    Label releaseLabel;
    Slider releaseSlider;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    Label outGainLabel;
    Slider outGainSlider;
    std::unique_ptr<SliderAttachment> outGainAttachment;

    Label nameLabel;
    Path titlePath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathSynthAudioProcessorEditor)
};
