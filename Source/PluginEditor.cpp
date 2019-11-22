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
    GlyphArrangement glyph;
    glyph.addLineOfText(Font(32, Font::bold), "PATH SYNTH", 12, 38);
    glyph.createPath(titlePath);

    nameLabel.setFont(Font(16, Font::bold));
    nameLabel.setText("Luke M. Craig", dontSendNotification);
    nameLabel.setColour(Label::textColourId, Colour(0xffdcc296));
    nameLabel.setJustificationType(Justification::bottomLeft);
    addAndMakeVisible(nameLabel);

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
    oversamplingBox.addItem("16x", 16);
    oversamplingBox.setSelectedId(processor.getOversampleFactor());
    oversamplingBox.addListener(this);

    voicesLabel.setText("Max Voices", dontSendNotification);
    makeLabelUpperCase(voicesLabel);
    addAndMakeVisible(voicesLabel);
    addAndMakeVisible(voicesSlider);
    voicesSlider.setRange(1, PathSynthConstants::maxVoices, 1);
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

    outGainLabel.setText("Out", dontSendNotification);
    makeLabelUpperCase(outGainLabel);
    addAndMakeVisible(outGainLabel);
    addAndMakeVisible(outGainSlider);
    outGainSlider.setSliderStyle(Slider::Rotary);
    outGainAttachment.reset(new SliderAttachment(parameters, "outgain", outGainSlider));

    setupAdsrControl(attackLabel, attackSlider, attackAttachment, "Attack", "attack");
    setupAdsrControl(decayLabel, decaySlider, decayAttachment, "Decay", "decay");
    setupAdsrControl(sustainLabel, sustainSlider, sustainAttachment, "Sustain", "sustain");
    setupAdsrControl(releaseLabel, releaseSlider, releaseAttachment, "Release", "release");

    auto& lookAndFeel = getLookAndFeel();
    lookAndFeel.setColour(ResizableWindow::backgroundColourId, Colour(0xffe4753d));
    lookAndFeel.setColour(Label::textColourId, Colours::black);

    lookAndFeel.setColour(Slider::trackColourId, Colour(0xffa84350));
    lookAndFeel.setColour(Slider::backgroundColourId, Colour(0xffdac9cb));
    lookAndFeel.setColour(Slider::thumbColourId, Colour(0xff98acb9));
    lookAndFeel.setColour(Slider::rotarySliderOutlineColourId, Colour(0xffdac9cb));
    lookAndFeel.setColour(Slider::rotarySliderFillColourId, Colour(0xffa84350));

    lookAndFeel.setColour(ComboBox::backgroundColourId, Colour(0xffdcc296));
    lookAndFeel.setColour(ComboBox::textColourId, Colours::black);

    lookAndFeel.setColour(MidiKeyboardComponent::ColourIds::keyDownOverlayColourId, Colour(0xff97b0c4));
    lookAndFeel.setColour(MidiKeyboardComponent::ColourIds::whiteNoteColourId, Colour(0xffdcc296));
    lookAndFeel.setColour(MidiKeyboardComponent::ColourIds::blackNoteColourId, Colour(0xff513a1d));
    lookAndFeel.setColour(MidiKeyboardComponent::ColourIds::upDownButtonBackgroundColourId, Colour(0xffdcc296));
    lookAndFeel.setColour(MidiKeyboardComponent::ColourIds::upDownButtonArrowColourId, Colour(0xff513a1d));

    setResizable(true, true);
    setResizeLimits(256, 256, 2048, 2048);
    setSize(885, 720);

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

    g.setColour(getLookAndFeel().findColour(MidiKeyboardComponent::ColourIds::keyDownOverlayColourId));
    g.fillPath(titlePath);

    g.setColour(getLookAndFeel().findColour(ComboBox::backgroundColourId));
    g.strokePath(titlePath, PathStrokeType(1.0f));
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
    DBG(area.getWidth());
    DBG(area.getHeight());
    area.reduce(10, 10);

    auto titleArea = area.removeFromTop(32);
    titleArea.removeFromLeft(titlePath.getBounds().getWidth() + 10);
    nameLabel.setBounds(titleArea.removeFromLeft(100));

    titleArea.removeFromLeft(titleArea.proportionOfWidth(0.25f));

    auto voicesOversamplingArea = titleArea;

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

    auto belowPanels = area.removeFromBottom(64);

    auto outArea = belowPanels.removeFromRight(belowPanels.proportionOfWidth(0.25f));
    outGainLabel.
        setBounds(outArea.removeFromLeft(1.2f * outGainLabel.getFont().getStringWidth(outGainLabel.getText())));
    outGainSlider.setBounds(outArea);

    auto directionArea = belowPanels.removeFromBottom(20);
    directionLabel.setBounds(
        directionArea.removeFromLeft(directionLabel.getFont().getStringWidth(directionLabel.getText())));
    directionBox.setBounds(directionArea);

    area.removeFromBottom(10);

    auto smoothArea = belowPanels.removeFromBottom(20);
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
