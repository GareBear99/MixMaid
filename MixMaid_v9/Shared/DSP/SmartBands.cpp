#include "SmartBands.h"
#include <cmath>

namespace mixmaid {
namespace {
using Coeff = juce::dsp::IIR::Coefficients<float>;

template <typename Arr, typename Factory>
void assign(Arr& a, Factory&& f)
{
    for (auto& item : a)
        item.coefficients = f();
}
}

void SmartBands::prepare(double sampleRate, int, int numChannels)
{
    sr = sampleRate;
    channels = juce::jlimit(1, 2, numChannels);
    for (auto& s : smooth)
        s.reset(sr, 0.08);
    reset();
    refreshCoefficients();
}

void SmartBands::reset()
{
    for (auto& f : lowShelf) f.reset();
    for (auto& f : mudBell) f.reset();
    for (auto& f : presenceBell) f.reset();
    for (auto& f : airShelf) f.reset();
    envelopes = {0,0,0,0};
    detectorState = {0,0,0,0};
    detectorPrev = 0.0f;
    coeffCounter = 0;
}

void SmartBands::setCommand(const BandCommand& c, bool safeMode)
{
    command = c;
    safe = safeMode;

    const float eqLimit = safe ? 3.0f : 6.0f;
    const float widthLimit = safe ? 0.15f : 0.35f;
    const float trimLimit = safe ? 2.5f : 6.0f;

    smooth[0].setTargetValue(juce::jlimit(-eqLimit, eqLimit, c.lowDb));
    smooth[1].setTargetValue(juce::jlimit(-eqLimit, eqLimit, c.lowMidDb));
    smooth[2].setTargetValue(juce::jlimit(-eqLimit, eqLimit, c.presenceDb));
    smooth[3].setTargetValue(juce::jlimit(-eqLimit, eqLimit, c.airDb));
    smooth[4].setTargetValue(juce::jlimit(-widthLimit, widthLimit, c.width));
    smooth[5].setTargetValue(juce::jlimit(-trimLimit, trimLimit, c.trimDb));
}

void SmartBands::refreshCoefficients()
{
    assign(lowShelf, [this]{ return Coeff::makeLowShelf(sr, 95.0f, 0.707f, dbToLin(smooth[0].getCurrentValue())); });
    assign(mudBell, [this]{ return Coeff::makePeakFilter(sr, 320.0f, 0.9f, dbToLin(smooth[1].getCurrentValue())); });
    assign(presenceBell, [this]{ return Coeff::makePeakFilter(sr, 3200.0f, 0.85f, dbToLin(smooth[2].getCurrentValue())); });
    assign(airShelf, [this]{ return Coeff::makeHighShelf(sr, 9600.0f, 0.707f, dbToLin(smooth[3].getCurrentValue())); });
}

void SmartBands::updateDetectorEnvelopes(float mono) noexcept
{
    const float absMono = std::abs(mono);
    const float hp = mono - detectorPrev;
    detectorPrev = mono;

    const float low = absMono;
    const float lowMid = std::abs(0.72f * mono - 0.28f * hp);
    const float presence = std::abs(hp);
    const float air = std::abs(hp - detectorState[3] * 0.5f);

    detectorState[0] = 0.985f * detectorState[0] + 0.015f * low;
    detectorState[1] = 0.975f * detectorState[1] + 0.025f * lowMid;
    detectorState[2] = 0.955f * detectorState[2] + 0.045f * presence;
    detectorState[3] = 0.935f * detectorState[3] + 0.065f * air;

    envelopes[0] = juce::jmax(absMono, envelopes[0] * 0.995f);
    envelopes[1] = juce::jmax(detectorState[1], envelopes[1] * 0.996f);
    envelopes[2] = juce::jmax(detectorState[2], envelopes[2] * 0.996f);
    envelopes[3] = juce::jmax(detectorState[3], envelopes[3] * 0.997f);
}

void SmartBands::process(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() < 2) return;

    auto* L = buffer.getWritePointer(0);
    auto* R = buffer.getWritePointer(1);
    const int N = buffer.getNumSamples();
    const float dynamicAmt = juce::jlimit(0.0f, 1.0f, command.dynamicAmount);

    for (int i = 0; i < N; ++i)
    {
        for (auto& s : smooth)
            s.skip(1);

        if ((coeffCounter++ & 31) == 0)
            refreshCoefficients();

        float l = L[i];
        float r = R[i];

        const float mono = 0.5f * (l + r);
        updateDetectorEnvelopes(mono);

        const float dynLowMid = 1.0f - juce::jlimit(0.0f, 0.25f, envelopes[1] * 0.18f * dynamicAmt);
        const float dynPres   = 1.0f - juce::jlimit(0.0f, 0.20f, envelopes[2] * 0.15f * dynamicAmt);
        const float dynAir    = 1.0f - juce::jlimit(0.0f, 0.18f, envelopes[3] * 0.12f * dynamicAmt);

        l = lowShelf[0].processSample(l);
        r = lowShelf[1].processSample(r);
        l = mudBell[0].processSample(l) * dynLowMid;
        r = mudBell[1].processSample(r) * dynLowMid;
        l = presenceBell[0].processSample(l) * dynPres;
        r = presenceBell[1].processSample(r) * dynPres;
        l = airShelf[0].processSample(l) * dynAir;
        r = airShelf[1].processSample(r) * dynAir;

        const float mid = 0.5f * (l + r);
        float side = 0.5f * (l - r);
        side *= (1.0f + smooth[4].getCurrentValue());
        const float trim = dbToLin(smooth[5].getCurrentValue());
        l = (mid + side) * trim;
        r = (mid - side) * trim;

        L[i] = juce::jlimit(-1.0f, 1.0f, l);
        R[i] = juce::jlimit(-1.0f, 1.0f, r);
    }
}

}
