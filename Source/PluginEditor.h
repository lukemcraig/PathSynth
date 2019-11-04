#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "ControlPointComponent.h"

//==============================================================================
/**
*/
class PathSynthAudioProcessorEditor : public AudioProcessorEditor
{
public:
    PathSynthAudioProcessorEditor(PathSynthAudioProcessor&);

    ~PathSynthAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;

    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PathSynthAudioProcessor& processor;

    std::vector<std::unique_ptr<ControlPointComponent>> controlPoints;
    std::vector<Point<float>> points;
    Path straightPath;
    Path guiPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PathSynthAudioProcessorEditor)
};
