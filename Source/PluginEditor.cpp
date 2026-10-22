#include "PluginEditor.h"

using namespace juce;

namespace
{
    const Colour kBlack  (0xff06040c);
    const Colour kDeep   (0xff140a2a);
    const Colour kViolet (0xff7a3cff);
    const Colour kPink   (0xffff4f9a);
    const Colour kOrange (0xffff7a3c);
    const Colour kGold   (0xffe9b949);
    const Colour kBlue   (0xff3c8cff);
    const int W = 1100, H = 680, kMarqueeH = 54;
}

// ---------------------------------------------------------------- look & feel
EssenceLookAndFeel::EssenceLookAndFeel()
{
    setColour(ComboBox::backgroundColourId, Colour(0xcc0d0818));
    setColour(ComboBox::textColourId, kGold);
    setColour(ComboBox::outlineColourId, kGold.withAlpha(0.5f));
    setColour(ComboBox::arrowColourId, kGold);
    setColour(PopupMenu::backgroundColourId, Colour(0xff0d0818));
    setColour(PopupMenu::textColourId, Colours::white);
    setColour(PopupMenu::highlightedBackgroundColourId, kViolet);
    setColour(Slider::textBoxTextColourId, Colours::white.withAlpha(0.85f));
    setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
}

void EssenceLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int w, int h, float pos,
                                          float start, float end, Slider&)
{
    auto b = Rectangle<float>((float) x, (float) y, (float) w, (float) h).reduced(5.f);
    const float r = jmin(b.getWidth(), b.getHeight()) * 0.5f;
    const auto c = b.getCentre();
    const float angle = start + pos * (end - start);

    g.setColour(Colours::black.withAlpha(0.6f));
    g.fillEllipse(c.x - r, c.y - r, r * 2, r * 2);
    g.setColour(Colours::white.withAlpha(0.08f));
    g.drawEllipse(c.x - r, c.y - r, r * 2, r * 2, 1.f);

    Path track, val;
    track.addCentredArc(c.x, c.y, r - 3, r - 3, 0, start, end, true);
    val.addCentredArc(c.x, c.y, r - 3, r - 3, 0, start, angle, true);
    g.setColour(Colours::white.withAlpha(0.12f));
    g.strokePath(track, PathStrokeType(3.f, PathStrokeType::curved, PathStrokeType::rounded));
    g.setGradientFill(ColourGradient(kGold, c.x - r, c.y + r, kPink, c.x + r, c.y - r, false));
    g.strokePath(val, PathStrokeType(3.5f, PathStrokeType::curved, PathStrokeType::rounded));

    Path p;
    p.addRoundedRectangle(-1.5f, -r + 7.f, 3.f, r * 0.42f, 1.5f);
    p.applyTransform(AffineTransform::rotation(angle).translated(c.x, c.y));
    g.setColour(Colours::white);
    g.fillPath(p);
}

