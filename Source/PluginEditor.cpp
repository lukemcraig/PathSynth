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

    static const unsigned char pathData[] = {
        110, 109, 183, 254, 39, 65, 213, 9, 49, 67, 108, 66, 102, 249, 65, 229, 145, 50, 67, 98, 181, 118, 82, 66, 56,
        82, 26, 67, 28, 40, 116, 66, 29, 49, 165, 66, 77, 99, 59, 67, 244, 68, 193, 66, 98, 28, 203, 54, 67, 81, 250,
        201, 67, 241, 21, 64, 66, 169, 47, 218, 67, 89, 199, 97, 66, 164, 181, 249, 67, 98, 220, 7, 110, 66, 220,
        201, 5, 68, 75, 174, 175, 66, 193, 70, 11, 68, 218, 240, 236, 66, 112, 152, 11, 68, 98, 204, 53, 87, 67, 17,
        196, 10, 68, 243, 222, 82, 67, 191, 80, 212, 67, 138, 253, 112, 67, 227, 188, 191, 66, 108, 95, 231, 182, 67,
        227, 188, 191, 66, 98, 231, 149, 179, 67, 157, 192, 83, 67, 49, 134, 170, 67, 100, 209, 163, 67, 208,
        128, 169, 67, 109, 156, 220, 67, 98, 217, 68, 170, 67, 171, 49, 1, 68, 206, 61, 193, 67, 193, 70, 11, 68, 17,
        125, 223, 67, 110, 103, 11, 68, 98, 146, 161, 8, 68, 204, 59, 12, 68, 230, 122, 16, 68, 214, 101, 222, 67, 230,
        122, 16, 68, 120, 163, 197, 67, 108, 173, 30, 11, 68, 120, 163, 197, 67, 98, 253, 155, 10, 68, 253,
        14, 218, 67, 30, 178, 5, 68, 68, 188, 232, 67, 6, 118, 246, 67, 248, 160, 233, 67, 98, 165, 105, 189, 67, 1,
        101, 234, 67, 244, 206, 220, 67, 201, 80, 133, 67, 248, 48, 221, 67, 4, 205, 194, 66, 108, 230, 122, 16, 68, 20,
        85, 196, 66, 108, 228, 73, 16, 68, 217, 46, 58, 65, 98, 124, 235, 92, 65, 115, 133, 14, 65, 228, 159,
        170, 66, 77, 190, 8, 192, 183, 254, 39, 65, 213, 9, 49, 67, 99, 101, 0, 0
    };
    piPath.loadPathFromData(pathData, sizeof (pathData));

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

    {
        int i = 0;
        for (auto&& point : points)
        {
            point.applyTransform(transform);
            controlPoints.emplace_back(std::make_unique<ControlPointComponent>(pathChanged, parameters, i++));
        }
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
    const auto smoothing = *parameters.getRawParameterValue("smoothing");
    auto direction = *parameters.getRawParameterValue("direction");
    if (pathChanged || lastSmoothing != smoothing || lastDirection != direction || processorWasBusy)
    {
        pathChanged = false;
        lastSmoothing = smoothing;
        lastDirection = direction;
        straightPath.clear();

        auto firstPointPos = controlPoints[0]->getPosition().toFloat();
        firstPointPos.setX(*parameters.getRawParameterValue("point0x"));
        firstPointPos.setY(*parameters.getRawParameterValue("point0y"));
        straightPath.startNewSubPath(firstPointPos);

        for (auto i = 1; i < controlPoints.size(); ++i)
        {
            auto pointPos = controlPoints[i]->getPosition().toFloat();
            pointPos.setX(*parameters.getRawParameterValue("point" + String(i) + "x"));
            pointPos.setY(*parameters.getRawParameterValue("point" + String(i) + "y"));
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
            auto newPathBoundsScaled = newPath.getBounds();
        }

        //processorWasBusy = !processor.setPath(newPath);
        if (processorWasBusy)
        DBG("processorWasBusy");

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
