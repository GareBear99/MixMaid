#include "ABState.h"

namespace mixmaid {

juce::String ABState::serialize(const juce::AudioProcessorValueTreeState& apvts)
{
    if (auto xml = apvts.copyState().createXml())
        return xml->toString();
    return {};
}

bool ABState::restore(juce::AudioProcessorValueTreeState& apvts, const juce::String& xmlText)
{
    if (xmlText.isEmpty())
        return false;
    std::unique_ptr<juce::XmlElement> xml(juce::parseXML(xmlText));
    if (xml == nullptr)
        return false;
    if (! xml->hasTagName(apvts.state.getType()))
        return false;
    apvts.replaceState(juce::ValueTree::fromXml(*xml));
    return true;
}

void ABState::storeA(const juce::AudioProcessorValueTreeState& apvts) { stateA = serialize(apvts); }
void ABState::storeB(const juce::AudioProcessorValueTreeState& apvts) { stateB = serialize(apvts); }
bool ABState::restoreA(juce::AudioProcessorValueTreeState& apvts) const { return restore(apvts, stateA); }
bool ABState::restoreB(juce::AudioProcessorValueTreeState& apvts) const { return restore(apvts, stateB); }

}
