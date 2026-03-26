#include "PluginEditor.h"

static constexpr int kHeaderH    = 52;
static constexpr int kPadding    = 12;
static constexpr int kKnobW      = 64;
static constexpr int kKnobH      = 72;
static constexpr int kSectionH   = 160;
static constexpr int kLabelH     = 14;
static constexpr int kSectionGap = 8;
static constexpr int kMeterW     = 32;
static constexpr int kSectionPadTop = 30;

// Colours — dark matte palette
static const juce::Colour kBgDark      (0xFF111119);
static const juce::Colour kBgPanel     (0xFF1A1A28);
static const juce::Colour kBgPanelEdge (0xFF2A2A3A);
static const juce::Colour kTextDim     (0xFFAAAAAA);
static const juce::Colour kTextBright  (0xFFE8E8F0);

// Section accent colours — muted, sophisticated
static const juce::Colour kAccSoothe (0xFFA78BFA); // soft violet
static const juce::Colour kAccEQ     (0xFF60A5FA); // sky blue
static const juce::Colour kAccComp   (0xFF34D399); // emerald
static const juce::Colour kAccClip   (0xFFFBBF24); // amber
static const juce::Colour kAccLim    (0xFFF87171); // soft red
static const juce::Colour kAccOut    (0xFF9CA3AF); // neutral gray

HottMasterEditor::HottMasterEditor(HottMasterProcessor& p)
    : AudioProcessorEditor(&p), proc(p)
{
    setLookAndFeel(&lnf);

    sections.resize(6);
    sections[0] = {"RESONANCE",  kAccSoothe, {{"soothe_depth","DEPTH"}, {"soothe_speed","SPEED"}, {"soothe_select","SELECT"}}, {}, {}};
    sections[1] = {"LINEAR EQ",  kAccEQ,     {{"eq_low","LOW"}, {"eq_lowmid","LO-MID"}, {"eq_highmid","HI-MID"}, {"eq_high","HIGH"}}, {}, {}};
    sections[2] = {"BUS COMP",   kAccComp,   {{"comp_thresh","THRESH"}, {"comp_ratio","RATIO"}, {"comp_attack","ATK"}, {"comp_release","REL"}, {"comp_makeup","GAIN"}}, {}, {}};
    sections[3] = {"SOFT CLIP",  kAccClip,   {{"clip_ceiling","CEIL"}, {"clip_drive","DRIVE"}}, {}, {}};
    sections[4] = {"LIMITER",    kAccLim,    {{"lim_ceiling","CEIL"}, {"lim_release","REL"}}, {}, {}};
    sections[5] = {"OUTPUT",     kAccOut,    {{"output_gain","GAIN"}}, {}, {}};

    for (auto& sec : sections)
    {
        for (auto& [paramID, labelText] : sec.params)
        {
            ParamKnob pk;
            pk.slider = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
            pk.slider->setColour(juce::Slider::rotarySliderFillColourId, sec.accent);
            pk.slider->setColour(juce::Slider::thumbColourId, sec.accent);
            pk.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 54, 14);
            pk.slider->setColour(juce::Slider::textBoxTextColourId, kTextDim);
            addAndMakeVisible(pk.slider.get());

            pk.label = std::make_unique<juce::Label>(juce::String(), labelText);
            pk.label->setJustificationType(juce::Justification::centred);
            pk.label->setFont(juce::Font(9.5f));
            pk.label->setColour(juce::Label::textColourId, kTextDim);
            addAndMakeVisible(pk.label.get());

            pk.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                proc.apvts, paramID, *pk.slider);

            sec.knobs.push_back(std::move(pk));
        }
    }

    addAndMakeVisible(compMeter);
    addAndMakeVisible(limMeter);

    // Calculate width: sum of all section widths + meters + padding
    int totalW = kPadding;
    for (auto& sec : sections)
        totalW += (int)sec.params.size() * kKnobW + kPadding + kSectionGap;
    totalW += kMeterW * 2 + kPadding * 2;

    setSize(juce::jmax(totalW, 880), kHeaderH + kSectionH + kPadding * 3);
    setResizable(false, false);
    startTimerHz(24);
}

HottMasterEditor::~HottMasterEditor()
{
    setLookAndFeel(nullptr);
}

void HottMasterEditor::timerCallback()
{
    compMeter.setGainReduction(proc.getCompressorGR());
    limMeter.setGainReduction(std::abs(proc.getLimiterGR()));
}

// ─── Drawing ────────────────────────────────────────────────────────────────