// ---------------------------------------------------------------- editor
EssenceEditor::EssenceEditor(EssenceAudioProcessor& p) : AudioProcessorEditor(&p), proc(p)
{
    setLookAndFeel(&laf);
    setSize(W, H);

    const StringArray waves { "Sine", "Triangle", "Saw", "Square" };
    cbW1.addItemList(waves, 1); cbW2.addItemList(waves, 1);
    cbChord.addItemList({ "Off", "Min7", "Min9", "Maj7", "Add9", "7sus" }, 1);
    for (auto* c : { &cbW1, &cbW2, &cbChord }) addAndMakeVisible(c);
    aW1 = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(proc.apvts, "w1", cbW1);
    aW2 = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(proc.apvts, "w2", cbW2);
    aChord = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(proc.apvts, "chord", cbChord);

    addKnob("cut", "CUTOFF");   addKnob("res", "RESO");   addKnob("fenv", "FLT ENV"); addKnob("drive", "DRIVE");
    addKnob("rev", "REVERB");   addKnob("dly", "DELAY");
    addKnob("atk", "ATTACK");   addKnob("dec", "DECAY");  addKnob("sus", "SUSTAIN");  addKnob("rel", "RELEASE");
    addKnob("lfoR", "LFO RATE"); addKnob("lfoD", "LFO DEPTH");
    addKnob("det", "DETUNE");   addKnob("o2lvl", "OSC 2"); addKnob("sub", "SUB");     addKnob("noise", "NOISE");
    addKnob("oct", "OCTAVE");   addKnob("vol", "VOLUME");

    for (int i = 0; i < 38; ++i)
        bokeh.push_back({ rnd.nextFloat() * W, rnd.nextFloat() * H, 6.f + rnd.nextFloat() * 26.f,
                          6.f + rnd.nextFloat() * 14.f, rnd.nextFloat() * 6.28f, rnd.nextInt(5) });
    for (int i = 0; i < 110; ++i)
        drops.push_back({ rnd.nextFloat() * (W + 200), rnd.nextFloat() * H, 10.f + rnd.nextFloat() * 18.f,
                          380.f + rnd.nextFloat() * 320.f });

    float x = -10.f;
    while (x < W)
    {
        Building b;
        b.x = x; b.w = 34.f + rnd.nextFloat() * 44.f;
        const float mid = 1.f - std::abs(x - W * 0.55f) / (W * 0.6f);
        b.h = 70.f + rnd.nextFloat() * 90.f + jmax(0.f, mid) * 110.f;
        b.cols = jmax(2, (int) (b.w / 9.f)); b.rows = jmax(3, (int) (b.h / 12.f));
        b.lit.resize((size_t) (b.cols * b.rows));
        for (auto& l : b.lit) l = rnd.nextFloat() < 0.35f ? 1 : 0;
        city.push_back(std::move(b));
        x += b.w + 3.f + rnd.nextFloat() * 5.f;
    }
    resized();
    startTimerHz(30);
}

EssenceEditor::~EssenceEditor() { stopTimer(); setLookAndFeel(nullptr); }

void EssenceEditor::addKnob(const char* id, const char* name)
{
    auto s = std::make_unique<Slider>(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow);
    s->setRotaryParameters(MathConstants<float>::pi * 1.25f, MathConstants<float>::pi * 2.75f, true);
    s->setTextBoxStyle(Slider::TextBoxBelow, false, 64, 14);
    s->setNumDecimalPlacesToDisplay(String(id) == "cut" ? 0 : 2);
    s->setDoubleClickReturnValue(true, proc.apvts.getParameter(id)->convertFrom0to1(
                                           proc.apvts.getParameter(id)->getDefaultValue()));
    addAndMakeVisible(*s);
    sliderAtt.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, id, *s));
    auto l = std::make_unique<Label>(String(), name);
    l->setJustificationType(Justification::centred);
    l->setFont(Font(11.f, Font::bold));
    l->setColour(Label::textColourId, kGold);
    l->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(*l);
    knobs.push_back(std::move(s)); knobLabels.push_back(std::move(l));
}

void EssenceEditor::resized()
{
    creditsBtn = { W - 120, 24, 96, 28 };
    tabsArea = { 24, 122, 580, 34 };
    gridArea = { 24, 166, 580, H - 166 - kMarqueeH - 22 };
    marqueeArea = { 0, H - kMarqueeH, W, kMarqueeH };
    rightPanel = { 626, 122, 450, H - 122 - kMarqueeH - 22 };

    cbW1.setBounds(646, 166, 130, 28);
    cbW2.setBounds(788, 166, 130, 28);
    cbChord.setBounds(930, 166, 130, 28);

    const int cols = 6, cw = 71, chh = 118, x0 = 641, y0 = 206;
    for (size_t i = 0; i < knobs.size(); ++i)
    {
        const int cx = x0 + (int) (i % cols) * cw, cy = y0 + (int) (i / cols) * chh;
        knobLabels[i]->setBounds(cx, cy, cw, 16);
        knobs[i]->setBounds(cx, cy + 16, cw, 98);
    }
}

