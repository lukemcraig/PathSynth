#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PathSynthAudioProcessorEditor::PathSynthAudioProcessorEditor(PathSynthAudioProcessor& p,
                                                             AudioProcessorValueTreeState& apvts, MidiKeyboardState& ks)
    : AudioProcessorEditor(&p), processor(p), parameters(apvts), planeComponent(apvts), keyboardState(ks),
      keyboardComponent(keyboardState, MidiKeyboardComponent::Orientation::horizontalKeyboard)
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

    {
        const auto adsrSliderStyle = Slider::LinearBar;

        attackSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 64, 32);
        attackSlider.setSliderStyle(adsrSliderStyle);
        addAndMakeVisible(attackSlider);
        attackAttachment.reset(new SliderAttachment(parameters, "attack", attackSlider));

        decaySlider.setTextBoxStyle(Slider::TextBoxAbove, false, 64, 32);
        decaySlider.setSliderStyle(adsrSliderStyle);
        addAndMakeVisible(decaySlider);
        decayAttachment.reset(new SliderAttachment(parameters, "decay", decaySlider));

        sustainSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 64, 32);
        sustainSlider.setSliderStyle(adsrSliderStyle);
        addAndMakeVisible(sustainSlider);
        sustainAttachment.reset(new SliderAttachment(parameters, "sustain", sustainSlider));

        releaseSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 64, 32);
        releaseSlider.setSliderStyle(adsrSliderStyle);
        addAndMakeVisible(releaseSlider);
        releaseAttachment.reset(new SliderAttachment(parameters, "release", releaseSlider));
    }

    auto& lookAndFeel = getLookAndFeel();
    lookAndFeel.setColour(ResizableWindow::backgroundColourId, Colour(0xffe4753d));
    lookAndFeel.setColour(ResizableWindow::backgroundColourId, Colour(0xffe4753d));

    setResizable(true, true);
    setResizeLimits(32, 32, 2048, 2048);
    setSize(1024, 512);

    startTimer(100);
}

PathSynthAudioProcessorEditor::~PathSynthAudioProcessorEditor()
{
}

//==============================================================================
void PathSynthAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void PathSynthAudioProcessorEditor::resized()
{
    auto bounds = getBounds();

    auto adsrBounds = bounds.removeFromTop(bounds.proportionOfHeight(0.1f));
    auto adsrWidth = adsrBounds.getWidth() / 4.0f;
    attackSlider.setBounds(adsrBounds.removeFromLeft(adsrWidth));
    decaySlider.setBounds(adsrBounds.removeFromLeft(adsrWidth));
    sustainSlider.setBounds(adsrBounds.removeFromLeft(adsrWidth));
    releaseSlider.setBounds(adsrBounds.removeFromLeft(adsrWidth));

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
