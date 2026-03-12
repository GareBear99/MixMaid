#pragma once
#include <JuceHeader.h>
#include <array>

namespace mixmaid {

struct PresetDefinition
{
    juce::String name;
    float strength = 55.0f;
    float speed = 45.0f;
    float bassFocus = 50.0f;
    float stereoAssist = 35.0f;
    float dynamicAmount = 45.0f;
    float referenceBlend = 0.0f;
    int target = 0;
};

class PresetBank
{
public:
    static std::array<PresetDefinition, 5> builtins();
    static juce::var toVar(const PresetDefinition& preset);
    static PresetDefinition fromVar(const juce::var& v);
    static bool savePresetToFile(const PresetDefinition& preset, const juce::File& file);
    static std::optional<PresetDefinition> loadPresetFromFile(const juce::File& file);
};

}
