#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SpectrumPanel final : public juce::Component,
                            private juce::Timer
{
public:
    explicit SpectrumPanel(MixMaidAudioProcessor& p) : processor(p) { startTimerHz(24); }
    void paint(juce::Graphics&) override;
private:
    void timerCallback() override { repaint(); }
    MixMaidAudioProcessor& processor;
};

class MixMaidAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                         private juce::Timer,
                                         private juce::Button::Listener
{
public:
    explicit MixMaidAudioProcessorEditor (MixMaidAudioProcessor&);
    ~MixMaidAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void buttonClicked(juce::Button* button) override;
    void setupKnob(juce::Slider& slider, const juce::String& name);

    MixMaidAudioProcessor& audioProcessor;
    SpectrumPanel spectrum;

    juce::ToggleButton optimize, safeMode, lowProtect, learn, delta;
    juce::TextButton snapAStore, snapARecall, snapBStore, snapBRecall, loadPreset, savePreset;
    juce::Slider strength, speed, bassFocus, stereoAssist, dynamicAmount, inputTrim, outGain, mix, referenceBlend;
    juce::ComboBox target, builtInPresets;
    juce::Label title, subtitle, inLbl, outLbl, correctionLbl, widthLbl, headroomLbl, dynamicLbl, lowMidLbl, refLbl, limitLbl;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<BA> aOptimize, aSafeMode, aLowProtect, aLearn, aDelta;
    std::unique_ptr<SA> aStrength, aSpeed, aBassFocus, aStereoAssist, aDynamicAmount, aInputTrim, aOutGain, aMix, aReferenceBlend;
    std::unique_ptr<CA> aTarget;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixMaidAudioProcessorEditor)
};
