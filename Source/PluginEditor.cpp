#include "PluginProcessor.h"
#include "PluginEditor.h"

void PathSynthAudioProcessorEditor::setupAdsrControl(Label& label, Slider& slider,
                                                     std::unique_ptr<PathSynthAudioProcessorEditor::SliderAttachment>&
                                                     attachment, const String& labelText, const String& parameterId)
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

    addAndMakeVisible(smoothSlider);
    smoothAttachment.reset(new SliderAttachment(parameters, "smoothing", smoothSlider));

    addAndMakeVisible(directionBox);
    directionBox.addItem("X", 1);
    directionBox.addItem("Y", 2);
    directionAttachment.reset(new ComboBoxAttachment(parameters, "direction", directionBox));

    setupAdsrControl(attackLabel, attackSlider, attackAttachment, "Attack", "attack");
    setupAdsrControl(decayLabel, decaySlider, decayAttachment, "Decay", "decay");
    setupAdsrControl(attackLabel, attackSlider, attackAttachment, "Sustain", "sustain");
    setupAdsrControl(releaseLabel, releaseSlider, releaseAttachment, "Release", "release");

    auto& lookAndFeel = getLookAndFeel();
    lookAndFeel.setColour(ResizableWindow::backgroundColourId, Colour(0xffe4753d));
    lookAndFeel.setColour(Label::textColourId, Colour(0xff513a1d));

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
    auto bounds = getBounds();

    auto adsrBounds = bounds.removeFromTop(bounds.proportionOfHeight(0.1f));
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

    keyboardComponent.setBounds(bounds.removeFromBottom(100));

    directionBox.setBounds(bounds.removeFromBottom(20));
    smoothSlider.setBounds(bounds.removeFromBottom(20));

    planeComponent.setBounds(bounds.removeFromLeft(bounds.proportionOfWidth(0.5)).reduced(10));
    waveDisplayComponent.setBounds(bounds.reduced(10));
}

void PathSynthAudioProcessorEditor::timerCallback()
{
    const Path smoothPath = planeComponent.update();
    const auto direction = *parameters.getRawParameterValue("direction");
    waveDisplayComponent.update(smoothPath, direction);
    // TODO more efficient repaint
    repaint();
}
