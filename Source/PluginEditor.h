#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class EssenceLookAndFeel : public juce::LookAndFeel_V4
{
public:
    EssenceLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h, float pos,
                          float startAngle, float endAngle, juce::Slider&) override;
};

class EssenceEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit EssenceEditor(EssenceAudioProcessor&);
    ~EssenceEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseMove(const juce::MouseEvent&) override;

private:
    struct Bokeh { float x, y, r, vy, phase; int col; };
    struct Drop  { float x, y, len, speed; };
    struct Building { float x, w, h; int cols, rows; std::vector<uint8_t> lit; };

    void timerCallback() override;
    void drawScene(juce::Graphics&, float t, float lvl);
    void drawHeader(juce::Graphics&, float t, float lvl);
    void drawMarquee(juce::Graphics&, float t);
    void drawCredits(juce::Graphics&, float t);
    void addKnob(const char* id, const char* name);
    juce::String tickerText() const { return "sincerely your's - Reyaan"; }

    EssenceAudioProcessor& proc;
    EssenceLookAndFeel laf;

    std::vector<std::unique_ptr<juce::Slider>> knobs;
    std::vector<std::unique_ptr<juce::Label>> knobLabels;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAtt;
    juce::ComboBox cbW1, cbW2, cbChord;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> aW1, aW2, aChord;

    std::vector<Bokeh> bokeh;
    std::vector<Drop> drops;
    std::vector<Building> city;

    juce::Rectangle<int> tabsArea, gridArea, creditsBtn, marqueeArea, rightPanel;
    int category = 0, lastPreset = -1, hoverCell = -1;
    bool showCredits = false;
    juce::Random rnd { 7 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EssenceEditor)
};
