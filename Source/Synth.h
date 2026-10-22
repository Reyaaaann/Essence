#pragma once
#include <JuceHeader.h>

struct EssenceSound : public juce::SynthesiserSound
{
    bool appliesToNote(int) override    { return true; }
    bool appliesToChannel(int) override { return true; }
};

struct SVF
{
    float ic1 = 0.f, ic2 = 0.f;
    void reset() { ic1 = ic2 = 0.f; }
    float lowpass(float x, float a1, float a2, float a3)
    {
        float v3 = x - ic2;
        float v1 = a1 * ic1 + a2 * v3;
        float v2 = ic2 + a2 * ic1 + a3 * v3;
        ic1 = 2.f * v1 - ic1;
        ic2 = 2.f * v2 - ic2;
        return v2;
    }
};

class EssenceVoice : public juce::SynthesiserVoice
{
public:
    explicit EssenceVoice(juce::AudioProcessorValueTreeState& s)
    {
        auto g = [&s](const char* id) { return s.getRawParameterValue(id); };
        w1 = g("w1"); w2 = g("w2"); det = g("det"); o2 = g("o2lvl"); sub = g("sub"); noise = g("noise");
        cut = g("cut"); res = g("res"); fenv = g("fenv"); atk = g("atk"); dec = g("dec"); sus = g("sus");
        rel = g("rel"); lfoR = g("lfoR"); lfoD = g("lfoD"); drive = g("drive"); oct = g("oct");
    }

    bool canPlaySound(juce::SynthesiserSound* s) override { return dynamic_cast<EssenceSound*>(s) != nullptr; }

    void startNote(int midiNote, float velocity, juce::SynthesiserSound*, int) override
    {
        baseFreq = (float) juce::MidiMessage::getMidiNoteInHertz(midiNote);
        level = velocity * 0.9f;
        ph1 = ph2 = phs = lfoPhase = 0.f;
        sr = (float) getSampleRate();
        ampEnv.setSampleRate(sr);
        fltEnv.setSampleRate(sr);
        ampEnv.setParameters({ atk->load(), dec->load(), sus->load(), rel->load() });
        fltEnv.setParameters({ 0.002f, dec->load(), 0.f, rel->load() });
        ampEnv.noteOn();
        fltEnv.noteOn();
        L.reset(); R.reset();
    }

    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff) { ampEnv.noteOff(); fltEnv.noteOff(); }
        else { ampEnv.reset(); fltEnv.reset(); clearCurrentNote(); }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioBuffer<float>& out, int start, int num) override
    {
        if (!ampEnv.isActive()) { clearCurrentNote(); return; }

        const int wv1 = (int) w1->load(), wv2 = (int) w2->load();
        const float octMul = std::pow(2.f, oct->load());
        const float f1 = baseFreq * octMul;
        const float f2 = f1 * std::pow(2.f, det->load() / 1200.f);
        const float d1 = f1 / sr, d2 = f2 / sr, ds = (f1 * 0.5f) / sr;
        const float o2l = o2->load(), subL = sub->load(), noiseL = noise->load();
        const float cutHz = cut->load(), fenvAmt = fenv->load();
        const float lfoRate = lfoR->load() / sr, lfoDepth = lfoD->load();
        const float q = 0.6f + res->load() * 9.f, k = 1.f / q;
        const float drv = 1.f + drive->load() * 6.f, drvNorm = 1.f / (1.f + drive->load());
        const float maxFc = juce::jmin(18000.f, 0.45f * sr);
        const int nCh = out.getNumChannels();

        for (int i = 0; i < num; ++i)
        {
            const float a = ampEnv.getNextSample();
            if (!ampEnv.isActive()) { clearCurrentNote(); break; }
            const float fe = fltEnv.getNextSample();

            const float lfo = std::sin(juce::MathConstants<float>::twoPi * lfoPhase);
            lfoPhase += lfoRate; if (lfoPhase >= 1.f) lfoPhase -= 1.f;

            const float s1 = osc(wv1, ph1, d1);
            const float s2 = osc(wv2, ph2, d2) * o2l;
            const float sb = std::sin(juce::MathConstants<float>::twoPi * phs) * subL;
            ph1 = adv(ph1, d1); ph2 = adv(ph2, d2); phs = adv(phs, ds);

            const float nL = noiseL * (rng.nextFloat() * 2.f - 1.f);
            const float nR = noiseL * (rng.nextFloat() * 2.f - 1.f);

            float xl = s1 * 0.80f + s2 * 0.40f + sb + nL;
            float xr = s1 * 0.40f + s2 * 0.80f + sb + nR;

            const float mod = fenvAmt * fe * 5.f + lfoDepth * lfo * 3.f;
            const float fc = juce::jlimit(20.f, maxFc, cutHz * std::pow(2.f, mod));
            const float g = std::tan(juce::MathConstants<float>::pi * fc / sr);
            const float a1 = 1.f / (1.f + g * (g + k)), a2 = g * a1, a3 = g * a2;

            xl = L.lowpass(xl, a1, a2, a3);
            xr = R.lowpass(xr, a1, a2, a3);
            xl = std::tanh(xl * drv) * drvNorm;
            xr = std::tanh(xr * drv) * drvNorm;

            const float gain = a * level * 0.30f;
            out.addSample(0, start + i, xl * gain);
            if (nCh > 1) out.addSample(1, start + i, xr * gain);
        }
    }

private:
    static float adv(float p, float d) { p += d; return p >= 1.f ? p - 1.f : p; }

    static float blep(float t, float dt)
    {
        if (t < dt)       { t /= dt; return t + t - t * t - 1.f; }
        if (t > 1.f - dt) { t = (t - 1.f) / dt; return t * t + t + t + 1.f; }
        return 0.f;
    }

    static float osc(int wave, float p, float dt)
    {
        switch (wave)
        {
            case 0:  return std::sin(juce::MathConstants<float>::twoPi * p);
            case 1:  return 4.f * std::abs(p - 0.5f) - 1.f;
            case 2:  return 2.f * p - 1.f - blep(p, dt);
            default: { float p2 = p + 0.5f; if (p2 >= 1.f) p2 -= 1.f;
                       return (p < 0.5f ? 1.f : -1.f) + blep(p, dt) - blep(p2, dt); }
        }
    }

    std::atomic<float> *w1, *w2, *det, *o2, *sub, *noise, *cut, *res, *fenv, *atk, *dec, *sus,
                       *rel, *lfoR, *lfoD, *drive, *oct;
    juce::ADSR ampEnv, fltEnv;
    SVF L, R;
    juce::Random rng;
    float baseFreq = 440.f, level = 0.f, sr = 44100.f, ph1 = 0.f, ph2 = 0.f, phs = 0.f, lfoPhase = 0.f;
};
