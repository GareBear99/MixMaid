# MixMaid

**MixMaid** is an intelligent real-time mix correction plugin designed to improve tonal balance, headroom, and spectral distribution while preserving the musical character of the source.

Instead of aggressive “AI mastering,” MixMaid uses bounded analysis and dynamic correction to gently guide mixes toward a balanced spectrum and stable dynamics.

The goal is simple: fix common mix problems without destroying the mix.

---

## Features

### Intelligent Spectral Balance
MixMaid analyzes tonal distribution across the full frequency spectrum and applies controlled dynamic correction to maintain balance.

Reduces:

- muddy low mids
- harsh presence spikes
- weak low end
- missing air and clarity

---

### True FFT Analyzer
MixMaid uses FFT-bin energy analysis to evaluate tonal balance across key frequency regions.

Frequency bands monitored:


Sub 20–60 Hz
Bass 60–150 Hz
Low Mid 150–500 Hz
Mid 500–2k Hz
Presence 2k–6k Hz
Air 6k–16k Hz


---

### Dynamic Correction Engine
Instead of static EQ boosts or cuts, MixMaid dynamically adjusts tonal balance in response to the incoming signal.

This keeps the mix musical while still correcting spectral issues.

---

### Stereo Balance Assist
MixMaid monitors mid/side energy and can apply subtle stereo correction to maintain stable imaging and mono compatibility.

---

### Headroom Management
MixMaid tracks RMS and peak levels and can gently apply trim correction to maintain safe headroom.

Typical targets:


RMS ≈ -18 dB
Peak ≈ -6 dB


---

### Stereo Safety Limiter
A transparent stereo-linked limiter at the output stage prevents overshoots caused by correction stages.

---

### Wet / Dry Control
Parallel processing allows blending MixMaid’s correction with the original signal.

---

### Delta Audition
Delta mode allows engineers to hear exactly what MixMaid is changing.

---

### Reference Learning
MixMaid can learn the spectral distribution of a reference track and apply tonal correction toward that profile.

---

### Preset System
Includes built-in genre presets and user preset save/load support.

Example presets:

- Rock Balance
- EDM Balance
- Hip Hop Balance
- Cinematic Balance
- Podcast Clarity

---

## Unlock Model

MixMaid unlocks free if the full Pro plugin suite is owned:


FreeEQ8 Pro
BassMaid Pro
GlueMaid Pro
SpaceMaid Pro
WaveForm Pro
RiftSynth Pro


and **Aether is installed**.

Otherwise MixMaid is available as a standalone **$50 plugin**.

---

## Plugin Formats

Supported formats:


VST3
AU
Standalone


AAX may be added in the future.

---

## Performance

Designed for low CPU usage.

Typical performance:


CPU usage: <3%
Memory: <20MB


Suitable for use on:

- mix bus
- subgroups
- individual tracks

---

## Built With

MixMaid is built using:

- JUCE framework
- modern C++ DSP architecture
- realtime-safe processing principles

---

## Ecosystem

MixMaid integrates with the rest of the plugin lineup:


FreeEQ8 – parametric EQ
BassMaid – bass enhancement
GlueMaid – bus compression
SpaceMaid – spatial enhancement
WaveForm – waveform modulation tools
RiftSynth – generative synth
Aether – atmospheric texture generator


---

## Philosophy

MixMaid is built around a simple principle:

> intelligent correction should guide a mix, not replace the engineer.

All correction stages are intentionally bounded to maintain musical transparency.

---

## License

MIT License

See the LICENSE file for details.

---

## Development Status

MixMaid is under active development and will continue to evolve with:

- improved analysis models
- expanded reference learning
- additional presets
- ecosystem integration
