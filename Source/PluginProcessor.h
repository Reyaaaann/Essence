#pragma once
#include <JuceHeader.h>
#include "Presets.h"
#include "Synth.h"

class EssenceAudioProcessor : public juce::AudioProcessor
{
public:
    EssenceAudioProcessor();
    ~EssenceAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Essence"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override { return (int) presets().size(); }
    int getCurrentProgram() override { return currentPreset.load(); }
    void setCurrentProgram(int i) override { loadPreset(i); }
    const juce::String getProgramName(int i) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    void loadPreset(int index);
    static const std::vector<Essence::Preset>& presets();

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<int> currentPreset { 0 };
    std::atomic<float> level { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    juce::Synthesiser synth;
    juce::Reverb reverb;
    std::vector<float> dBuf[2];
    int dPos = 0;
    float dLp[2] { 0.f, 0.f };
    double sr = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EssenceAudioProcessor)
};
