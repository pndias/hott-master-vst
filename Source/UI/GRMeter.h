#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Vertical LED-style gain reduction meter with smooth animation.
class GRMeter : public juce::Component, private juce::Timer
{
public:
    GRMeter(juce::Colour c) : colour(c) { startTimerHz(30); }

    void setGainReduction(float dB) { targetGR = dB; }

    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();

        // Background slot
        g.setColour(juce::Colour(0xFF1A1A28));
        g.fillRoundedRectangle(b, 4.f);
        g.setColour(juce::Colour(0xFF2A2A3A));
        g.drawRoundedRectangle(b, 4.f, 0.5f);

        // Fill from top (GR goes downward)
        float maxGR = 24.f;
        float normGR = juce::jlimit(0.f, 1.f, smoothGR / maxGR);
        float fillH = b.getHeight() * normGR;

        if (fillH > 1.f)
        {
            auto fillRect = b.removeFromTop(fillH);
            auto grad = juce::ColourGradient(colour.withAlpha(0.3f), fillRect.getX(), fillRect.getY(),
                                             colour.withAlpha(0.8f), fillRect.getX(), fillRect.getBottom(), false);
            g.setGradientFill(grad);
            g.fillRoundedRectangle(fillRect.reduced(1.f), 3.f);
        }

        // dB text at bottom
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(juce::Font(9.f));
        g.drawText(juce::String(-smoothGR, 1) + " dB", getLocalBounds().removeFromBottom(14),
                   juce::Justification::centred, false);
    }

private:
    void timerCallback() override
    {
        float diff = targetGR - smoothGR;
        smoothGR += diff * 0.2f;
        if (std::abs(diff) > 0.01f) repaint();
    }

    juce::Colour colour;
    float targetGR = 0.f, smoothGR = 0.f;
};