void EssenceEditor::timerCallback()
{
    const float dt = 1.f / 30.f;
    const float lvl = proc.level.load();
    for (auto& b : bokeh)
    {
        b.y -= b.vy * dt; b.phase += dt * 0.6f; b.x += std::sin(b.phase) * 0.25f;
        if (b.y < -40.f) { b.y = H + 40.f; b.x = rnd.nextFloat() * W; }
    }
    for (auto& d : drops)
    {
        d.y += d.speed * dt; d.x -= d.speed * 0.18f * dt;
        if (d.y > H) { d.y = -20.f; d.x = rnd.nextFloat() * (W + 200); }
    }
    for (int k = 0; k < 8; ++k)               // flickering windows
    {
        auto& b = city[(size_t) rnd.nextInt((int) city.size())];
        if (!b.lit.empty()) b.lit[(size_t) rnd.nextInt((int) b.lit.size())] ^= 1;
    }
    const int cur = proc.currentPreset.load();
    if (cur != lastPreset) { lastPreset = cur; category = cur / Essence::perCategory; }
    ignoreUnused(lvl);
    repaint();
}

void EssenceEditor::drawScene(Graphics& g, float t, float lvl)
{
    // night sky (Trapsoul black -> violet, Honestly Nevermind glow on the horizon)
    g.setGradientFill(ColourGradient(kBlack, 0, 0, Colour(0xff2a0f4d), 0, (float) H, false));
    g.fillAll();
    g.setGradientFill(ColourGradient(kOrange.withAlpha(0.32f + lvl * 0.3f), W * 0.78f, H * 0.78f,
                                     Colours::transparentBlack, W * 0.78f - 520.f * (1.f + lvl * 0.2f), H * 0.78f, true));
    g.fillAll();
    g.setGradientFill(ColourGradient(kViolet.withAlpha(0.30f), W * 0.15f, H * 0.30f,
                                     Colours::transparentBlack, W * 0.15f + 420.f, H * 0.30f, true));
    g.fillAll();

    // bokeh lights
    const Colour cols[5] = { kViolet, kPink, kGold, kBlue, kOrange };
    for (auto& b : bokeh)
    {
        const float pulse = 1.f + 0.15f * std::sin(t * 1.3f + b.phase) + lvl * 0.8f;
        const float r = b.r * pulse;
        g.setColour(cols[b.col].withAlpha(0.10f));
        g.fillEllipse(b.x - r, b.y - r, r * 2, r * 2);
        g.setColour(cols[b.col].withAlpha(0.22f));
        g.drawEllipse(b.x - r, b.y - r, r * 2, r * 2, 1.f);
    }

    // CN-tower style silhouette
    const float baseY = (float) H - kMarqueeH, tx = 190.f;
    Path tower;
    tower.startNewSubPath(tx - 9, baseY); tower.lineTo(tx - 2.5f, baseY - 300); tower.lineTo(tx + 2.5f, baseY - 300);
    tower.lineTo(tx + 9, baseY); tower.closeSubPath();
    tower.addEllipse(tx - 20, baseY - 330, 40, 14);
    tower.addRectangle(tx - 1.2f, baseY - 420, 2.4f, 95.f);
    g.setColour(Colour(0xff0a0614)); g.fillPath(tower);
    g.setColour(Colour(0xffff2b3d).withAlpha(0.35f + 0.65f * (0.5f + 0.5f * std::sin(t * 3.f))));
    g.fillEllipse(tx - 3, baseY - 424, 6, 6);

    // skyline with flickering windows
    for (auto& b : city)
    {
        const float top = baseY - b.h;
        g.setColour(Colour(0xff09050f)); g.fillRect(b.x, top, b.w, b.h);
        g.setColour(kViolet.withAlpha(0.18f)); g.fillRect(b.x, top, b.w, 1.5f);
        for (int r = 0; r < b.rows; ++r)
            for (int c = 0; c < b.cols; ++c)
                if (b.lit[(size_t) (r * b.cols + c)])
                {
                    g.setColour((((r + c) & 3) == 0 ? kOrange : kGold).withAlpha(0.55f));
                    g.fillRect(b.x + 4.f + c * 9.f, top + 6.f + r * 12.f, 4.f, 6.f);
                }
    }

    // rain
    g.setColour(Colours::white.withAlpha(0.16f));
    for (auto& d : drops) g.drawLine(d.x, d.y, d.x - d.len * 0.18f, d.y + d.len, 1.f);

    // slow floating signature in the sky
    const float sy = 250.f + std::sin(t * 0.5f) * 14.f;
    g.setColour(Colours::white.withAlpha(0.05f + 0.03f * std::sin(t * 0.8f)));
    g.setFont(Font(46.f, Font::italic));
    g.drawText(tickerText(), Rectangle<float>(0.f, sy, (float) W, 60.f), Justification::centred);

    // vignette
    g.setGradientFill(ColourGradient(Colours::transparentBlack, W * 0.5f, H * 0.5f,
                                     Colours::black.withAlpha(0.7f), 0, 0, true));
    g.fillAll();
}

