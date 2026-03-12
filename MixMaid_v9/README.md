# MixMaid AAAAAA v9

MixMaid v9 is the DARPA-focused continuation of the earlier packages. This pass is centered on correcting the last major source-level mismatches called out in the audit.

## What changed in v9
- upgraded analyzer tonal-band extraction to use true FFT-bin energy
- replaced the old saturator-style safety stage with a real stereo-linked limiter
- added preset file loading helper alongside preset saving
- aligned CMake/package identity to v9 (`0.9.0`, `MmA9`)
- refreshed docs to match the actual package

## Product intent
MixMaid is a bounded smart mix correction engine aimed at fast tonal cleanup, stereo containment, and safer headroom shaping without pretending to be a fully autonomous mix engineer.

## Honest status
This is the strongest source package in this conversation so far. The remaining truth-gate is no longer product direction drift; it is real JUCE compile proof and host validation.

## Remaining hard gates
1. compile against your exact JUCE tree
2. validate DAW load/save state, automation, and preset restore
3. run CPU and null/delta listening passes in hosts
4. add signing/notarization/release pipeline
