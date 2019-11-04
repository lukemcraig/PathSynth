#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PathSynthAudioProcessorEditor::PathSynthAudioProcessorEditor(PathSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    auto width = 512;
    auto height = 512;

    points.emplace_back(3.0f, 1.0f);
    points.emplace_back(2.5f, 4.0f);
    points.emplace_back(0.0f, 1.0f);
    points.emplace_back(-2.5f, 4.0f);
    points.emplace_back(-3.0f, 0.0f);
    points.emplace_back(-2.5f, -4.0f);
    points.emplace_back(0.0f, -1.0f);
    points.emplace_back(2.5f, -4.0f);
    points.emplace_back(3.0f, -1.0f);

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

    setResizable(true, true);
    setResizeLimits(10, 10, 1000, 1000);
    setSize(width, height);

    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        auto x = points[i].getX() - 5.0f;
        auto y = points[i].getY() - 5.0f;
        controlPoints[i]->setBounds(x, y, 10.0f, 10.0f);
    }
    startTimer(100);
}

PathSynthAudioProcessorEditor::~PathSynthAudioProcessorEditor()
{
}

//==============================================================================
void PathSynthAudioProcessorEditor::paint(Graphics& g)
{
    //straightPath.clear();
    //straightPath.startNewSubPath(points[0]);
    //for (auto&& point : points)
    //{
    //    straightPath.lineTo(point);
    //}
    //guiPath = straightPath.createPathWithRoundedCorners(100.0f);

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::white);
    //g.setFont(15.0f);
    //g.drawFittedText("Hello World!", getLocalBounds(), Justification::centred, 1);

    //for (auto&& point : points)
    //{
    //    g.drawEllipse(point.getX() - 2.5f, point.getY() - 2.5f, 5.0f, 5.0f, 1.0f);
    //}

    g.strokePath(straightPath, PathStrokeType(1.0));
    g.strokePath(smoothPath, PathStrokeType(1.0));
}

void PathSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //controlPoint.setBounds(getBounds().removeFromTop(100));
}

void PathSynthAudioProcessorEditor::timerCallback()
{
    if (pathChanged)
    {
        pathChanged = false;
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
        //straightPath.lineTo(firstPointPos);

        smoothPath = straightPath.createPathWithRoundedCorners(100.0f);

        processor.setPath(smoothPath,getWidth(),getHeight());

        repaint();
    }
}
