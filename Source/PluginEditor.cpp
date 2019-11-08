#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PathSynthAudioProcessorEditor::PathSynthAudioProcessorEditor(PathSynthAudioProcessor& p,
                                                             AudioProcessorValueTreeState& apvts)
    : AudioProcessorEditor(&p), processor(p), parameters(apvts)
{
    auto width = 512;
    auto height = 512;

    addAndMakeVisible(frequencySlider);
    frequencyAttachment.reset(new SliderAttachment(parameters, "frequency", frequencySlider));

    addAndMakeVisible(smoothSlider);
    smoothAttachment.reset(new SliderAttachment(parameters, "smoothing", smoothSlider));

    addAndMakeVisible(directionBox);
    directionBox.addItem("X", 1);
    directionBox.addItem("Y", 2);
    directionAttachment.reset(new ComboBoxAttachment(parameters, "direction", directionBox));

    for (auto i = 0; i < 8; ++i)
    {
        controlPoints.emplace_back(std::make_unique<ControlPointComponent>(pathChanged, parameters, i));
    }

    for (auto&& controlPoint : controlPoints)
    {
        addAndMakeVisible(controlPoint.get());
    }

    setSize(width * 2, height);

    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = (*parameters.getRawParameterValue("point" + String(i) + "x") * 512.0f) + 256.0f;
        auto y = (*parameters.getRawParameterValue("point" + String(i) + "y") * 512.0f) + 256.0f;
        controlPoints[i]->setBounds(x, y, 10.0f, 10.0f);
    }
    startTimer(5);
}

PathSynthAudioProcessorEditor::~PathSynthAudioProcessorEditor()
{
}

//==============================================================================
void PathSynthAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::grey);

    g.drawLine(12.0f, 256.0f, 500.0f, 256.0f);
    g.drawLine(256.0f, 12.0f, 256.0f, 500.0f);

    g.drawLine(512.0f, 256.0f, 1012.0f, 256.0f);

    g.strokePath(straightPath, PathStrokeType(1.0));

    g.setColour(Colours::white);

    g.strokePath(smoothPath, PathStrokeType(1.0));

    g.strokePath(signalPath, PathStrokeType(1.0));
}

void PathSynthAudioProcessorEditor::resized()
{
    auto bounds = getBounds();
    directionBox.setBounds(bounds.removeFromBottom(20));
    smoothSlider.setBounds(bounds.removeFromBottom(20));
    frequencySlider.setBounds(bounds.removeFromBottom(20));
}

void PathSynthAudioProcessorEditor::timerCallback()
{
    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = (*parameters.getRawParameterValue("point" + String(i) + "x") * 512.0f);
        auto y = (*parameters.getRawParameterValue("point" + String(i) + "y") * 512.0f);
        controlPoints[i]->setBounds(x, y, 10.0f, 10.0f);
    }

    const auto smoothing = *parameters.getRawParameterValue("smoothing");
    const auto direction = *parameters.getRawParameterValue("direction");
    if (pathChanged || lastSmoothing != smoothing || lastDirection != direction || processorWasBusy)
    {
        pathChanged = false;
        lastSmoothing = smoothing;
        lastDirection = direction;
        straightPath.clear();

        // TODO maybe just make this use the control points instead of the parameters?
        const Point<float> firstPointPos{
            *parameters.getRawParameterValue("point0x") * 512.0f, *parameters.getRawParameterValue("point0y") * 512.0f
        };
        straightPath.startNewSubPath(firstPointPos);

        for (auto i = 1; i < 8; ++i)
        {
            const Point<float> pointPos{
                *parameters.getRawParameterValue("point" + String(i) + "x") * 512.0f,
                *parameters.getRawParameterValue("point" + String(i) + "y") * 512.0f
            };
            straightPath.lineTo(pointPos);
        }
        straightPath.closeSubPath();

        smoothPath = straightPath.createPathWithRoundedCorners(smoothing);

        float dashes[] = {2.0f, 3.0f, 4.0f, 5.0f};
        PathStrokeType(1.0).createDashedStroke(straightPath, straightPath, dashes, 4);

        Path newPath = smoothPath;
        {
            auto newPathBounds = newPath.getBounds();
            newPath.applyTransform(
                AffineTransform::translation(-newPathBounds.getCentreX(), -newPathBounds.getCentreY()).followedBy(
                    AffineTransform::scale(1.0f / newPathBounds.getWidth(), 1.0f / newPathBounds.getHeight())));
        }

        {
            const auto length = newPath.getLength();
            float t = 0.0f;
            signalPath.clear();

            for (int i = 1; i <= 1000; ++i)
            {
                t = static_cast<float>(i) / 1000.0f;
                const auto point = newPath.getPointAlongPath(length * t);
                float pointValue;
                if (direction == 0)
                    pointValue = point.getX();
                else
                    pointValue = point.getY();
                pointValue = (pointValue * 256.0f) + 256.0f;
                if (i == 1)
                    signalPath.startNewSubPath(512.0f, pointValue);
                else
                    signalPath.lineTo(512.0f + (i * 0.5f), pointValue);
            }
        }

        repaint();
    }
}