void HottMasterEditor::drawBackground(juce::Graphics& g)
{
    // Subtle vertical gradient — almost flat dark
    auto grad = juce::ColourGradient(juce::Colour(0xFF141420), 0.f, 0.f,
                                     kBgDark, 0.f, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillAll();

    // Subtle noise texture (horizontal lines every 2px)
    g.setColour(juce::Colours::white.withAlpha(0.012f));
    for (int y = 0; y < getHeight(); y += 2)
        g.drawHorizontalLine(y, 0.f, (float)getWidth());
}

void HottMasterEditor::drawHeader(juce::Graphics& g)
{
    auto headerArea = getLocalBounds().removeFromTop(kHeaderH).toFloat();

    // Bottom separator — thin accent line
    g.setColour(juce::Colour(0xFF2A2A3A));
    g.fillRect(headerArea.removeFromBottom(1.f));

    // Brand
    g.setColour(kTextBright);
    g.setFont(juce::Font(18.f, juce::Font::bold));
    g.drawText("HOTT", headerArea.removeFromLeft(80.f).withTrimmedLeft(kPadding), juce::Justification::centredLeft);

    g.setColour(kTextDim.withAlpha(0.5f));
    g.setFont(juce::Font(18.f));
    g.drawText("MASTER", headerArea.removeFromLeft(90.f).withTrimmedLeft(-4.f), juce::Justification::centredLeft);

    // Version tag
    g.setColour(kTextDim.withAlpha(0.3f));
    g.setFont(juce::Font(10.f));
    g.drawText("v1.0", getLocalBounds().removeFromTop(kHeaderH).toFloat().withTrimmedRight(kPadding),
               juce::Justification::centredRight);
}

void HottMasterEditor::drawSectionPanel(juce::Graphics& g, juce::Rectangle<float> bounds,
                                         const juce::String& title, juce::Colour accent)
{
    // Panel background — slightly lighter than bg, rounded
    g.setColour(kBgPanel);
    g.fillRoundedRectangle(bounds, 8.f);

    // Subtle border
    g.setColour(kBgPanelEdge.withAlpha(0.4f));
    g.drawRoundedRectangle(bounds, 8.f, 0.5f);

    // Top accent bar — thin colored line at top of panel
    auto accentBar = bounds.removeFromTop(3.f).reduced(12.f, 0.f);
    g.setColour(accent.withAlpha(0.6f));
    g.fillRoundedRectangle(accentBar.getX(), bounds.getY() - 2.f, accentBar.getWidth(), 2.f, 1.f);

    // Section title
    g.setColour(accent.withAlpha(0.85f));
    g.setFont(juce::Font(10.f, juce::Font::bold));
    g.drawText(title, bounds.getX(), bounds.getY() + 4.f, bounds.getWidth(), 16.f, juce::Justification::centred);
}

void HottMasterEditor::drawSignalChainLine(juce::Graphics& g)
{
    // Thin dotted line connecting sections (signal flow indicator)
    if (sections.size() < 2) return;

    g.setColour(juce::Colour(0xFF3A3A4A));
    float y = sections[0].bounds.getCentreY();

    for (size_t i = 0; i + 1 < sections.size(); ++i)
    {
        float x1 = sections[i].bounds.getRight() + 1.f;
        float x2 = sections[i + 1].bounds.getX() - 1.f;
        if (x2 > x1)
        {
            // Arrow-like dashes
            for (float x = x1; x < x2; x += 4.f)
                g.fillRect(x, y, 2.f, 1.f);
        }
    }
}

void HottMasterEditor::paint(juce::Graphics& g)
{
    drawBackground(g);
    drawHeader(g);

    for (auto& sec : sections)
        drawSectionPanel(g, sec.bounds, sec.name, sec.accent);

    drawSignalChainLine(g);

    // Meter labels
    g.setColour(kTextDim.withAlpha(0.5f));
    g.setFont(juce::Font(8.5f));
    auto meterArea = getLocalBounds().removeFromRight(kMeterW * 2 + kPadding * 2);
    meterArea.removeFromTop(kHeaderH + kPadding);
    g.drawText("COMP", meterArea.removeFromLeft(kMeterW + kPadding).removeFromTop(14), juce::Justification::centred);
    g.drawText("LIM", meterArea.removeFromTop(14), juce::Justification::centred);
}

void HottMasterEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(kHeaderH + kPadding);

    // Reserve right side for meters
    auto meterArea = area.removeFromRight(kMeterW * 2 + kPadding * 2);
    meterArea.removeFromTop(kLabelH + 2);
    auto meterInner = meterArea.reduced(kPadding / 2, 0);
    compMeter.setBounds(meterInner.removeFromLeft(kMeterW).withHeight(kSectionH - kLabelH - 4));
    meterInner.removeFromLeft(kPadding);
    limMeter.setBounds(meterInner.removeFromLeft(kMeterW).withHeight(kSectionH - kLabelH - 4));

    // Layout sections left to right
    int x = kPadding;
    for (auto& sec : sections)
    {
        int numKnobs = (int)sec.knobs.size();
        int secW = numKnobs * kKnobW + kPadding;
        sec.bounds = juce::Rectangle<float>((float)x, (float)(kHeaderH + kPadding), (float)secW, (float)kSectionH);

        for (int i = 0; i < numKnobs; ++i)
        {
            int kx = x + (kPadding / 2) + i * kKnobW;
            int ky = kHeaderH + kPadding + kSectionPadTop;
            sec.knobs[i].label->setBounds(kx, ky, kKnobW, kLabelH);
            sec.knobs[i].slider->setBounds(kx, ky + kLabelH + 2, kKnobW, kKnobH);
        }

        x += secW + kSectionGap;
    }
}
