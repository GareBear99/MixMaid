#include "AnalyzerState.h"

namespace mixmaid {
AnalyzerState::AnalyzerState() = default;

void AnalyzerState::prepare(double newSampleRate, int)
{
    const juce::ScopedLock sl(lock);
    sampleRate = newSampleRate;
    fifoIndex = 0;
    fifo.fill(0.0f);
    fftData.fill(0.0f);
    snapshot = {};
}

void AnalyzerState::push(const juce::AudioBuffer<float>& buffer)
{
    const auto channels = juce::jmin(2, buffer.getNumChannels());
    if (channels <= 0) return;

    const auto numSamples = buffer.getNumSamples();
    const auto* left = buffer.getReadPointer(0);
    const auto* right = channels > 1 ? buffer.getReadPointer(1) : left;

    AnalyzerSnapshot local {};
    local.peakL = buffer.getMagnitude(0, 0, numSamples);
    local.rmsL = buffer.getRMSLevel(0, 0, numSamples);
    local.peakR = channels > 1 ? buffer.getMagnitude(1, 0, numSamples) : local.peakL;
    local.rmsR = channels > 1 ? buffer.getRMSLevel(1, 0, numSamples) : local.rmsL;

    float sumMul = 0.0f, sumL2 = 0.0f, sumR2 = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        const auto l = left[i];
        const auto r = right[i];
        pushSample(0.5f * (l + r));
        sumMul += l * r;
        sumL2 += l * l;
        sumR2 += r * r;
    }

    local.correlation = juce::jlimit(-1.0f, 1.0f, sumMul / (std::sqrt(sumL2 * sumR2) + 1.0e-6f));
    local.stereoWidth = juce::jlimit(0.0f, 1.0f, 1.0f - 0.5f * (local.correlation + 1.0f));

    {
        const juce::ScopedLock sl(lock);
        local.spectrum = snapshot.spectrum;
        local.lowEnergy = snapshot.lowEnergy;
        local.lowMidEnergy = snapshot.lowMidEnergy;
        local.midEnergy = snapshot.midEnergy;
        local.presenceEnergy = snapshot.presenceEnergy;
        local.airEnergy = snapshot.airEnergy;
        snapshot.peakL = local.peakL;
        snapshot.peakR = local.peakR;
        snapshot.rmsL = local.rmsL;
        snapshot.rmsR = local.rmsR;
        snapshot.correlation = local.correlation;
        snapshot.stereoWidth = local.stereoWidth;
    }
}

AnalyzerSnapshot AnalyzerState::getSnapshot() const
{
    const juce::ScopedLock sl(lock);
    return snapshot;
}

void AnalyzerState::pushSample(float mono)
{
    fifo[(size_t) fifoIndex++] = mono;
    if (fifoIndex == kFftSize)
    {
        computeFrame();
        fifoIndex = 0;
    }
}

void AnalyzerState::computeFrame()
{
    std::copy(fifo.begin(), fifo.end(), fftData.begin());
    std::fill(fftData.begin() + kFftSize, fftData.end(), 0.0f);
    window.multiplyWithWindowingTable(fftData.data(), kFftSize);
    fft.performFrequencyOnlyForwardTransform(fftData.data());

    const juce::ScopedLock sl(lock);
    snapshot.spectrum.fill(0.0f);

    double low = 0.0, lowMid = 0.0, mid = 0.0, presence = 0.0, air = 0.0;

    for (int bin = 1; bin < kFftSize / 2; ++bin)
    {
        const float magLin = fftData[(size_t) bin] / (float) kFftSize;
        const float energy = magLin * magLin;
        const auto magDb = juce::Decibels::gainToDecibels(magLin, -100.0f);
        const auto norm = juce::jlimit(0.0f, 1.0f, juce::jmap(magDb, -80.0f, 0.0f, 0.0f, 1.0f));
        const auto band = mapBinToBand(bin);
        snapshot.spectrum[(size_t) band] = juce::jmax(snapshot.spectrum[(size_t) band], norm);

        const auto frequency = (float) bin * (float) sampleRate / (float) kFftSize;
        if (frequency >= 20.0f && frequency < 80.0f) low += energy;
        else if (frequency >= 80.0f && frequency < 250.0f) lowMid += energy;
        else if (frequency >= 250.0f && frequency < 2000.0f) mid += energy;
        else if (frequency >= 2000.0f && frequency < 6000.0f) presence += energy;
        else if (frequency >= 6000.0f && frequency < 16000.0f) air += energy;
    }

    snapshot.lowEnergy = (float) std::sqrt(low);
    snapshot.lowMidEnergy = (float) std::sqrt(lowMid);
    snapshot.midEnergy = (float) std::sqrt(mid);
    snapshot.presenceEnergy = (float) std::sqrt(presence);
    snapshot.airEnergy = (float) std::sqrt(air);
}

float AnalyzerState::bandEnergyForHzRange(float minHz, float maxHz) const noexcept
{
    float sum = 0.0f;
    for (int bin = 1; bin < kFftSize / 2; ++bin)
    {
        const auto frequency = (float) bin * (float) sampleRate / (float) kFftSize;
        if (frequency < minHz || frequency >= maxHz)
            continue;
        const float magLin = fftData[(size_t) bin] / (float) kFftSize;
        sum += magLin * magLin;
    }
    return std::sqrt(sum);
}

int AnalyzerState::mapBinToBand(int fftBin) const noexcept
{
    const auto frequency = (float) fftBin * (float) sampleRate / (float) kFftSize;
    const auto logMin = std::log10(20.0f);
    const auto logMax = std::log10(20000.0f);
    const auto logFreq = std::log10(juce::jlimit(20.0f, 20000.0f, frequency));
    const auto t = (logFreq - logMin) / (logMax - logMin);
    return juce::jlimit(0, kVisualBands - 1, (int) std::floor(t * (float) (kVisualBands - 1)));
}
}
