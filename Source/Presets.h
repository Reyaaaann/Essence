#pragma once
#include <juce_core/juce_core.h>
#include <vector>
#include <utility>

// ESSENCE - 9 categories x 20 sounds = 180 presets.
namespace Essence
{
enum Category { Bass, Pad, Lead, Synth, Keys, FX, Hit, Chord, Wobbler, NumCategories };
inline const char* categoryNames[] = { "BASS", "PADS", "LEADS", "SYNTHS", "KEYS", "FX", "HITS", "CHORDS", "WOBBLERS" };
constexpr int perCategory = 20;

struct Preset
{
    juce::String name;
    int category;
    std::vector<std::pair<juce::String, float>> values;
};

// waves: 0 sine, 1 triangle, 2 saw, 3 square
// x = category specific: Bass/Synth filter-env amount, Pad lfo depth, Lead vibrato rate,
//     Keys decay, FX noise, Hit decay, Chord type (1 min7, 2 min9, 3 maj7, 4 add9, 5 7sus), Wobbler lfo rate (Hz)
struct Row { const char* name; int w1, w2; float cut, res, rev, dly, det, atk, rel, x; };

struct Base { float o2, sub, noise, fenv, dec, sus, drive, lfoR, lfoD; int oct; };

inline const Base bases[NumCategories] = {
    /*Bass*/   { 0.50f, 0.80f, 0.00f, 0.25f, 0.35f, 0.70f, 0.25f, 0.5f, 0.00f, -1 },
    /*Pad*/    { 0.80f, 0.00f, 0.02f, 0.20f, 1.50f, 0.80f, 0.05f, 0.3f, 0.15f,  0 },
    /*Lead*/   { 0.60f, 0.10f, 0.00f, 0.30f, 0.40f, 0.70f, 0.15f, 5.5f, 0.05f,  0 },
    /*Synth*/  { 0.70f, 0.20f, 0.00f, 0.40f, 0.50f, 0.50f, 0.20f, 0.8f, 0.20f,  0 },
    /*Keys*/   { 0.50f, 0.00f, 0.01f, 0.50f, 0.90f, 0.20f, 0.05f, 4.0f, 0.03f,  0 },
    /*FX*/     { 0.70f, 0.00f, 0.30f, 0.80f, 2.00f, 0.30f, 0.10f, 0.2f, 0.50f,  0 },
    /*Hit*/    { 0.50f, 0.60f, 0.15f, 0.80f, 0.25f, 0.00f, 0.30f, 1.0f, 0.00f,  0 },
    /*Chord*/  { 0.70f, 0.10f, 0.00f, 0.25f, 1.00f, 0.60f, 0.10f, 0.3f, 0.10f,  0 },
    /*Wobbler*/{ 0.80f, 0.50f, 0.00f, 0.00f, 0.30f, 0.90f, 0.35f, 2.0f, 0.85f, -1 },
};

//                 name                 w1 w2  cutoff  res   rev   dly   det   atk    rel    x
inline const Row bassRows[20] = {
    { "Midnight 808",      0,1,  550,0.10f,0.04f,0.00f, 4,0.002f,0.55f,0.10f },
    { "Velvet Sub",        0,0,  320,0.05f,0.00f,0.00f, 0,0.004f,0.35f,0.00f },
    { "Toronto Slide",     0,2,  800,0.20f,0.05f,0.00f, 6,0.003f,0.50f,0.25f },
    { "Gold Chain Bass",   1,2,  900,0.15f,0.03f,0.00f, 8,0.002f,0.40f,0.35f },
    { "Hotel Room Sub",    0,1,  420,0.10f,0.12f,0.00f, 3,0.006f,0.60f,0.10f },
    { "Rainy Driveway",    1,3,  650,0.25f,0.10f,0.10f, 5,0.003f,0.45f,0.30f },
    { "Low End Theory",    0,2,  700,0.12f,0.04f,0.00f, 7,0.002f,0.38f,0.28f },
    { "Silk Rumble",       0,1,  380,0.18f,0.08f,0.00f, 2,0.005f,0.70f,0.15f },
    { "After Hours Bass",  1,2, 1100,0.22f,0.06f,0.05f, 9,0.002f,0.35f,0.45f },
    { "Slow Burn 808",     0,0,  450,0.08f,0.03f,0.00f, 0,0.001f,0.80f,0.05f },
    { "Smoked Out Bass",   2,2, 1200,0.30f,0.02f,0.00f,12,0.002f,0.30f,0.50f },
    { "Neon Pulse Bass",   1,3,  950,0.28f,0.05f,0.15f, 6,0.003f,0.40f,0.40f },
    { "Cold Shoulder",     0,1,  600,0.14f,0.15f,0.00f, 4,0.004f,0.55f,0.20f },
    { "Basement Tape",     1,2,  500,0.20f,0.10f,0.00f,10,0.003f,0.45f,0.15f },
    { "Lowkey Growl",      2,3, 1000,0.35f,0.03f,0.00f,14,0.002f,0.35f,0.55f },
    { "Moonlit Sub",       0,0,  280,0.05f,0.08f,0.05f, 0,0.008f,0.90f,0.00f },
    { "Trapsoul Thump",    0,1,  700,0.10f,0.02f,0.00f, 3,0.001f,0.28f,0.30f },
    { "Burner Phone 808",  1,0,  520,0.12f,0.04f,0.00f, 5,0.001f,0.65f,0.12f },
    { "Smoke Session",     2,1,  750,0.25f,0.12f,0.10f, 8,0.003f,0.50f,0.30f },
    { "Eclipse Bass",      0,2,  850,0.18f,0.06f,0.00f, 7,0.002f,0.60f,0.40f },
};
inline const Row padRows[20] = {
    { "Night Drive",       2,2, 1800,0.12f,0.45f,0.15f,12,0.90f,1.80f,0.20f },
    { "Velvet Haze",       2,1, 1400,0.10f,0.55f,0.10f,10,1.10f,2.20f,0.15f },
    { "Tower Glow",        2,2, 2600,0.10f,0.50f,0.20f,14,0.70f,2.00f,0.25f },
    { "Lowlight",          1,2, 1100,0.15f,0.40f,0.05f, 9,1.30f,2.50f,0.10f },
    { "Cold Breath",       2,3, 2200,0.18f,0.60f,0.12f,16,1.50f,3.00f,0.30f },
    { "Purple Static",     2,2, 1600,0.20f,0.50f,0.25f,18,0.80f,1.60f,0.35f },
    { "Afterglow Pad",     1,1, 2000,0.08f,0.55f,0.10f, 8,1.00f,2.40f,0.12f },
    { "Midnight Mist",     2,1,  900,0.14f,0.65f,0.15f,11,1.80f,3.20f,0.18f },
    { "Cruising Strings",  2,2, 2400,0.10f,0.35f,0.05f,15,0.50f,1.20f,0.10f },
    { "Hotel Lobby",       1,2, 1500,0.12f,0.45f,0.08f, 7,0.90f,1.80f,0.15f },
    { "Streetlight Wash",  2,2, 3000,0.12f,0.50f,0.20f,13,0.60f,1.80f,0.25f },
    { "Smoked Glass",      2,3, 1000,0.22f,0.55f,0.12f,10,1.20f,2.60f,0.20f },
    { "Warm Static",       1,2, 1300,0.10f,0.40f,0.05f, 6,0.90f,2.00f,0.08f },
    { "Heartbreak Hour",   2,2, 1700,0.16f,0.60f,0.18f,17,1.40f,3.50f,0.30f },
    { "Slow Dance",        1,1, 1500,0.10f,0.45f,0.10f, 9,1.00f,2.20f,0.12f },
    { "Fog Horn Pad",      2,1,  800,0.20f,0.50f,0.05f, 8,1.60f,3.00f,0.10f },
    { "Orbit",             2,2, 3500,0.14f,0.55f,0.25f,20,0.80f,2.40f,0.40f },
    { "Rooftop Air",       1,2, 2800,0.08f,0.60f,0.15f,12,1.10f,2.80f,0.20f },
    { "Ghost Choir",       1,3, 1900,0.18f,0.65f,0.20f,11,1.50f,3.50f,0.28f },
    { "Dusk Blanket",      2,2, 1200,0.12f,0.50f,0.10f, 8,1.20f,2.60f,0.12f },
};
inline const Row leadRows[20] = {
    { "Crying Lead",       2,2, 3500,0.15f,0.30f,0.20f,10,0.010f,0.50f,5.5f },
    { "Silk Whistle",      0,1, 4500,0.08f,0.25f,0.15f, 4,0.020f,0.60f,5.0f },
    { "Neon Cry",          2,3, 3000,0.22f,0.30f,0.25f,12,0.005f,0.40f,6.0f },
    { "Late Night Lead",   3,2, 2800,0.20f,0.25f,0.18f, 8,0.010f,0.45f,5.0f },
    { "Glass Tear",        1,2, 5000,0.10f,0.35f,0.10f, 6,0.010f,0.70f,5.8f },
    { "Falsetto Synth",    0,2, 4000,0.12f,0.30f,0.15f, 5,0.030f,0.80f,5.2f },
    { "Moody Pluck Lead",  2,1, 3200,0.18f,0.20f,0.30f, 7,0.002f,0.35f,0.5f },
    { "Pitched Hook",      1,2, 3800,0.14f,0.35f,0.20f, 9,0.020f,0.55f,6.2f },
    { "Ghost Whistle",     0,0, 5500,0.05f,0.40f,0.25f, 3,0.050f,0.90f,4.8f },
    { "Spotlight",         2,2, 4200,0.20f,0.25f,0.12f,14,0.005f,0.40f,5.5f },
    { "Velvet Saw Lead",   2,1, 2600,0.12f,0.28f,0.18f,11,0.010f,0.50f,5.0f },
    { "Cold Call",         3,3, 3400,0.25f,0.20f,0.20f, 6,0.003f,0.30f,0.5f },
    { "Phantom Hook",      2,3, 3600,0.18f,0.32f,0.22f,10,0.015f,0.60f,5.6f },
    { "Blue Hour Lead",    1,1, 4800,0.10f,0.38f,0.15f, 5,0.040f,0.70f,5.0f },
    { "Mirror Cry",        2,2, 3000,0.30f,0.30f,0.25f,13,0.010f,0.50f,6.5f },
    { "Stray Melody",      0,2, 4000,0.10f,0.25f,0.12f, 4,0.010f,0.45f,5.2f },
    { "Pager Lead",        3,2, 2400,0.28f,0.15f,0.10f, 5,0.002f,0.25f,0.5f },
    { "Whisper Hook",      1,0, 5200,0.06f,0.40f,0.20f, 3,0.030f,0.80f,5.0f },
    { "Late Text Lead",    2,1, 3300,0.16f,0.30f,0.28f, 8,0.008f,0.50f,5.4f },
    { "Heartline",         2,2, 3900,0.20f,0.35f,0.20f,12,0.010f,0.65f,5.8f },
};
inline const Row synthRows[20] = {
    { "Northern Nights",   2,2, 1800,0.25f,0.25f,0.15f,12,0.010f,0.60f,0.40f },
    { "Dark Arp Bed",      3,2, 1500,0.30f,0.15f,0.30f, 8,0.002f,0.35f,0.50f },
    { "Pulse Memory",      3,3, 2200,0.20f,0.20f,0.20f, 6,0.005f,0.40f,0.40f },
    { "Neon Skyline",      2,2, 1400,0.28f,0.30f,0.20f,14,0.050f,0.90f,0.30f },
    { "Nightshift",        2,3, 1900,0.22f,0.20f,0.15f,10,0.010f,0.50f,0.45f },
    { "Chrome Haze",       2,1, 2500,0.15f,0.35f,0.10f, 9,0.020f,0.70f,0.35f },
    { "Static Bounce",     3,2, 1700,0.35f,0.10f,0.25f, 7,0.002f,0.30f,0.60f },
    { "Metro Line",        2,2, 2100,0.20f,0.20f,0.20f,11,0.010f,0.45f,0.40f },
    { "Echo Chamber",      1,2, 2000,0.18f,0.30f,0.35f, 8,0.010f,0.60f,0.30f },
    { "Lowrider",          2,3, 1300,0.30f,0.15f,0.10f,13,0.010f,0.40f,0.50f },
    { "Silhouette",        2,2, 1600,0.22f,0.30f,0.18f, 9,0.030f,0.80f,0.30f },
    { "Red Eye Flight",    2,1, 2800,0.18f,0.28f,0.22f,12,0.040f,1.00f,0.25f },
    { "Voltage",           3,2, 2300,0.32f,0.12f,0.15f, 5,0.002f,0.30f,0.65f },
    { "Dim Lights",        1,2, 1200,0.20f,0.35f,0.12f, 6,0.020f,0.70f,0.30f },
    { "Digital Rain",      3,3, 2600,0.26f,0.20f,0.35f, 4,0.002f,0.35f,0.55f },
    { "Candlelit Saw",     2,2, 1500,0.16f,0.30f,0.10f,10,0.015f,0.60f,0.35f },
    { "Alley Neon",        2,3, 2000,0.28f,0.18f,0.22f,11,0.008f,0.45f,0.50f },
    { "Sub Zero Synth",    2,2, 1100,0.24f,0.25f,0.15f, 7,0.010f,0.55f,0.40f },
    { "Gloss",             1,2, 3000,0.14f,0.30f,0.20f, 8,0.010f,0.50f,0.30f },
    { "Midnight Marquee",  2,2, 2200,0.20f,0.28f,0.25f,13,0.020f,0.75f,0.40f },
};
inline const Row keysRows[20] = {
    { "Rhodes After Dark", 0,1, 2200,0.05f,0.25f,0.10f, 3,0.003f,0.90f,1.20f },
    { "Soft Keys",         0,0, 2800,0.04f,0.20f,0.05f, 2,0.004f,0.80f,1.00f },
    { "Muted Piano",       1,0, 1800,0.06f,0.15f,0.05f, 2,0.002f,0.50f,0.70f },
    { "Velvet EP",         0,1, 2500,0.05f,0.30f,0.12f, 4,0.004f,1.00f,1.40f },
    { "Bedroom Piano",     1,1, 2000,0.05f,0.35f,0.08f, 3,0.002f,0.90f,1.30f },
    { "Glassy Keys",       0,2, 3500,0.08f,0.25f,0.10f, 5,0.002f,0.70f,0.90f },
    { "Wurly Dream",       1,2, 2000,0.10f,0.20f,0.15f, 6,0.003f,0.80f,1.00f },
    { "Lo-Fi Upright",     1,0, 1400,0.05f,0.15f,0.05f, 2,0.002f,0.60f,0.80f },
    { "Music Box Noir",    0,3, 4500,0.06f,0.35f,0.10f, 2,0.001f,1.20f,1.00f },
    { "Dusty Keys",        1,2, 1500,0.08f,0.20f,0.05f, 6,0.003f,0.70f,0.90f },
    { "Candle Keys",       0,0, 2400,0.04f,0.30f,0.10f, 1,0.005f,1.00f,1.50f },
    { "Ballad Grand",      1,0, 3000,0.05f,0.30f,0.08f, 2,0.002f,1.10f,1.60f },
    { "Tape EP",           0,1, 1700,0.07f,0.20f,0.18f, 5,0.004f,0.80f,1.10f },
    { "Silk Bells",        0,1, 5000,0.05f,0.40f,0.12f, 3,0.002f,1.40f,1.20f },
    { "Warm Felt",         1,0, 1200,0.04f,0.15f,0.05f, 1,0.004f,0.60f,0.80f },
    { "Slow Jam EP",       0,1, 2300,0.05f,0.28f,0.12f, 4,0.003f,1.00f,1.40f },
    { "Moonlight Keys",    0,0, 2600,0.05f,0.38f,0.10f, 2,0.003f,1.10f,1.30f },
    { "Cathedral Piano",   1,0, 3200,0.05f,0.45f,0.05f, 2,0.002f,1.30f,1.80f },
    { "Vinyl Keys",        1,2, 1600,0.09f,0.20f,0.10f, 7,0.003f,0.80f,0.90f },
    { "Late Call Keys",    0,1, 2100,0.05f,0.28f,0.15f, 3,0.003f,0.90f,1.20f },
};
inline const Row fxRows[20] = {
    { "Rain On Glass",     2,2, 6000,0.10f,0.50f,0.10f,20,1.50f,2.50f,0.40f },
    { "Tape Rewind",       2,3, 3000,0.30f,0.30f,0.20f,30,0.80f,1.20f,0.20f },
    { "Reverse Rise",      2,2, 4000,0.40f,0.40f,0.15f,15,2.50f,0.50f,0.10f },
    { "Downlift",          2,1, 2500,0.35f,0.45f,0.20f,18,0.02f,2.80f,0.15f },
    { "Vinyl Crackle Bed", 3,2, 7000,0.05f,0.20f,0.05f, 0,0.10f,1.50f,0.60f },
    { "Airport Drift",     2,2, 2000,0.25f,0.60f,0.30f,22,2.00f,3.00f,0.30f },
    { "Night Wind",        2,3, 1800,0.30f,0.55f,0.15f,25,2.50f,3.50f,0.50f },
    { "Slow Motion Sweep", 2,2, 3500,0.45f,0.45f,0.25f,12,3.00f,2.50f,0.20f },
    { "Ghost Whisper",     0,2, 4500,0.20f,0.65f,0.30f,16,1.80f,3.00f,0.45f },
    { "Sub Drop Swell",    0,1,  600,0.15f,0.40f,0.10f, 5,2.00f,1.50f,0.05f },
    { "Thunder Roll",      2,3,  900,0.20f,0.55f,0.10f,28,0.50f,3.80f,0.60f },
    { "City Hum",          2,2,  700,0.15f,0.50f,0.05f,10,1.50f,3.00f,0.35f },
    { "Glitch Flutter",    3,3, 5000,0.40f,0.25f,0.35f,35,0.01f,0.40f,0.25f },
    { "Neon Riser",        2,2, 5500,0.50f,0.35f,0.20f,14,3.50f,0.80f,0.15f },
    { "Heartbeat Pulse",   0,0,  400,0.10f,0.30f,0.10f, 0,0.01f,0.60f,0.05f },
    { "Dream Static",      2,3, 3800,0.22f,0.55f,0.20f,24,1.00f,2.20f,0.55f },
    { "Elevator Fade",     1,2, 1500,0.18f,0.50f,0.15f, 8,1.20f,3.00f,0.10f },
    { "Smoke Trail",       2,1, 2800,0.30f,0.60f,0.25f,17,1.60f,3.20f,0.30f },
    { "Wire Hum",          3,2, 3200,0.35f,0.30f,0.10f,40,0.50f,1.50f,0.20f },
    { "Fade To Black",     2,2, 1200,0.20f,0.65f,0.30f,12,0.05f,4.00f,0.15f },
};
inline const Row hitRows[20] = {
    { "Soft Impact",       0,1, 1800,0.10f,0.20f,0.05f, 3,0.001f,0.50f,0.45f },
    { "Velvet Stab",       2,2, 2200,0.20f,0.15f,0.10f,10,0.001f,0.35f,0.30f },
    { "Dark Stab",         3,2, 1500,0.25f,0.12f,0.08f, 8,0.001f,0.30f,0.25f },
    { "808 Hit",           0,0,  500,0.05f,0.05f,0.00f, 0,0.001f,0.60f,0.70f },
    { "Rim Click",         3,3, 6000,0.20f,0.10f,0.05f, 5,0.001f,0.12f,0.06f },
    { "Clap Hit",          2,3, 4500,0.15f,0.35f,0.15f,25,0.001f,0.35f,0.18f },
    { "Thunder Hit",       2,3, 1200,0.20f,0.40f,0.10f,18,0.001f,0.70f,0.60f },
    { "Pluck Hit",         2,1, 3000,0.25f,0.15f,0.15f, 6,0.001f,0.25f,0.20f },
    { "Snap",              3,2, 5500,0.30f,0.10f,0.05f, 4,0.001f,0.15f,0.10f },
    { "Sub Boom",          0,0,  350,0.05f,0.30f,0.00f, 0,0.001f,0.90f,0.90f },
    { "Vinyl Stab",        1,2, 2000,0.15f,0.25f,0.10f, 9,0.001f,0.40f,0.35f },
    { "Glass Hit",         0,1, 5000,0.10f,0.40f,0.15f, 4,0.001f,0.80f,0.50f },
    { "Bell Hit",          0,3, 4000,0.10f,0.35f,0.10f, 2,0.001f,0.90f,0.70f },
    { "Dirty Hat",         3,3, 8000,0.30f,0.05f,0.00f,40,0.001f,0.10f,0.05f },
    { "Tom Drop",          0,1,  900,0.10f,0.20f,0.05f, 3,0.001f,0.45f,0.35f },
    { "Impact Hit",        2,2, 1600,0.20f,0.45f,0.15f,16,0.001f,0.90f,0.60f },
    { "Low Stab",          2,2,  900,0.25f,0.20f,0.05f, 7,0.001f,0.35f,0.30f },
    { "Whip Crack",        3,2, 7000,0.35f,0.15f,0.10f,15,0.001f,0.20f,0.12f },
    { "Soft Thud",         0,1,  700,0.10f,0.20f,0.05f, 2,0.001f,0.30f,0.22f },
    { "Ghost Hit",         0,1, 2500,0.10f,0.60f,0.30f, 5,0.001f,1.20f,0.50f },
};
inline const Row chordRows[20] = {
    { "Min7 Haze",         2,1, 2000,0.12f,0.35f,0.15f, 8,0.020f,0.90f,1 },
    { "Min9 Late Night",   1,2, 1800,0.10f,0.40f,0.12f, 6,0.030f,1.00f,2 },
    { "Maj7 Sunrise",      2,2, 2400,0.10f,0.35f,0.10f, 9,0.020f,0.90f,3 },
    { "Add9 Heartbreak",   1,1, 2200,0.12f,0.40f,0.15f, 7,0.020f,1.10f,4 },
    { "7sus Drift",        2,3, 1900,0.14f,0.35f,0.15f,10,0.040f,1.00f,5 },
    { "Velvet Min7",       0,1, 2600,0.06f,0.30f,0.10f, 4,0.005f,0.90f,1 },
    { "EP Min9",           0,1, 2300,0.05f,0.30f,0.12f, 4,0.004f,1.10f,2 },
    { "Dark Chord Bed",    2,2, 1300,0.15f,0.50f,0.10f,12,0.500f,1.80f,1 },
    { "Smoke Maj7",        1,2, 2000,0.10f,0.45f,0.15f, 8,0.040f,1.20f,3 },
    { "Pluck Add9",        2,1, 3000,0.20f,0.25f,0.25f, 6,0.002f,0.50f,4 },
    { "Gospel Min9",       1,0, 2800,0.08f,0.35f,0.10f, 5,0.010f,1.00f,2 },
    { "Stab Min7",         3,2, 2400,0.25f,0.15f,0.10f, 7,0.002f,0.40f,1 },
    { "Wide Maj7",         2,2, 3200,0.10f,0.50f,0.20f,15,0.050f,1.30f,3 },
    { "Bedroom Sus",       1,2, 1900,0.10f,0.40f,0.12f, 6,0.030f,1.00f,5 },
    { "Chrome Min9",       2,3, 2600,0.18f,0.35f,0.20f,10,0.010f,0.80f,2 },
    { "Low Min7",          2,1, 1100,0.15f,0.30f,0.05f, 6,0.020f,0.80f,1 },
    { "Silk Add9",         0,2, 3000,0.08f,0.40f,0.15f, 4,0.010f,1.20f,4 },
    { "Cold Maj7",         2,2, 1700,0.15f,0.45f,0.10f,11,0.060f,1.50f,3 },
    { "Midnight 7sus",     1,2, 2200,0.12f,0.40f,0.20f, 8,0.030f,1.10f,5 },
    { "Final Chord",       2,1, 2000,0.12f,0.55f,0.25f, 9,0.050f,2.00f,2 },
};
inline const Row wobbleRows[20] = {
    { "Slow Wobble",       2,2,  900,0.35f,0.10f,0.05f, 8,0.003f,0.40f,2.0f },
    { "Half-Time Wob",     2,3,  800,0.40f,0.08f,0.00f,10,0.003f,0.40f,3.0f },
    { "Tremor Bass",       3,2,  700,0.45f,0.05f,0.00f,12,0.002f,0.30f,5.0f },
    { "Liquid Wob",        1,2, 1200,0.30f,0.12f,0.05f, 7,0.004f,0.45f,1.5f },
    { "Dark Pulse",        2,2,  600,0.50f,0.05f,0.00f, 9,0.002f,0.35f,4.0f },
    { "Triplet Wobble",    2,1, 1000,0.38f,0.08f,0.10f, 6,0.003f,0.40f,6.0f },
    { "Ripple",            1,1, 1500,0.25f,0.20f,0.10f, 5,0.010f,0.60f,2.5f },
    { "Deep Sway",         2,2,  500,0.35f,0.15f,0.00f, 8,0.010f,0.70f,1.0f },
    { "Night Shaker",      3,2,  900,0.42f,0.08f,0.08f,14,0.003f,0.40f,7.0f },
    { "Gator Wob",         2,3,  750,0.55f,0.05f,0.00f,16,0.002f,0.30f,8.0f },
    { "Smooth Throb",      0,2, 1100,0.20f,0.15f,0.05f, 3,0.010f,0.60f,2.0f },
    { "Syrup Wobble",      1,2, 1000,0.30f,0.25f,0.10f, 6,0.010f,0.80f,1.2f },
    { "Heartbeat Wob",     0,1,  800,0.30f,0.10f,0.05f, 2,0.005f,0.50f,1.8f },
    { "Warp Drive",        2,2, 1400,0.45f,0.10f,0.20f,18,0.004f,0.50f,5.5f },
    { "Wide Wobble",       2,2, 1300,0.35f,0.30f,0.15f,20,0.010f,0.70f,3.5f },
    { "Velvet Wobble",     1,2, 1000,0.22f,0.22f,0.05f, 5,0.010f,0.70f,2.2f },
    { "Choke",             3,3,  700,0.60f,0.05f,0.00f, 6,0.002f,0.25f,9.0f },
    { "Foggy Wob",         2,1,  600,0.30f,0.30f,0.10f, 8,0.010f,0.80f,0.8f },
    { "Pitch-Black Wob",   2,2,  450,0.50f,0.10f,0.00f,12,0.003f,0.40f,2.8f },
    { "Moonwalk Wob",      2,3,  950,0.38f,0.12f,0.10f, 9,0.004f,0.50f,4.5f },
};

inline std::vector<Preset> buildPresets()
{
    const Row* tables[NumCategories] = { bassRows, padRows, leadRows, synthRows, keysRows,
                                         fxRows, hitRows, chordRows, wobbleRows };
    std::vector<Preset> out;
    for (int c = 0; c < NumCategories; ++c)
        for (int i = 0; i < perCategory; ++i)
        {
            const Row& r = tables[c][i];
            const Base& b = bases[c];
            float fenv = b.fenv, dec = b.dec, noise = b.noise, lfoR = b.lfoR, lfoD = b.lfoD;
            int chord = 0, oct = b.oct;
            switch (c)
            {
                case Bass: case Synth: fenv = r.x; break;
                case Pad:   lfoD = r.x; break;
                case Lead:  lfoR = r.x; lfoD = (r.x > 1.0f ? 0.05f : 0.0f); break;
                case Keys:  dec = r.x; break;
                case FX:    noise = r.x; break;
                case Hit:   dec = r.x; if (r.cut < 1000.0f) oct = -1; break;
                case Chord: chord = (int) r.x; break;
                case Wobbler: lfoR = r.x; break;
            }
            Preset p;
            p.name = r.name;
            p.category = c;
            p.values = {
                { "w1", (float) r.w1 }, { "w2", (float) r.w2 }, { "det", r.det }, { "o2lvl", b.o2 },
                { "sub", b.sub }, { "noise", noise }, { "cut", r.cut }, { "res", r.res },
                { "fenv", fenv }, { "atk", r.atk }, { "dec", dec }, { "sus", b.sus }, { "rel", r.rel },
                { "lfoR", lfoR }, { "lfoD", lfoD }, { "drive", b.drive }, { "rev", r.rev },
                { "dly", r.dly }, { "oct", (float) oct }, { "chord", (float) chord }
            };
            out.push_back(std::move(p));
        }
    return out;
}
} // namespace Essence
