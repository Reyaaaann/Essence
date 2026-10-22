#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

const std::vector<Essence::Preset>& EssenceAudioProcessor::presets()
{
    static const std::vector<Essence::Preset> p = Essence::buildPresets();
    return p;
}

AudioProcessorValueTreeState::ParameterLayout EssenceAudioProcessor::createLayout()
{
    AudioProcessorValueTreeState::ParameterLayout l;
    const StringArray waves { "Sine", "Triangle", "Saw", "Square" };
    auto flt = [&l](const char* id, const char* name, float lo, float hi, float def, float centre = -1.f)
    {
        NormalisableRange<float> r(lo, hi, 0.0001f);
        if (centre > 0.f) r.setSkewForCentre(centre);
        l.add(std::make_unique<AudioParameterFloat>(ParameterID { id, 1 }, name, r, def));
    };

    l.add(std::make_unique<AudioParameterChoice>(ParameterID { "w1", 1 }, "Osc 1", waves, 2));
    l.add(std::make_unique<AudioParameterChoice>(ParameterID { "w2", 1 }, "Osc 2", waves, 2));
    l.add(std::make_unique<AudioParameterChoice>(ParameterID { "chord", 1 }, "Chord",
          StringArray { "Off", "Min7", "Min9", "Maj7", "Add9", "7sus" }, 0));
    l.add(std::make_unique<AudioParameterInt>(ParameterID { "oct", 1 }, "Octave", -2, 1, 0));

    flt("det",   "Detune",       0.f, 50.f, 8.f);
    flt("o2lvl", "Osc 2 Level",  0.f, 1.f, 0.6f);
    flt("sub",   "Sub",          0.f, 1.f, 0.2f);
    flt("noise", "Noise",        0.f, 1.f, 0.f);
    flt("cut",   "Cutoff",       20.f, 18000.f, 4000.f, 1200.f);
    flt("res",   "Resonance",    0.f, 1.f, 0.15f);
    flt("fenv",  "Filter Env",   0.f, 1.f, 0.3f);
    flt("atk",   "Attack",       0.001f, 4.f, 0.01f, 0.25f);
    flt("dec",   "Decay",        0.01f, 4.f, 0.5f, 0.4f);
    flt("sus",   "Sustain",      0.f, 1.f, 0.7f);
    flt("rel",   "Release",      0.005f, 5.f, 0.5f, 0.5f);
    flt("lfoR",  "LFO Rate",     0.1f, 20.f, 3.f, 3.f);
    flt("lfoD",  "LFO Depth",    0.f, 1.f, 0.f);
    flt("drive", "Drive",        0.f, 1.f, 0.2f);
    flt("rev",   "Reverb",       0.f, 1.f, 0.2f);
    flt("dly",   "Delay",        0.f, 1.f, 0.1f);
    flt("vol",   "Volume",       0.f, 1.f, 0.75f);
    return l;
}

EssenceAudioProcessor::EssenceAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "STATE", createLayout())
{
    for (int i = 0; i < 16; ++i) synth.addVoice(new EssenceVoice(apvts));
    synth.addSound(new EssenceSound());
    loadPreset(0);
}

void EssenceAudioProcessor::prepareToPlay(double sampleRate, int)
{
    sr = sampleRate;
    synth.setCurrentPlaybackSampleRate(sampleRate);
    reverb.setSampleRate(sampleRate);
    reverb.reset();
    for (auto& b : dBuf) b.assign((size_t) (sampleRate * 3.0), 0.f);
    dPos = 0;
    dLp[0] = dLp[1] = 0.f;
}

bool EssenceAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == AudioChannelSet::stereo();
}

void EssenceAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();
    buffer.clear();

    // Chord mode: expand every incoming note into a chord
    static const std::vector<int> chords[6] = { {0}, {0,3,7,10}, {0,3,7,10,14}, {0,4,7,11}, {0,3,7,14}, {0,5,7,10} };
    const int chordType = (int) *apvts.getRawParameterValue("chord");
    MidiBuffer expanded;
    if (chordType == 0) expanded = midi;
    else
        for (const auto m : midi)
        {
            auto msg = m.getMessage();
            if (msg.isNoteOn() || msg.isNoteOff())
                for (int iv : chords[chordType])
                {
                    int note = msg.getNoteNumber() + iv;
                    if (note > 127) continue;
                    expanded.addEvent(msg.isNoteOn() ? MidiMessage::noteOn(msg.getChannel(), note, msg.getVelocity())
                                                     : MidiMessage::noteOff(msg.getChannel(), note), m.samplePosition);
                }
            else expanded.addEvent(msg, m.samplePosition);
        }

    synth.renderNextBlock(buffer, expanded, 0, n);

    // Delay (dotted 8th, follows host tempo)
    double bpm = 120.0;
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
            if (auto b = pos->getBpm()) bpm = *b;
    const int dSize = (int) dBuf[0].size();
    const int dLen = jlimit(1, dSize - 1, (int) (sr * 60.0 / jmax(40.0, bpm) * 0.75));
    const float dMix = *apvts.getRawParameterValue("dly") * 0.6f;
    if (dSize > 0)
        for (int i = 0; i < n; ++i)
        {
            const int rp = (dPos - dLen + dSize) % dSize;
            for (int ch = 0; ch < 2; ++ch)
            {
                float* d = buffer.getWritePointer(ch);
                const float in = d[i];
                dLp[ch] += 0.35f * (dBuf[ch][(size_t) rp] - dLp[ch]);
                dBuf[ch][(size_t) dPos] = in + dLp[ch] * 0.45f;
                d[i] = in + dLp[ch] * dMix;
            }
            dPos = (dPos + 1) % dSize;
        }

    // Reverb
    Reverb::Parameters rp;
    rp.roomSize = 0.78f; rp.damping = 0.5f; rp.width = 1.f; rp.freezeMode = 0.f;
    rp.wetLevel = *apvts.getRawParameterValue("rev") * 0.45f; rp.dryLevel = 1.f;
    reverb.setParameters(rp);
    reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), n);

    // Master
    const float vol = *apvts.getRawParameterValue("vol");
    for (int ch = 0; ch < 2; ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        for (int i = 0; i < n; ++i) d[i] = std::tanh(d[i] * vol * 1.4f);
    }
    level.store(buffer.getMagnitude(0, n));
}

const String EssenceAudioProcessor::getProgramName(int i)
{
    if (i < 0 || i >= (int) presets().size()) return {};
    auto& p = presets()[(size_t) i];
    return String(Essence::categoryNames[p.category]) + " - " + p.name;
}

void EssenceAudioProcessor::loadPreset(int index)
{
    index = jlimit(0, (int) presets().size() - 1, index);
    currentPreset = index;
    for (auto& kv : presets()[(size_t) index].values)
        if (auto* p = apvts.getParameter(kv.first))
            p->setValueNotifyingHost(p->convertTo0to1(kv.second));
}

void EssenceAudioProcessor::getStateInformation(MemoryBlock& dest)
{
    auto state = apvts.copyState();
    state.setProperty("preset", currentPreset.load(), nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary(*xml, dest);
}

void EssenceAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(apvts.state.getType()))
        {
            auto vt = ValueTree::fromXml(*xml);
            currentPreset = (int) vt.getProperty("preset", 0);
            apvts.replaceState(vt);
        }
}

AudioProcessorEditor* EssenceAudioProcessor::createEditor() { return new EssenceEditor(*this); }
AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new EssenceAudioProcessor(); }
