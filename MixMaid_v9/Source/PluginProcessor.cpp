#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
float clampDb(float db, float limit) { return juce::jlimit(-limit, limit, db); }
}

MixMaidAudioProcessor::MixMaidAudioProcessor()
: AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                  .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
  apvts(*this, nullptr, "PARAMS", createParameters())
{
    for (auto* id : { IDs::optimize, IDs::strength, IDs::speed, IDs::bassFocus, IDs::stereoAssist,
                      IDs::dynamicAmount, IDs::target, IDs::safeMode, IDs::lowProtect,
                      IDs::inputTrim, IDs::outGain, IDs::mix, IDs::learn, IDs::referenceBlend, IDs::delta })
        apvts.addParameterListener(id, this);
}

MixMaidAudioProcessor::~MixMaidAudioProcessor()
{
    for (auto* id : { IDs::optimize, IDs::strength, IDs::speed, IDs::bassFocus, IDs::stereoAssist,
                      IDs::dynamicAmount, IDs::target, IDs::safeMode, IDs::lowProtect,
                      IDs::inputTrim, IDs::outGain, IDs::mix, IDs::learn, IDs::referenceBlend, IDs::delta })
        apvts.removeParameterListener(id, this);
}

juce::AudioProcessorValueTreeState::ParameterLayout MixMaidAudioProcessor::createParameters()
{
    using F = juce::AudioParameterFloat;
    using B = juce::AudioParameterBool;
    using C = juce::AudioParameterChoice;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<B>(IDs::optimize, "Optimize", true));
    layout.add(std::make_unique<F>(IDs::strength, "Strength", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 58.0f));
    layout.add(std::make_unique<F>(IDs::speed, "Speed", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 48.0f));
    layout.add(std::make_unique<F>(IDs::bassFocus, "Bass Focus", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 54.0f));
    layout.add(std::make_unique<F>(IDs::stereoAssist, "Stereo Assist", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 32.0f));
    layout.add(std::make_unique<F>(IDs::dynamicAmount, "Dynamic Amount", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 46.0f));
    layout.add(std::make_unique<C>(IDs::target, "Target", juce::StringArray{ "Universal", "EDM", "Hip-Hop", "Rock", "Podcast" }, 0));
    layout.add(std::make_unique<B>(IDs::safeMode, "Safe Mode", true));
    layout.add(std::make_unique<B>(IDs::lowProtect, "Low Protect", true));
    layout.add(std::make_unique<F>(IDs::inputTrim, "Input Trim", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<F>(IDs::outGain, "Output", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<F>(IDs::mix, "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 100.0f));
    layout.add(std::make_unique<B>(IDs::learn, "Learn", false));
    layout.add(std::make_unique<F>(IDs::referenceBlend, "Reference Blend", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<B>(IDs::delta, "Delta", false));
    return layout;
}

void MixMaidAudioProcessor::parameterChanged(const juce::String&, float)
{
    stateDirty.store(true, std::memory_order_release);
}

bool MixMaidAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getChannelSet(true, 0) == juce::AudioChannelSet::stereo()
        && layouts.getChannelSet(false, 0) == juce::AudioChannelSet::stereo();
}

void MixMaidAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    analyzer.prepare(sampleRate, samplesPerBlock);
    smartBands.prepare(sampleRate, samplesPerBlock, 2);
    limiter.prepare(sampleRate, samplesPerBlock, 2);
    preparedDryCapacity = juce::jmax(samplesPerBlock, 32768);
    dryBuffer.setSize(2, preparedDryCapacity, false, false, true);
    inputTrim.reset(sr, 0.05);
    outputTrim.reset(sr, 0.05);
    wetMixSmoothed.reset(sr, 0.03);
    detector.fill(0.0f);
    currentTilt.fill(0.0f);
    stateDirty.store(true);
    refreshFromParameters();
}

void MixMaidAudioProcessor::releaseResources() {}

void MixMaidAudioProcessor::updateTargets()
{
    const int idx = (int) apvts.getRawParameterValue(IDs::target)->load();
    switch (idx)
    {
        case 1: tonalTarget = { 0.23f, 0.20f, 0.25f, 0.18f, 0.14f }; break;
        case 2: tonalTarget = { 0.25f, 0.23f, 0.24f, 0.16f, 0.12f }; break;
        case 3: tonalTarget = { 0.17f, 0.22f, 0.30f, 0.19f, 0.12f }; break;
        case 4: tonalTarget = { 0.10f, 0.19f, 0.39f, 0.23f, 0.09f }; break;
        default: tonalTarget = { 0.18f, 0.22f, 0.28f, 0.18f, 0.14f }; break;
    }
}

void MixMaidAudioProcessor::refreshFromParameters()
{
    if (! stateDirty.exchange(false)) return;
    updateTargets();
    const auto speed = apvts.getRawParameterValue(IDs::speed)->load() / 100.0f;
    attackCoeff = juce::jmap(speed, 0.05f, 0.35f);
    releaseCoeff = juce::jmap(speed, 0.997f, 0.93f);
    inputTrim.setTargetValue(apvts.getRawParameterValue(IDs::inputTrim)->load());
    outputTrim.setTargetValue(apvts.getRawParameterValue(IDs::outGain)->load());
    wetMixSmoothed.setTargetValue(apvts.getRawParameterValue(IDs::mix)->load() / 100.0f);

    const bool learning = apvts.getRawParameterValue(IDs::learn)->load() > 0.5f;
    if (learning && ! wasLearning)
        referenceMatcher.reset();
    referenceMatcher.setLearning(learning);
    wasLearning = learning;
}

void MixMaidAudioProcessor::analyseBlock(const juce::AudioBuffer<float>& buffer)
{
    analyzer.push(buffer);
    const auto snap = analyzer.getSnapshot();
    referenceMatcher.ingest(snap);

    const std::array<float, 5> fallback {
        tonalTarget.low, tonalTarget.lowMid, tonalTarget.mid, tonalTarget.presence, tonalTarget.air
    };
    const float refBlend = apvts.getRawParameterValue(IDs::referenceBlend)->load() / 100.0f;
    const auto learnedTarget = referenceMatcher.getTargetBlend(fallback, refBlend);

    const float totalEnergy = juce::jmax(0.0001f, snap.lowEnergy + snap.lowMidEnergy + snap.midEnergy + snap.presenceEnergy + snap.airEnergy);
    std::array<float, 5> actual {
        snap.lowEnergy / totalEnergy,
        snap.lowMidEnergy / totalEnergy,
        snap.midEnergy / totalEnergy,
        snap.presenceEnergy / totalEnergy,
        snap.airEnergy / totalEnergy
    };

    const auto& target = learnedTarget.distribution;
    const float strength = apvts.getRawParameterValue(IDs::strength)->load() / 100.0f;
    const float bassFocus = apvts.getRawParameterValue(IDs::bassFocus)->load() / 100.0f;
    const float stereoAssist = apvts.getRawParameterValue(IDs::stereoAssist)->load() / 100.0f;
    const float dynamicAmount = apvts.getRawParameterValue(IDs::dynamicAmount)->load() / 100.0f;
    const bool safeMode = apvts.getRawParameterValue(IDs::safeMode)->load() > 0.5f;
    const bool lowProtect = apvts.getRawParameterValue(IDs::lowProtect)->load() > 0.5f;
    const float maxEq = safeMode ? 3.0f : 6.0f;
    const float maxWidth = safeMode ? 0.15f : 0.3f;

    for (int i = 0; i < 5; ++i)
    {
        const float rawTilt = juce::jlimit(-1.0f, 1.0f, (target[(size_t) i] - actual[(size_t) i]) * 5.2f);
        detector[(size_t) i] = rawTilt > detector[(size_t) i]
            ? detector[(size_t) i] + (rawTilt - detector[(size_t) i]) * attackCoeff
            : detector[(size_t) i] * releaseCoeff + rawTilt * (1.0f - releaseCoeff);
        currentTilt[(size_t) i] = detector[(size_t) i];
    }

    command.lowDb = clampDb(currentTilt[0] * (1.0f + bassFocus * 0.40f) * maxEq * strength * (lowProtect ? 0.55f : 1.0f), maxEq);
    command.lowMidDb = clampDb(-currentTilt[1] * maxEq * strength, maxEq);
    command.presenceDb = clampDb(currentTilt[3] * maxEq * strength, maxEq);
    command.airDb = clampDb(currentTilt[4] * maxEq * strength, maxEq);
    command.width = juce::jlimit(-maxWidth, maxWidth, (0.35f - snap.correlation) * stereoAssist * maxWidth);
    command.dynamicAmount = dynamicAmount;

    const float peak = juce::jmax(snap.peakL, snap.peakR);
    command.trimDb = juce::jlimit(-2.5f, 2.5f, juce::jmap(peak, 0.35f, 0.97f, 1.0f, -1.8f));

    limiter.setDrive(juce::jmax(0.0f, std::abs(command.trimDb) * 0.5f + dynamicAmount * 2.0f));
    limiter.setCeiling(safeMode ? -1.0f : -0.3f);

    meters.lowTilt.store(command.lowDb);
    meters.presenceTilt.store(command.presenceDb);
    meters.highTilt.store(command.airDb);
    meters.widthPct.store((1.0f + command.width) * 100.0f);
    meters.referenceConfidence.store(referenceMatcher.getConfidence() * 100.0f);
}

void MixMaidAudioProcessor::applySmartCorrection(juce::AudioBuffer<float>& buffer)
{
    if (apvts.getRawParameterValue(IDs::optimize)->load() < 0.5f)
        return;

    const float trimStart = dbToLin(inputTrim.getCurrentValue());
    const float trimEnd   = dbToLin(inputTrim.skip(buffer.getNumSamples()));
    for (int ch = 0; ch < juce::jmin(2, buffer.getNumChannels()); ++ch)
        buffer.applyGainRamp(ch, 0, buffer.getNumSamples(), trimStart, trimEnd);

    smartBands.setCommand(command, apvts.getRawParameterValue(IDs::safeMode)->load() > 0.5f);
    smartBands.process(buffer);
    limiter.process(buffer);

    const float outTrimStart = dbToLin(outputTrim.getCurrentValue());
    const float outTrimEnd   = dbToLin(outputTrim.skip(buffer.getNumSamples()));
    for (int ch = 0; ch < juce::jmin(2, buffer.getNumChannels()); ++ch)
        buffer.applyGainRamp(ch, 0, buffer.getNumSamples(), outTrimStart, outTrimEnd);

    const bool delta = apvts.getRawParameterValue(IDs::delta)->load() > 0.5f;
    if (delta)
    {
        for (int ch = 0; ch < juce::jmin(2, buffer.getNumChannels()); ++ch)
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                buffer.setSample(ch, i, buffer.getSample(ch, i) - dryBuffer.getSample(ch, i));
    }
    else
    {
        const float wetStart = juce::jlimit(0.0f, 1.0f, wetMixSmoothed.getCurrentValue());
        const float wetEnd   = juce::jlimit(0.0f, 1.0f, wetMixSmoothed.skip(buffer.getNumSamples()));
        const float dryStart = 1.0f - wetStart;
        const float dryEnd   = 1.0f - wetEnd;
        for (int ch = 0; ch < juce::jmin(2, buffer.getNumChannels()); ++ch)
        {
            buffer.applyGainRamp(ch, 0, buffer.getNumSamples(), wetStart, wetEnd);
            buffer.addFromWithRamp(ch, 0, dryBuffer.getReadPointer(ch), buffer.getNumSamples(), dryStart, dryEnd);
        }
    }

    auto env = smartBands.getEnvelopeState();
    meters.dynamicLoad.store((env[1] + env[2] + env[3]) * 33.3f);
    meters.lowMidLoad.store(env[1] * 100.0f);
    meters.limiterReduction.store(limiter.getReductionDb());
    meters.correctionDb.store(std::abs(command.lowDb) + std::abs(command.lowMidDb) + std::abs(command.presenceDb) + std::abs(command.airDb));
}

void MixMaidAudioProcessor::computeOutputMeters(const juce::AudioBuffer<float>& buffer)
{
    const float outRms = 0.5f * (buffer.getRMSLevel(0, 0, buffer.getNumSamples()) + buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    const float peak = juce::jmax(buffer.getMagnitude(0, 0, buffer.getNumSamples()), buffer.getMagnitude(1, 0, buffer.getNumSamples()));
    meters.outputRms.store(outRms);
    meters.headroomDb.store(-linToDb(juce::jmax(peak, 1.0e-6f)));
}

void MixMaidAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    if (buffer.getNumChannels() < 2)
        return;

    refreshFromParameters();
    if (buffer.getNumSamples() > preparedDryCapacity || dryBuffer.getNumSamples() < preparedDryCapacity)
    {
        meters.inputRms.store(0.5f * (buffer.getRMSLevel(0, 0, buffer.getNumSamples())
                                    + buffer.getRMSLevel(1, 0, buffer.getNumSamples())));
        meters.outputRms.store(meters.inputRms.load());
        meters.correctionDb.store(0.0f);
        meters.dynamicLoad.store(0.0f);
        meters.lowMidLoad.store(0.0f);
        meters.limiterReduction.store(0.0f);
        return; // fail-closed on unexpected oversized host blocks; no audio-thread reallocation
    }
    for (int ch = 0; ch < 2; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, buffer.getNumSamples());
    const float inRms = 0.5f * (dryBuffer.getRMSLevel(0, 0, buffer.getNumSamples()) + dryBuffer.getRMSLevel(1, 0, buffer.getNumSamples()));
    meters.inputRms.store(inRms);

    analyseBlock(dryBuffer);
    applySmartCorrection(buffer);
    computeOutputMeters(buffer);
}

void MixMaidAudioProcessor::storeSnapshotA() { abState.storeA(apvts); }
void MixMaidAudioProcessor::storeSnapshotB() { abState.storeB(apvts); }
void MixMaidAudioProcessor::recallSnapshotA() { if (abState.restoreA(apvts)) stateDirty.store(true); }
void MixMaidAudioProcessor::recallSnapshotB() { if (abState.restoreB(apvts)) stateDirty.store(true); }

void MixMaidAudioProcessor::applyBuiltInPreset(int presetIndex)
{
    const auto presets = mixmaid::PresetBank::builtins();
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;

    const auto& p = presets[(size_t)presetIndex];
    auto setFloat = [this](const char* id, float value)
    {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    };

    setFloat(IDs::strength, p.strength);
    setFloat(IDs::speed, p.speed);
    setFloat(IDs::bassFocus, p.bassFocus);
    setFloat(IDs::stereoAssist, p.stereoAssist);
    setFloat(IDs::dynamicAmount, p.dynamicAmount);
    setFloat(IDs::referenceBlend, p.referenceBlend);
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(IDs::target)))
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.target)));
    stateDirty.store(true);
}

