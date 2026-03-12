#include "PresetBank.h"

namespace mixmaid {
std::array<PresetDefinition, 5> PresetBank::builtins()
{
    return {{
        {"Universal", 55.0f, 45.0f, 50.0f, 35.0f, 45.0f, 0.0f, 0},
        {"EDM Punch", 68.0f, 65.0f, 70.0f, 38.0f, 52.0f, 0.0f, 1},
        {"Hip-Hop Weight", 62.0f, 42.0f, 76.0f, 26.0f, 48.0f, 0.0f, 2},
        {"Rock Clarity", 58.0f, 54.0f, 38.0f, 24.0f, 51.0f, 0.0f, 3},
        {"Podcast Focus", 47.0f, 35.0f, 18.0f, 8.0f, 30.0f, 0.0f, 4}
    }};
}

juce::var PresetBank::toVar(const PresetDefinition& p)
{
    auto* obj = new juce::DynamicObject();
    obj->setProperty("name", p.name);
    obj->setProperty("strength", p.strength);
    obj->setProperty("speed", p.speed);
    obj->setProperty("bassFocus", p.bassFocus);
    obj->setProperty("stereoAssist", p.stereoAssist);
    obj->setProperty("dynamicAmount", p.dynamicAmount);
    obj->setProperty("referenceBlend", p.referenceBlend);
    obj->setProperty("target", p.target);
    return obj;
}

PresetDefinition PresetBank::fromVar(const juce::var& v)
{
    PresetDefinition p;
    if (auto* obj = v.getDynamicObject())
    {
        p.name = obj->getProperty("name").toString();
        p.strength = static_cast<float>(obj->getProperty("strength"));
        p.speed = static_cast<float>(obj->getProperty("speed"));
        p.bassFocus = static_cast<float>(obj->getProperty("bassFocus"));
        p.stereoAssist = static_cast<float>(obj->getProperty("stereoAssist"));
        p.dynamicAmount = static_cast<float>(obj->getProperty("dynamicAmount"));
        p.referenceBlend = static_cast<float>(obj->getProperty("referenceBlend"));
        p.target = static_cast<int>(obj->getProperty("target"));
    }
    return p;
}

bool PresetBank::savePresetToFile(const PresetDefinition& preset, const juce::File& file)
{
    return file.replaceWithText(juce::JSON::toString(toVar(preset), true));
}

std::optional<PresetDefinition> PresetBank::loadPresetFromFile(const juce::File& file)
{
    if (! file.existsAsFile())
        return std::nullopt;

    const auto parsed = juce::JSON::parse(file);
    if (parsed.isVoid())
        return std::nullopt;

    return fromVar(parsed);
}

}
