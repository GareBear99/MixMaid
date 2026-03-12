# MixMaid AAAAAA v8 Release Notes

This pass focuses on audit-fix hardening rather than adding cosmetic features.

## Added / improved
- Removed SmartBands detector misuse of live filter state.
- Added detector-only envelope tracking independent of program EQ filters.
- Removed audio-thread dry-buffer resize path.
- Added fail-closed oversized-block handling for realtime safety.
- Aligned CMake project/package metadata to v8.
- Refreshed audit and implementation docs.

## Remaining truth-gates
- Compile against the exact JUCE version in the target environment.
- DAW validation in AU/VST3 hosts.
- Automation stress test on fast parameter changes.
- Null/delta audition listening pass and preset QA.
