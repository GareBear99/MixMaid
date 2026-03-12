#pragma once
#include <JuceHeader.h>
#include "../Shared/Analysis/AnalyzerState.h"
#include "../Shared/Analysis/ReferenceMatcher.h"
#include "../Shared/DSP/SmartBands.h"
#include "../Shared/DSP/SafetyLimiter.h"
#include "../Shared/Core/ABState.h"
#include "../Shared/Core/PresetBank.h"

class MixMaidAudioProcessor final : public juce::AudioProcessor,
                                    private juce::AudioProcessorValueTreeState::Listener
{
public:
    MixMaidAudioProcessor();
    ~MixMaidAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MixMaid"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    struct IDs
    {
        static constexpr const char* optimize = "optimize";
        static constexpr const char* strength = "strength";
        static constexpr const char* speed = "speed";
        static constexpr const char* bassFocus = "bassFocus";
        static constexpr const char* stereoAssist = "stereoAssist";
        static constexpr const char* dynamicAmount = "dynamicAmount";
        static constexpr const char* target = "target";
        static constexpr const char* safeMode = "safeMode";
        static constexpr const char* lowProtect = "lowProtect";
        static constexpr const char* inputTrim = "inputTrim";
        static constexpr const char* outGain = "outGain";
        static constexpr const char* mix = "mix";
        static constexpr const char* learn = "learn";
        static constexpr const char* referenceBlend = "referenceBlend";
        static constexpr const char* delta = "delta";
    };

    struct MeterState
    {
        std::atomic<float> inputRms { 0.0f };
        std::atomic<float> outputRms { 0.0f };
        std::atomic<float> correctionDb { 0.0f };
        std::atomic<float> widthPct { 100.0f };
        std::atomic<float> headroomDb { 0.0f };
        std::atomic<float> lowTilt { 0.0f };
        std::atomic<float> highTilt { 0.0f };
        std::atomic<float> presenceTilt { 0.0f };
        std::atomic<float> dynamicLoad { 0.0f };
        std::atomic<float> lowMidLoad { 0.0f };
        std::atomic<float> referenceConfidence { 0.0f };
        std::atomic<float> limiterReduction { 0.0f };
    } meters;

    mixmaid::AnalyzerSnapshot getAnalyzerSnapshot() const { return analyzer.getSnapshot(); }
    float getReferenceConfidence() const noexcept { return referenceMatcher.getConfidence(); }
    void storeSnapshotA();
    void storeSnapshotB();
    void recallSnapshotA();
    void recallSnapshotB();
    void applyBuiltInPreset(int presetIndex);
    bool saveUserPresetToFile(const juce::File& file);
    bool loadUserPresetFromFile(const juce::File& file);

private:
    struct TonalTarget
    {
        float low = 0.18f;
        float lowMid = 0.22f;
        float mid = 0.28f;
        float presence = 0.18f;
        float air = 0.14f;
    };

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void updateTargets();
    void analyseBlock(const juce::AudioBuffer<float>& buffer);
    void applySmartCorrection(juce::AudioBuffer<float>& buffer);
    void computeOutputMeters(const juce::AudioBuffer<float>& buffer);
    void refreshFromParameters();

    static float dbToLin(float db) { return juce::Decibels::decibelsToGain(db); }
    static float linToDb(float lin) { return juce::Decibels::gainToDecibels(lin, -100.0f); }

    double sr = 44100.0;
    mixmaid::AnalyzerState analyzer;
    mixmaid::ReferenceMatcher referenceMatcher;
    mixmaid::SmartBands smartBands;
    mixmaid::SafetyLimiter limiter;
    mixmaid::ABState abState;
    juce::AudioBuffer<float> dryBuffer;
    int preparedDryCapacity = 0;
    juce::SmoothedValue<float> inputTrim;
    juce::SmoothedValue<float> outputTrim;
    juce::SmoothedValue<float> wetMixSmoothed;
    std::atomic<bool> stateDirty { true };
    TonalTarget tonalTarget;
    std::array<float, 5> currentTilt {};
    std::array<float, 5> detector {};
    float attackCoeff = 0.18f;
    float releaseCoeff = 0.98f;
    mixmaid::BandCommand command;
    bool wasLearning = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixMaidAudioProcessor)
};
