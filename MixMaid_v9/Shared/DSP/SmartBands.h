#pragma once
#include <JuceHeader.h>
#include <array>

namespace mixmaid {

struct BandCommand
{
    float lowDb = 0.0f;
    float lowMidDb = 0.0f;
    float presenceDb = 0.0f;
    float airDb = 0.0f;
    float dynamicAmount = 0.0f;
    float width = 0.0f;
    float trimDb = 0.0f;
};

class SmartBands
{
public:
    void prepare(double sampleRate, int maxBlockSize, int numChannels);
    void reset();
    void setCommand(const BandCommand& c, bool safeMode);
    void process(juce::AudioBuffer<float>& buffer);

    std::array<float, 4> getEnvelopeState() const noexcept { return envelopes; }

private:
    static float dbToLin(float db) noexcept { return juce::Decibels::decibelsToGain(db); }
    void refreshCoefficients();
    void updateDetectorEnvelopes(float mono) noexcept;

    double sr = 44100.0;
    int channels = 2;
    bool safe = true;
    BandCommand command;
    int coeffCounter = 0;

    std::array<juce::dsp::IIR::Filter<float>, 2> lowShelf, mudBell, presenceBell, airShelf;
    std::array<float, 4> envelopes { 0,0,0,0 };
    std::array<juce::SmoothedValue<float>, 6> smooth;

    float detectorPrev = 0.0f;
    std::array<float, 4> detectorState { 0,0,0,0 };
};

}
