#include "SafetyLimiter.h"
#include <cmath>

namespace mixmaid {

void SafetyLimiter::prepare(double sampleRate, int, int numChannels)
{
    sr = sampleRate;
    channels = juce::jmax(1, numChannels);
    reset();
}

void SafetyLimiter::reset()
{
    currentGain = 1.0f;
    releaseCoeff = std::exp(-1.0f / (float) juce::jmax(1.0, sr * 0.050));
    reductionDb.store(0.0f);
}

void SafetyLimiter::process(juce::AudioBuffer<float>& buffer)
{
    const int activeChannels = juce::jmin(channels, buffer.getNumChannels());
    if (activeChannels <= 0)
        return;

    const float preGain = dbToLin(driveDb);
    const float ceiling = dbToLin(ceilingDb);
    float worstReductionDb = 0.0f;

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float linkedPeak = 0.0f;
        for (int ch = 0; ch < activeChannels; ++ch)
            linkedPeak = juce::jmax(linkedPeak, std::abs(buffer.getSample(ch, i) * preGain));

        float targetGain = 1.0f;
        if (linkedPeak > ceiling)
            targetGain = ceiling / juce::jmax(1.0e-6f, linkedPeak);

        if (targetGain < currentGain)
            currentGain = targetGain; // instant attack for safety
        else
            currentGain = currentGain * releaseCoeff + targetGain * (1.0f - releaseCoeff);

        const float reduction = juce::jmax(1.0e-6f, currentGain);
        worstReductionDb = juce::jmin(worstReductionDb, juce::Decibels::gainToDecibels(reduction, -48.0f));

        for (int ch = 0; ch < activeChannels; ++ch)
            buffer.setSample(ch, i, buffer.getSample(ch, i) * preGain * currentGain);
    }

    reductionDb.store(std::abs(worstReductionDb));
}

}
