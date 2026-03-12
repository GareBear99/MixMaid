# MixMaid AAAAAA v6 Release Notes

This pass focuses on host-facing hardening rather than cosmetic drift.

## Added / improved
- Removed `makeCopyOf` from the steady-state processing path; dry path now uses pre-sized buffers and `copyFrom`.
- Added smoothed output gain stage.
- Added smoothed wet/dry blending with ramps to reduce zippering on automation.
- Added input trim ramping instead of single-sample jumps.
- Kept limiter/safety stage after smart correction.
- Preset application path kept explicit for target choice restoration.

## Remaining truth-gates
- Compile against the exact JUCE version in the target environment.
- DAW validation in AU/VST3 hosts.
- Automation stress test on fast parameter changes.
- Null/delta audition listening pass and preset QA.
