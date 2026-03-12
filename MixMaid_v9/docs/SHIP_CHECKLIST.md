# MixMaid v9 Ship Checklist

## Build truth gate
- [ ] JUCE folder added and project config succeeds
- [ ] AU builds on macOS
- [ ] VST3 builds on macOS + Windows
- [ ] Standalone launches

## Functional gate
- [ ] parameter automation works for all exposed controls
- [ ] A/B snapshot store/recall survives host state save/load
- [ ] reference learn starts clean on every new Learn pass
- [ ] delta path nulls correctly against dry/wet expectations
- [ ] safe mode never exceeds bounded correction rules
- [ ] oversized host blocks fail closed without allocation or corruption

## Product gate
- [ ] preset browser finished
- [ ] proper branding assets added
- [ ] trial/licensing rules integrated if required
- [ ] installer/notarization pipeline completed