void EssenceEditor::drawHeader(Graphics& g, float t, float lvl)
{
    // moving ribbons
    for (int k = 0; k < 3; ++k)
    {
        Path p;
        for (int x = 0; x <= W; x += 6)
        {
            const float y = 56.f + std::sin(x * 0.011f + t * (0.8f + k * 0.35f) + k * 2.1f) * (14.f + lvl * 55.f + k * 4.f);
            if (x == 0) p.startNewSubPath((float) x, y); else p.lineTo((float) x, y);
        }
        g.setColour((k == 0 ? kViolet : k == 1 ? kPink : kGold).withAlpha(0.35f));
        g.strokePath(p, PathStrokeType(2.f));
    }

    const Font tf = Font(58.f, Font::bold).withExtraKerningFactor(0.14f);
    g.setFont(tf);
    g.setColour(kViolet.withAlpha(0.35f));
    for (auto o : { -2.f, 2.f }) g.drawText("ESSENCE", Rectangle<float>(24.f + o, 14.f + o, 420.f, 66.f), Justification::centredLeft);
    g.setGradientFill(ColourGradient(Colours::white, 24, 14, kGold, 24, 74, false));
    g.drawText("ESSENCE", Rectangle<float>(24.f, 14.f, 420.f, 66.f), Justification::centredLeft);

    g.setFont(Font(12.f, Font::bold).withExtraKerningFactor(0.3f));
    g.setColour(kGold);
    g.drawText("DARK R&B  /  TRAPSOUL  SYNTHESIZER", 26, 82, 420, 18, Justification::centredLeft);

    // current preset readout
    const int cur = proc.currentPreset.load();
    const auto& pr = EssenceAudioProcessor::presets()[(size_t) cur];
    g.setFont(Font(12.f, Font::bold).withExtraKerningFactor(0.25f));
    g.setColour(kPink);
    g.drawText(String(Essence::categoryNames[pr.category]) + "  " + String(cur % Essence::perCategory + 1).paddedLeft('0', 2),
               460, 26, 520, 16, Justification::centredRight);
    g.setFont(Font(28.f, Font::bold));
    g.setColour(Colours::white);
    g.drawText(pr.name, 460, 42, 520, 36, Justification::centredRight);

    // credits button
    g.setColour(Colour(0xaa0d0818)); g.fillRoundedRectangle(creditsBtn.toFloat(), 14.f);
    g.setColour(kGold.withAlpha(0.7f)); g.drawRoundedRectangle(creditsBtn.toFloat(), 14.f, 1.2f);
    g.setFont(Font(11.f, Font::bold).withExtraKerningFactor(0.2f));
    g.drawText("CREDITS", creditsBtn, Justification::centred);
}

