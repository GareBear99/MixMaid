#include "ReferenceMatcher.h"
#include <numeric>

namespace mixmaid {

void ReferenceMatcher::reset() noexcept
{
    learning = false;
    frames = 0;
    accum = { 0, 0, 0, 0, 0 };
}

void ReferenceMatcher::ingest(const AnalyzerSnapshot& snapshot) noexcept
{
    if (! learning)
        return;

    const float total = juce::jmax(0.0001f,
                                   snapshot.lowEnergy + snapshot.lowMidEnergy + snapshot.midEnergy
                                 + snapshot.presenceEnergy + snapshot.airEnergy);
    const std::array<float, 5> dist {
        snapshot.lowEnergy / total,
        snapshot.lowMidEnergy / total,
        snapshot.midEnergy / total,
        snapshot.presenceEnergy / total,
        snapshot.airEnergy / total
    };

    for (size_t i = 0; i < dist.size(); ++i)
        accum[i] += dist[i];

    ++frames;
}

ReferenceTarget ReferenceMatcher::getTargetBlend(const std::array<float, 5>& fallback, float blend) const noexcept
{
    ReferenceTarget t;
    t.distribution = fallback;
    t.valid = frames > 8;
    if (! t.valid)
        return t;

    const float b = juce::jlimit(0.0f, 1.0f, blend);
    for (size_t i = 0; i < t.distribution.size(); ++i)
    {
        const float learned = static_cast<float>(accum[i] / (double) frames);
        t.distribution[i] = juce::jlimit(0.02f, 0.70f, fallback[i] * (1.0f - b) + learned * b);
    }

    const float sum = std::accumulate(t.distribution.begin(), t.distribution.end(), 0.0f);
    if (sum > 0.0001f)
        for (auto& v : t.distribution)
            v /= sum;

    return t;
}

float ReferenceMatcher::getConfidence() const noexcept
{
    return juce::jlimit(0.0f, 1.0f, frames / 240.0f);
}

}
