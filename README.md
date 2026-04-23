> 🎛️ Part of the [TizWildin Plugin Ecosystem](https://garebear99.github.io/TizWildinEntertainmentHUB/) — 19 free audio plugins with a live update dashboard.
>
> [FreeEQ8](https://github.com/GareBear99/FreeEQ8) · [XyloCore](https://github.com/GareBear99/XyloCore) · [Instrudio](https://github.com/GareBear99/Instrudio) · [Therum](https://github.com/GareBear99/Therum_JUCE-Plugin) · [BassMaid](https://github.com/GareBear99/BassMaid) · [SpaceMaid](https://github.com/GareBear99/SpaceMaid) · [GlueMaid](https://github.com/GareBear99/GlueMaid) · [MixMaid](https://github.com/GareBear99/MixMaid) · [MultiMaid](https://github.com/GareBear99/MultiMaid) · [MeterMaid](https://github.com/GareBear99/MeterMaid) · [ChainMaid](https://github.com/GareBear99/ChainMaid) · [PaintMask](https://github.com/GareBear99/PaintMask_Free-JUCE-Plugin) · [WURP](https://github.com/GareBear99/WURP_Toxic-Motion-Engine_JUCE) · [AETHER](https://github.com/GareBear99/AETHER_Choir-Atmosphere-Designer) · [WhisperGate](https://github.com/GareBear99/WhisperGate_Free-JUCE-Plugin) · [RiftWave](https://github.com/GareBear99/RiftWaveSuite_RiftSynth_WaveForm_Lite) · [FreeSampler](https://github.com/GareBear99/FreeSampler_v0.3) · [VF-PlexLab](https://github.com/GareBear99/VF-PlexLab) · [PAP-Forge-Audio](https://github.com/GareBear99/PAP-Forge-Audio)
>
> 🎁 [Free Packs & Samples](#tizwildin-free-sample-packs) — jump to free packs & samples
>
> 🎵 [Awesome Audio](https://github.com/GareBear99/awesome-audio-plugins-dev) — (FREE) Awesome Audio Dev List

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

## TizWildin FREE sample packs

| Pack | Description |
|------|-------------|
| [**TizWildin-Aurora**](https://github.com/GareBear99/TizWildin-Aurora) | 3-segment original synth melody pack with loops, stems, demo renders, and neon/cinematic phrasing |
| [**TizWildin-Obsidian**](https://github.com/GareBear99/TizWildin-Obsidian) | Dark cinematic sample pack with choir textures, menu loops, transitions, bass, atmosphere, drums, and electric-banjo extensions |
| [**TizWildin-Skyline**](https://github.com/GareBear99/TizWildin-Skyline) | 30 BPM-tagged synthwave and darkwave loops with generator snapshot and dark neon additions |
| [**TizWildin-Chroma**](https://github.com/GareBear99/TizWildin-Chroma) | Multi-segment game synthwave loop sample pack from TizWildin Entertainment |
| [**TizWildin-Chime**](https://github.com/GareBear99/TizWildin-Chime) | Multi-part 88 BPM chime collection spanning glass, void, halo, reed, and neon synthwave lanes |
| [**Free Violin Synth Sample Kit**](https://github.com/GareBear99/Free-Violin-Synth-Sample-Kit) | Physical-model violin sample kit rendered from the Instrudio violin instrument |
| [**Free Dark Piano Sound Kit**](https://github.com/GareBear99/Free-Dark-Piano-Sound-Kit) | 88 piano notes + dark/cinematic loops and MIDI |
| [**Free 808 Producer Kit**](https://github.com/GareBear99/Free-808-Producer-Kit) | 94 hand-crafted 808 bass samples tuned to every chromatic key |
| [**Free Riser Producer Kit**](https://github.com/GareBear99/Free-Riser-Producer-Kit) | 115+ risers and 63 downlifters - noise, synth, drum, FX, cinematic |
| [**Phonk Producer Toolkit**](https://github.com/GareBear99/Phonk_Producer_Toolkit) | Drift phonk starter kit - 808s, cowbells, drums, MIDI, templates |
| [**Free Future Bass Producer Kit**](https://github.com/GareBear99/Free-Future-Bass-Producer-Kit) | Loops, fills, drums, bass, synths, pads, and FX |

### Related audio projects
- [**VF-PlexLab**](https://github.com/GareBear99/VF-PlexLab) - VocalForge PersonaPlex Lab starter repo for a JUCE plugin + local backend + HTML tester around NVIDIA PersonaPlex.
- [**PAP-Forge-Audio**](https://github.com/GareBear99/PAP-Forge-Audio) - Procedural Autonomous Plugins runtime for generating, branching, validating, and restoring plugin projects from natural-language sound intent.
