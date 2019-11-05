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

    points.emplace_back(2.5f, 2.5f);
    points.emplace_back(0.0f, 5.0f);
    points.emplace_back(-2.5f, 2.5f);
    points.emplace_back(-5.0f, 0.0f);
    points.emplace_back(-2.5f, -2.5f);
    points.emplace_back(0.0f, -5.0f);
    points.emplace_back(2.5f, -2.5f);
    points.emplace_back(5.0f, 0.0f);;

    const auto halfWidth = 0.5f * width;
    const auto halfHeight = 0.5f * height;
    const auto transform = AffineTransform::scale(40.0f, 40.0f).followedBy(
        AffineTransform::translation(halfWidth, halfHeight));
    for (auto&& point : points)
    {
        point.applyTransform(transform);
        controlPoints.emplace_back(std::make_unique<ControlPointComponent>(pathChanged));
    }

    for (auto&& controlPoint : controlPoints)
    {
        addAndMakeVisible(controlPoint.get());
    }

    setSize(width * 2, height);

    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = points[i].getX() - 5.0f;
        auto y = points[i].getY() - 5.0f;
        controlPoints[i]->setBounds(x, y, 10.0f, 10.0f);
    }
    startTimer(25);
}

PathSynthAudioProcessorEditor::~PathSynthAudioProcessorEditor()
{
}

//==============================================================================
void PathSynthAudioProcessorEditor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);

    g.drawLine(12.0f, 256.0f, 500.0f, 256.0f);
    g.drawLine(256.0f, 12.0f, 256.0f, 500.0f);

    g.strokePath(straightPath, PathStrokeType(1.0));
    g.strokePath(smoothPath, PathStrokeType(1.0));

    g.strokePath(signalPath, PathStrokeType(1.0));

    g.drawLine(512.0f, 256.0f, 1012.0f, 256.0f);
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
    const auto smoothing = *parameters.getRawParameterValue("smoothing");
    auto direction = *parameters.getRawParameterValue("direction");
    if (pathChanged || lastSmoothing != smoothing || lastDirection != direction || processorWasBusy)
    {
        pathChanged = false;
        lastSmoothing = smoothing;
        lastDirection = direction;
        straightPath.clear();

        auto firstPointPos = controlPoints[0]->getPosition().toFloat();
        firstPointPos.setX(firstPointPos.x + controlPoints[0]->getWidth() * 0.5f);
        firstPointPos.setY(firstPointPos.y + controlPoints[0]->getHeight() * 0.5f);
        straightPath.startNewSubPath(firstPointPos);

        for (auto i = 1; i < controlPoints.size(); ++i)
        {
            auto pointPos = controlPoints[i]->getPosition().toFloat();
            pointPos.setX(pointPos.x + controlPoints[i]->getWidth() * 0.5f);
            pointPos.setY(pointPos.y + controlPoints[i]->getHeight() * 0.5f);
            straightPath.lineTo(pointPos);
        }
        straightPath.closeSubPath();

        smoothPath = straightPath.createPathWithRoundedCorners(smoothing);

        Path newPath = smoothPath;
        {
            auto newPathBounds = newPath.getBounds();
            newPath.applyTransform(
                AffineTransform::translation(-newPathBounds.getCentreX(), -newPathBounds.getCentreY()).followedBy(
                    AffineTransform::scale(1.0f / newPathBounds.getWidth(), 1.0f / newPathBounds.getHeight())));
            auto newPathBoundsScaled = newPath.getBounds();
        }

        processorWasBusy = !processor.setPath(newPath);

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