void EssenceEditor::drawMarquee(Graphics& g, float t)
{
    auto bar = marqueeArea.toFloat();
    g.setColour(Colour(0xdd05030a)); g.fillRect(bar);
    g.setGradientFill(ColourGradient(kViolet, 0, bar.getY(), kGold, (float) W, bar.getY(), false));
    g.fillRect(bar.getX(), bar.getY(), bar.getWidth(), 2.f);

    const Font f = Font(24.f, Font::bold | Font::italic).withExtraKerningFactor(0.06f);
    const String unit = tickerText() + "      *      ";
    const float uw = f.getStringWidthFloat(unit);
    const float x0 = -std::fmod(t * 70.f, uw);

    Graphics::ScopedSaveState ss(g);
    g.reduceClipRegion(marqueeArea);
    g.setFont(f);
    for (float x = x0; x < W; x += uw)
    {
        auto r = Rectangle<float>(x, bar.getY(), uw, bar.getHeight());
        g.setColour(kViolet.withAlpha(0.35f));
        g.drawText(unit, r.translated(0.f, 2.f), Justification::centredLeft, false);
        g.setGradientFill(ColourGradient(kGold, 0, bar.getY(), kPink, (float) W, bar.getY(), false));
        g.drawText(unit, r, Justification::centredLeft, false);
    }
    const float sweep = std::fmod(t * 320.f, (float) W + 400.f) - 200.f;
    g.setGradientFill(ColourGradient(Colours::transparentWhite, sweep - 120, 0,
                                     Colours::white.withAlpha(0.13f), sweep, 0, false));
    g.fillRect(sweep - 120, bar.getY(), 120.f, bar.getHeight());
}

void EssenceEditor::drawCredits(Graphics& g, float t)
{
    g.setColour(Colours::black.withAlpha(0.86f)); g.fillAll();
    auto box = Rectangle<float>(0, 0, 620, 380).withCentre({ W * 0.5f, H * 0.5f - 20.f });
    g.setColour(Colour(0xf00d0818)); g.fillRoundedRectangle(box, 18.f);
    g.setGradientFill(ColourGradient(kViolet, box.getX(), box.getY(), kGold, box.getRight(), box.getBottom(), false));
    g.drawRoundedRectangle(box, 18.f, 2.f);

    g.setFont(Font(46.f, Font::bold).withExtraKerningFactor(0.14f));
    g.setGradientFill(ColourGradient(Colours::white, 0, box.getY() + 20, kGold, 0, box.getY() + 70, false));
    g.drawText("ESSENCE", box.withTrimmedTop(24).withHeight(58), Justification::centred);

    g.setFont(Font(13.f, Font::bold).withExtraKerningFactor(0.25f));
    g.setColour(kPink);
    g.drawText("v1.0.0  -  DARK R&B / TRAPSOUL SYNTH", box.withTrimmedTop(86).withHeight(20), Justification::centred);

    g.setColour(kGold); g.setFont(Font(13.f, Font::bold).withExtraKerningFactor(0.3f));
    g.drawText("CREATED BY", box.withTrimmedTop(140).withHeight(20), Justification::centred);
    g.setColour(Colours::white); g.setFont(Font(40.f, Font::bold | Font::italic));
    g.drawText("Reyaan", box.withTrimmedTop(162).withHeight(54), Justification::centred);

    g.setColour(Colours::white.withAlpha(0.75f)); g.setFont(Font(14.f));
    g.drawFittedText("Sound design, synth engine and interface by Reyaan.\n"
                     "180 sounds inspired by the late-night, moody R&B and trapsoul era of the 2010s and 2020s.\n"
                     "Built with JUCE. Not affiliated with or endorsed by any artist.",
                     box.withTrimmedTop(228).withHeight(80).reduced(40, 0).toNearestInt(), Justification::centred, 4);

    g.setColour(kGold.withAlpha(0.6f + 0.4f * std::sin(t * 2.f)));
    g.setFont(Font(20.f, Font::italic));
    g.drawText(tickerText(), box.withTrimmedTop(320).withHeight(34), Justification::centred);
}

