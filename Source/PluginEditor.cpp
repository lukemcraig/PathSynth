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
