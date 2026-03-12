#pragma once
#include <JuceHeader.h>

namespace mixmaid {

class ABState
{
public:
    void storeA(const juce::AudioProcessorValueTreeState& apvts);
    void storeB(const juce::AudioProcessorValueTreeState& apvts);
    bool restoreA(juce::AudioProcessorValueTreeState& apvts) const;
    bool restoreB(juce::AudioProcessorValueTreeState& apvts) const;
    bool hasA() const noexcept { return stateA.isNotEmpty(); }
    bool hasB() const noexcept { return stateB.isNotEmpty(); }

private:
    static juce::String serialize(const juce::AudioProcessorValueTreeState& apvts);
    static bool restore(juce::AudioProcessorValueTreeState& apvts, const juce::String& xmlText);

    juce::String stateA, stateB;
};

}