void EssenceEditor::paint(Graphics& g)
{
    const float t = (float) Time::getMillisecondCounterHiRes() * 0.001f;
    const float lvl = jlimit(0.f, 1.f, proc.level.load());
    drawScene(g, t, lvl);
    drawHeader(g, t, lvl);

    // panels
    for (auto r : { gridArea.expanded(0, 0), rightPanel })
    {
        g.setColour(Colour(0x99080510)); g.fillRoundedRectangle(r.toFloat(), 14.f);
        g.setColour(kGold.withAlpha(0.25f)); g.drawRoundedRectangle(r.toFloat(), 14.f, 1.f);
    }

    // category tabs
    const float tw = tabsArea.getWidth() / (float) Essence::NumCategories;
    g.setFont(Font(11.f, Font::bold).withExtraKerningFactor(0.08f));
    for (int c = 0; c < Essence::NumCategories; ++c)
    {
        auto r = Rectangle<float>(tabsArea.getX() + c * tw, (float) tabsArea.getY(), tw, (float) tabsArea.getHeight());
        const bool on = c == category;
        if (on)
        {
            g.setGradientFill(ColourGradient(kViolet.withAlpha(0.8f), r.getX(), r.getY(), kPink.withAlpha(0.8f), r.getRight(), r.getBottom(), false));
            g.fillRoundedRectangle(r.reduced(2.f), 8.f);
        }
        g.setColour(on ? Colours::white : kGold.withAlpha(0.8f));
        g.drawText(Essence::categoryNames[c], r, Justification::centred);
    }

    // preset grid (4 x 5)
    const int cols = 4, rows = 5;
    const float cw = (gridArea.getWidth() - 20.f) / cols, ch = (gridArea.getHeight() - 20.f) / rows;
    for (int i = 0; i < Essence::perCategory; ++i)
    {
        auto r = Rectangle<float>(gridArea.getX() + 10.f + (i % cols) * cw, gridArea.getY() + 10.f + (i / cols) * ch, cw, ch).reduced(4.f);
        const int idx = category * Essence::perCategory + i;
        const bool sel = idx == proc.currentPreset.load(), hov = i == hoverCell;
        if (sel)
        {
            g.setGradientFill(ColourGradient(kViolet, r.getX(), r.getY(), kPink, r.getRight(), r.getBottom(), false));
            g.fillRoundedRectangle(r, 10.f);
        }
        else
        {
            g.setColour(hov ? Colour(0xff2a1850) : Colour(0xff150d28)); g.fillRoundedRectangle(r, 10.f);
            g.setColour(kGold.withAlpha(hov ? 0.7f : 0.2f)); g.drawRoundedRectangle(r, 10.f, 1.f);
        }
        g.setColour(sel ? Colours::white.withAlpha(0.7f) : kPink.withAlpha(0.8f));
        g.setFont(Font(10.f, Font::bold));
        g.drawText(String(i + 1).paddedLeft('0', 2), r.reduced(8.f, 5.f), Justification::topLeft);
        g.setColour(sel ? Colours::white : Colour(0xffeadfff));
        g.setFont(Font(14.f, Font::bold));
        g.drawFittedText(EssenceAudioProcessor::presets()[(size_t) idx].name, r.reduced(8.f, 6.f).toNearestInt(),
                         Justification::centredBottom, 2);
    }

    // right panel headings
    g.setColour(kGold); g.setFont(Font(11.f, Font::bold).withExtraKerningFactor(0.2f));
    g.drawText("OSC 1", 646, 146, 130, 16, Justification::centredLeft);
    g.drawText("OSC 2", 788, 146, 130, 16, Justification::centredLeft);
    g.drawText("CHORD", 930, 146, 130, 16, Justification::centredLeft);

    drawMarquee(g, t);
    if (showCredits) drawCredits(g, t);
}

void EssenceEditor::mouseMove(const MouseEvent& e)
{
    hoverCell = -1;
    if (gridArea.contains(e.getPosition()))
    {
        const float cw = (gridArea.getWidth() - 20.f) / 4, ch = (gridArea.getHeight() - 20.f) / 5;
        const int cx = (int) ((e.x - gridArea.getX() - 10.f) / cw), cy = (int) ((e.y - gridArea.getY() - 10.f) / ch);
        if (cx >= 0 && cx < 4 && cy >= 0 && cy < 5) hoverCell = cy * 4 + cx;
    }
}

void EssenceEditor::mouseDown(const MouseEvent& e)
{
    if (showCredits) { showCredits = false; return; }
    if (creditsBtn.contains(e.getPosition())) { showCredits = true; return; }
    if (tabsArea.contains(e.getPosition()))
    {
        category = jlimit(0, Essence::NumCategories - 1,
                          (int) ((e.x - tabsArea.getX()) / (tabsArea.getWidth() / (float) Essence::NumCategories)));
        return;
    }
    mouseMove(e);
    if (hoverCell >= 0) proc.loadPreset(category * Essence::perCategory + hoverCell);
}
