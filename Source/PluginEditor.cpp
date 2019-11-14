#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PathSynthAudioProcessorEditor::PathSynthAudioProcessorEditor(PathSynthAudioProcessor& p,
                                                             AudioProcessorValueTreeState& apvts, MidiKeyboardState& ks)
    : AudioProcessorEditor(&p),
      processor(p),
      parameters(apvts),
      keyboardState(ks),
      keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
      planeComponent(apvts)
{
    addAndMakeVisible(planeComponent);
    addAndMakeVisible(waveDisplayComponent);

    addAndMakeVisible(keyboardComponent);

    oversamplingLabel.setText("Oversampling", dontSendNotification);
    makeLabelUpperCase(oversamplingLabel);
    addAndMakeVisible(oversamplingLabel);
    addAndMakeVisible(oversamplingBox);
    oversamplingBox.addItem("1x", 1);
    oversamplingBox.addItem("2x", 2);
    oversamplingBox.addItem("4x", 4);
    oversamplingBox.addItem("8x", 8);
    oversamplingBox.setSelectedId(processor.getOversampleFactor());
    oversamplingBox.addListener(this);

    voicesLabel.setText("Max Voices", dontSendNotification);
    makeLabelUpperCase(voicesLabel);
    addAndMakeVisible(voicesLabel);
    addAndMakeVisible(voicesSlider);
    voicesSlider.setRange(1, 10, 1);
    voicesSlider.setValue(processor.getNumVoices());
    voicesSlider.addListener(this);

    smoothLabel.setText("Smoothness", dontSendNotification);
    makeLabelUpperCase(smoothLabel);
    addAndMakeVisible(smoothLabel);
    addAndMakeVisible(smoothSlider);
    smoothAttachment.reset(new SliderAttachment(parameters, "smoothing", smoothSlider));

    directionLabel.setText("Direction", dontSendNotification);
    makeLabelUpperCase(directionLabel);
    addAndMakeVisible(directionLabel);
    addAndMakeVisible(directionBox);
    directionBox.addItem("X", 1);
    directionBox.addItem("Y", 2);
    directionAttachment.reset(new ComboBoxAttachment(parameters, "direction", directionBox));

    setupAdsrControl(attackLabel, attackSlider, attackAttachment, "Attack", "attack");
    setupAdsrControl(decayLabel, decaySlider, decayAttachment, "Decay", "decay");
    setupAdsrControl(sustainLabel, sustainSlider, sustainAttachment, "Sustain", "sustain");
    setupAdsrControl(releaseLabel, releaseSlider, releaseAttachment, "Release", "release");

    auto& lookAndFeel = getLookAndFeel();
    lookAndFeel.setColour(ResizableWindow::backgroundColourId, Colour(0xffe4753d));
    lookAndFeel.setColour(Label::textColourId, Colours::black);
    lookAndFeel.setColour(Slider::trackColourId, Colour(0xffa84350));
    lookAndFeel.setColour(Slider::backgroundColourId, Colour(0xffdac9cb));
    lookAndFeel.setColour(ComboBox::backgroundColourId, Colour(0xffdcc296));
    lookAndFeel.setColour(ComboBox::textColourId, Colours::black);
    lookAndFeel.setColour(MidiKeyboardComponent::ColourIds::keyDownOverlayColourId, Colour(0xff97b0c4));

    setResizable(true, true);
    setResizeLimits(32, 32, 2048, 2048);
    setSize(1024, 512);

    startTimer(100);
}

PathSynthAudioProcessorEditor::~PathSynthAudioProcessorEditor()
{
}

void PathSynthAudioProcessorEditor::makeLabelUpperCase(Label& label)
{
    label.setText(label.getText().toUpperCase(), dontSendNotification);
}

void PathSynthAudioProcessorEditor::setupAdsrControl(Label& label, Slider& slider,
                                                     std::unique_ptr<SliderAttachment>& attachment,
                                                     const String& labelText, const String& parameterId)
{
    label.setText(labelText, dontSendNotification);
    makeLabelUpperCase(label);
    addAndMakeVisible(label);

    slider.setTextBoxStyle(Slider::TextBoxAbove, false, 64, 32);
    slider.setSliderStyle(Slider::LinearBar);
    addAndMakeVisible(slider);

    attachment.reset(new SliderAttachment(parameters, parameterId, slider));
}

//==============================================================================
void PathSynthAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void PathSynthAudioProcessorEditor::setLabelAreaAboveCentered(Label& label, Rectangle<int>& labelArea)
{
    label.setBounds(
        labelArea.removeFromTop(16).withSizeKeepingCentre(
            6 + label.getFont().getStringWidth(label.getText()), 16));
}

void PathSynthAudioProcessorEditor::resized()
{
    auto area = getBounds();
    area.reduce(10, 10);

    auto voicesOversamplingArea = area.removeFromTop(20);

    voicesLabel.setBounds(
        voicesOversamplingArea.removeFromLeft(voicesLabel.getFont().getStringWidth(voicesLabel.getText())));
    voicesSlider.setBounds(voicesOversamplingArea.removeFromLeft(voicesOversamplingArea.proportionOfWidth(0.5f)));

    oversamplingLabel.setBounds(
        voicesOversamplingArea.removeFromLeft(oversamplingLabel.getFont().getStringWidth(oversamplingLabel.getText())));
    oversamplingBox.setBounds(voicesOversamplingArea);

    auto adsrBounds = area.removeFromTop(area.proportionOfHeight(0.1f));
    auto adsrWidth = adsrBounds.getWidth() / 4.0f;

    auto attackArea = adsrBounds.removeFromLeft(adsrWidth);
    auto decayArea = adsrBounds.removeFromLeft(adsrWidth);
    auto sustainArea = adsrBounds.removeFromLeft(adsrWidth);
    auto releaseArea = adsrBounds.removeFromLeft(adsrWidth);

    setLabelAreaAboveCentered(attackLabel, attackArea);
    setLabelAreaAboveCentered(decayLabel, decayArea);
    setLabelAreaAboveCentered(sustainLabel, sustainArea);
    setLabelAreaAboveCentered(releaseLabel, releaseArea);

    attackSlider.setBounds(attackArea);
    decaySlider.setBounds(decayArea);
    sustainSlider.setBounds(sustainArea);
    releaseSlider.setBounds(releaseArea);

    keyboardComponent.setBounds(area.removeFromBottom(100));
    area.removeFromBottom(10);

    auto directionArea = area.removeFromBottom(20);
    directionLabel.setBounds(
        directionArea.removeFromLeft(directionLabel.getFont().getStringWidth(directionLabel.getText())));
    directionBox.setBounds(directionArea);

    area.removeFromBottom(10);

    auto smoothArea = area.removeFromBottom(20);
    smoothLabel.setBounds(smoothArea.removeFromLeft(smoothLabel.getFont().getStringWidth(smoothLabel.getText())));
    smoothSlider.setBounds(smoothArea);

    planeComponent.setBounds(area.removeFromLeft(area.proportionOfWidth(0.5)).reduced(10));
    waveDisplayComponent.setBounds(area.reduced(10));
}

void PathSynthAudioProcessorEditor::timerCallback()
{
    const Path smoothPath = planeComponent.update();
    const auto direction = *parameters.getRawParameterValue("direction");
    waveDisplayComponent.update(smoothPath, direction);
}

void PathSynthAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &voicesSlider)
    {
        processor.setNumVoices(voicesSlider.getValue());
    }
}

void PathSynthAudioProcessorEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &oversamplingBox)
    {
        processor.setOversampleFactor(oversamplingBox.getSelectedId());
    }
}
