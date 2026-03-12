#pragma once
#include <JuceHeader.h>

namespace mixmaid {

class SafetyLimiter
{
public:
    void prepare(double sampleRate, int maxBlockSize, int numChannels);
    void reset();
    void setDrive(float db) noexcept { driveDb = juce::jlimit(0.0f, 12.0f, db); }
    void setCeiling(float db) noexcept { ceilingDb = juce::jlimit(-6.0f, 0.0f, db); }
    void process(juce::AudioBuffer<float>& buffer);
    float getReductionDb() const noexcept { return reductionDb.load(); }

private:
    static float dbToLin(float db) noexcept { return juce::Decibels::decibelsToGain(db); }

    double sr = 44100.0;
    int channels = 2;
    float driveDb = 0.0f;
    float ceilingDb = -0.8f;
    float releaseCoeff = 0.995f;
    float currentGain = 1.0f;
    std::atomic<float> reductionDb { 0.0f };
};

}
