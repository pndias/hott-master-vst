#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Custom LookAndFeel — dark matte aesthetic with accent-colored arcs,
// inspired by Neural DSP / FabFilter / macOS design language.
class HottLookAndFeel : public juce::LookAndFeel_V4
{
public:
    HottLookAndFeel()
    {
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white.withAlpha(0.85f));
        setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.6f));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, w, h).toFloat().reduced(4.f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.f;
        auto centre = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        auto arcColour = slider.findColour(juce::Slider::rotarySliderFillColourId);

        // Outer ring — dark track
        juce::Path bgArc;
        bgArc.addCentredArc(centre.x, centre.y, radius - 2.f, radius - 2.f,
                            0.f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xFF2A2A3A));
        g.strokePath(bgArc, juce::PathStrokeType(3.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Value arc — accent gradient
        if (sliderPos > 0.001f)
        {
            juce::Path valueArc;
            valueArc.addCentredArc(centre.x, centre.y, radius - 2.f, radius - 2.f,
                                   0.f, rotaryStartAngle, angle, true);
            auto grad = juce::ColourGradient(arcColour.withAlpha(0.5f), centre.x, bounds.getY(),
                                             arcColour, centre.x, bounds.getBottom(), false);
            g.setGradientFill(grad);
            g.strokePath(valueArc, juce::PathStrokeType(3.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Knob body — subtle radial gradient (matte metal)
        float knobRadius = radius * 0.62f;
        auto knobGrad = juce::ColourGradient(juce::Colour(0xFF3A3A4A), centre.x, centre.y - knobRadius * 0.5f,
                                             juce::Colour(0xFF1E1E2E), centre.x, centre.y + knobRadius, true);
        g.setGradientFill(knobGrad);
        g.fillEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.f, knobRadius * 2.f);

        // Knob edge ring
        g.setColour(juce::Colour(0xFF4A4A5A));
        g.drawEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.f, knobRadius * 2.f, 0.8f);

        // Pointer line
        juce::Path pointer;
        float pointerLen = knobRadius * 0.75f;
        float pointerThickness = 2.f;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f, -pointerLen, pointerThickness, pointerLen * 0.5f, 1.f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
        g.setColour(arcColour);
        g.fillPath(pointer);

        // Glow dot at tip
        float dotX = centre.x + std::sin(angle) * (knobRadius * 0.55f);
        float dotY = centre.y - std::cos(angle) * (knobRadius * 0.55f);
        g.setColour(arcColour.withAlpha(0.9f));
        g.fillEllipse(dotX - 2.f, dotY - 2.f, 4.f, 4.f);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(label.getFont());
        g.drawText(label.getText(), label.getLocalBounds(), label.getJustificationType(), false);
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font(10.5f);
    }
};