juce::AudioProcessorEditor* MixMaidAudioProcessor::createEditor() { return new MixMaidAudioProcessorEditor(*this); }



bool MixMaidAudioProcessor::saveUserPresetToFile(const juce::File& file)
{
    mixmaid::PresetDefinition p;
    p.name = file.getFileNameWithoutExtension();
    p.strength = apvts.getRawParameterValue(IDs::strength)->load();
    p.speed = apvts.getRawParameterValue(IDs::speed)->load();
    p.bassFocus = apvts.getRawParameterValue(IDs::bassFocus)->load();
    p.stereoAssist = apvts.getRawParameterValue(IDs::stereoAssist)->load();
    p.dynamicAmount = apvts.getRawParameterValue(IDs::dynamicAmount)->load();
    p.referenceBlend = apvts.getRawParameterValue(IDs::referenceBlend)->load();
    p.target = (int) apvts.getRawParameterValue(IDs::target)->load();
    return mixmaid::PresetBank::savePresetToFile(p, file);
}

bool MixMaidAudioProcessor::loadUserPresetFromFile(const juce::File& file)
{
    auto loaded = mixmaid::PresetBank::loadPresetFromFile(file);
    if (!loaded.has_value())
        return false;

    const auto& p = *loaded;
    auto setFloat = [this](const char* id, float value)
    {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    };

    setFloat(IDs::strength, p.strength);
    setFloat(IDs::speed, p.speed);
    setFloat(IDs::bassFocus, p.bassFocus);
    setFloat(IDs::stereoAssist, p.stereoAssist);
    setFloat(IDs::dynamicAmount, p.dynamicAmount);
    setFloat(IDs::referenceBlend, p.referenceBlend);
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(IDs::target)))
        param->setValueNotifyingHost(param->convertTo0to1((float) p.target));
    stateDirty.store(true);
    return true;
}

void MixMaidAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void MixMaidAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
    stateDirty.store(true);
}
