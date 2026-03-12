#include "PluginEditor.h"

namespace {
float toDb(float lin) { return juce::Decibels::gainToDecibels(lin, -100.0f); }
juce::Colour cyan() { return juce::Colour(76, 224, 255); }
juce::Colour violet() { return juce::Colour(182, 100, 255); }
juce::Colour amber() { return juce::Colour(255, 194, 92); }
}

void SpectrumPanel::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced(8.0f);
    g.setColour(juce::Colours::black.withAlpha(0.18f));
    g.fillRoundedRectangle(area, 18.0f);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawRoundedRectangle(area, 18.0f, 1.0f);

    auto snap = processor.getAnalyzerSnapshot();
    juce::Path p;
    const float w = area.getWidth();
    const float h = area.getHeight();
    for (int i = 0; i < mixmaid::kVisualBands; ++i)
    {
        const float x = area.getX() + w * ((float)i / (float)(mixmaid::kVisualBands - 1));
        const float y = area.getBottom() - snap.spectrum[(size_t)i] * (h - 16.0f);
        if (i == 0) p.startNewSubPath(x, y);
        else p.lineTo(x, y);
    }
    g.setColour(cyan().withAlpha(0.95f));
    g.strokePath(p, juce::PathStrokeType(2.0f));

    auto drawGuide = [&](float norm, const juce::String& label)
    {
        const float x = area.getX() + area.getWidth() * norm;
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawVerticalLine((int)x, area.getY(), area.getBottom());
        g.setColour(juce::Colours::white.withAlpha(0.55f));
        g.setFont(11.0f);
        g.drawText(label, (int)x + 4, (int)area.getY() + 6, 64, 16, juce::Justification::left);
    };
    drawGuide(0.18f, "LOW");
    drawGuide(0.34f, "MUD");
    drawGuide(0.58f, "MID");
    drawGuide(0.74f, "PRES");
    drawGuide(0.90f, "AIR");
}

MixMaidAudioProcessorEditor::MixMaidAudioProcessorEditor (MixMaidAudioProcessor& p)
: AudioProcessorEditor(&p), audioProcessor(p), spectrum(p)
{
    setSize(1320, 760);
    title.setText("MIXMAID", juce::dontSendNotification);
    title.setFont(juce::FontOptions(34.0f, juce::Font::bold));
    subtitle.setText("AAAAAA smart mix correction / tonal targeting / reference learn / A-B snapshots", juce::dontSendNotification);

    for (auto* l : { &title, &subtitle, &inLbl, &outLbl, &correctionLbl, &widthLbl, &headroomLbl, &dynamicLbl, &lowMidLbl, &refLbl, &limitLbl })
        addAndMakeVisible(*l);

    optimize.setButtonText("Optimize");
    safeMode.setButtonText("Safe Mode");
    lowProtect.setButtonText("Low Protect");
    learn.setButtonText("Learn Ref");
    delta.setButtonText("Delta");
    for (auto* b : { &optimize, &safeMode, &lowProtect, &learn, &delta }) addAndMakeVisible(*b);

    snapAStore.setButtonText("Store A");
    snapARecall.setButtonText("Recall A");
    snapBStore.setButtonText("Store B");
    snapBRecall.setButtonText("Recall B");
    loadPreset.setButtonText("Load Preset");
    savePreset.setButtonText("Save Preset");
    for (auto* b : { &snapAStore, &snapARecall, &snapBStore, &snapBRecall, &loadPreset, &savePreset })
    {
        addAndMakeVisible(*b);
        b->addListener(this);
    }

    for (auto* s : { &strength, &speed, &bassFocus, &stereoAssist, &dynamicAmount, &inputTrim, &outGain, &mix, &referenceBlend })
    {
        juce::String n;
        if (s == &strength) n = "Strength";
        else if (s == &speed) n = "Speed";
        else if (s == &bassFocus) n = "Bass Focus";
        else if (s == &stereoAssist) n = "Stereo Assist";
        else if (s == &dynamicAmount) n = "Dynamic";
        else if (s == &inputTrim) n = "Input";
        else if (s == &outGain) n = "Output";
        else if (s == &mix) n = "Mix";
        else n = "Ref Blend";
        setupKnob(*s, n);
    }

    target.addItemList({ "Universal", "EDM", "Hip-Hop", "Rock", "Podcast" }, 1);
    builtInPresets.addItemList({ "Universal", "EDM Punch", "Hip-Hop Weight", "Rock Clarity", "Podcast Focus" }, 1);
    builtInPresets.onChange = [this]
    {
        const int idx = builtInPresets.getSelectedItemIndex();
        if (idx >= 0)
            audioProcessor.applyBuiltInPreset(idx);
    };
    addAndMakeVisible(target);
    addAndMakeVisible(builtInPresets);
    addAndMakeVisible(spectrum);

    auto& apvts = audioProcessor.apvts;
    aOptimize = std::make_unique<BA>(apvts, MixMaidAudioProcessor::IDs::optimize, optimize);
    aSafeMode = std::make_unique<BA>(apvts, MixMaidAudioProcessor::IDs::safeMode, safeMode);
    aLowProtect = std::make_unique<BA>(apvts, MixMaidAudioProcessor::IDs::lowProtect, lowProtect);
    aLearn = std::make_unique<BA>(apvts, MixMaidAudioProcessor::IDs::learn, learn);
    aDelta = std::make_unique<BA>(apvts, MixMaidAudioProcessor::IDs::delta, delta);
    aStrength = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::strength, strength);
    aSpeed = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::speed, speed);
    aBassFocus = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::bassFocus, bassFocus);
    aStereoAssist = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::stereoAssist, stereoAssist);
    aDynamicAmount = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::dynamicAmount, dynamicAmount);
    aInputTrim = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::inputTrim, inputTrim);
    aOutGain = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::outGain, outGain);
    aMix = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::mix, mix);
    aReferenceBlend = std::make_unique<SA>(apvts, MixMaidAudioProcessor::IDs::referenceBlend, referenceBlend);
    aTarget = std::make_unique<CA>(apvts, MixMaidAudioProcessor::IDs::target, target);

    startTimerHz(20);
}

void MixMaidAudioProcessorEditor::setupKnob(juce::Slider& slider, const juce::String& name)
{
    slider.setName(name);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 84, 20);
    addAndMakeVisible(slider);
}

void MixMaidAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Colour bgA(7, 10, 18), bgB(11, 22, 34);
    g.setGradientFill(juce::ColourGradient(bgA, 0, 0, bgB, 0, (float)getHeight(), false));
    g.fillAll();
    g.setColour(violet().withAlpha(0.12f));
    g.fillEllipse(44.0f, 56.0f, 280.0f, 180.0f);
    g.setColour(cyan().withAlpha(0.10f));
    g.fillEllipse((float)getWidth() - 340.0f, 40.0f, 260.0f, 180.0f);
    auto frame = getLocalBounds().reduced(14).toFloat();
    g.setColour(juce::Colours::white.withAlpha(0.10f));
    g.drawRoundedRectangle(frame, 22.0f, 1.0f);
    g.setColour(amber().withAlpha(0.75f));
    g.fillRoundedRectangle(26.0f, (float)getHeight() - 48.0f, 300.0f, 8.0f, 4.0f);
    g.setColour(juce::Colours::white.withAlpha(0.70f));
    g.drawText("Build-hardened candidate / bounded smart correction", 26, 90, 420, 18, juce::Justification::left);
}

void MixMaidAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);
    auto header = area.removeFromTop(84);
    title.setBounds(header.removeFromTop(38));
    subtitle.setBounds(header.removeFromTop(22));

    auto topRow = area.removeFromTop(42);
    optimize.setBounds(topRow.removeFromLeft(110));
    safeMode.setBounds(topRow.removeFromLeft(120));
    lowProtect.setBounds(topRow.removeFromLeft(120));
    learn.setBounds(topRow.removeFromLeft(120));
    delta.setBounds(topRow.removeFromLeft(90));
    target.setBounds(topRow.removeFromLeft(180).reduced(6, 0));
    builtInPresets.setBounds(topRow.removeFromLeft(200).reduced(6, 0));
    snapAStore.setBounds(topRow.removeFromLeft(90).reduced(4, 0));
    snapARecall.setBounds(topRow.removeFromLeft(96).reduced(4, 0));
    snapBStore.setBounds(topRow.removeFromLeft(90).reduced(4, 0));
    snapBRecall.setBounds(topRow.removeFromLeft(96).reduced(4, 0));
    loadPreset.setBounds(topRow.removeFromLeft(110).reduced(4, 0));
    savePreset.setBounds(topRow.removeFromLeft(110).reduced(4, 0));

    auto meters = area.removeFromBottom(48);
    inLbl.setBounds(meters.removeFromLeft(120));
    outLbl.setBounds(meters.removeFromLeft(120));
    correctionLbl.setBounds(meters.removeFromLeft(165));
    widthLbl.setBounds(meters.removeFromLeft(125));
    headroomLbl.setBounds(meters.removeFromLeft(140));
    dynamicLbl.setBounds(meters.removeFromLeft(120));
    lowMidLbl.setBounds(meters.removeFromLeft(120));
    refLbl.setBounds(meters.removeFromLeft(130));
    limitLbl.setBounds(meters.removeFromLeft(120));

    auto controls = area.removeFromBottom(220);
    const int knobW = controls.getWidth() / 9;
    for (auto* s : { &strength, &speed, &bassFocus, &stereoAssist, &dynamicAmount, &inputTrim, &outGain, &mix, &referenceBlend })
        s->setBounds(controls.removeFromLeft(knobW).reduced(8));

    spectrum.setBounds(area.reduced(0, 10));
}

void MixMaidAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &snapAStore) audioProcessor.storeSnapshotA();
    else if (button == &snapARecall) audioProcessor.recallSnapshotA();
    else if (button == &snapBStore) audioProcessor.storeSnapshotB();
    else if (button == &snapBRecall) audioProcessor.recallSnapshotB();
    else if (button == &savePreset)
    {
        auto dir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("MixMaid Presets");
        dir.createDirectory();
        juce::FileChooser chooser("Save MixMaid preset", dir, "*.mmix");
        if (chooser.browseForFileToSave(true))
            audioProcessor.saveUserPresetToFile(chooser.getResult());
    }
    else if (button == &loadPreset)
    {
        auto dir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("MixMaid Presets");
        dir.createDirectory();
        juce::FileChooser chooser("Load MixMaid preset", dir, "*.mmix");
        if (chooser.browseForFileToOpen())
            audioProcessor.loadUserPresetFromFile(chooser.getResult());
    }
}

void MixMaidAudioProcessorEditor::timerCallback()
{
    auto& m = audioProcessor.meters;
    inLbl.setText("In: " + juce::String(toDb(m.inputRms.load()), 1) + " dB", juce::dontSendNotification);
    outLbl.setText("Out: " + juce::String(toDb(m.outputRms.load()), 1) + " dB", juce::dontSendNotification);
    correctionLbl.setText("Correction: " + juce::String(m.correctionDb.load(), 1) + " dB", juce::dontSendNotification);
    widthLbl.setText("Width: " + juce::String(m.widthPct.load(), 1) + "%", juce::dontSendNotification);
    headroomLbl.setText("Headroom: " + juce::String(m.headroomDb.load(), 1) + " dB", juce::dontSendNotification);
    dynamicLbl.setText("Dyn: " + juce::String(m.dynamicLoad.load(), 1) + "%", juce::dontSendNotification);
    lowMidLbl.setText("Mud ctl: " + juce::String(m.lowMidLoad.load(), 1) + "%", juce::dontSendNotification);
    refLbl.setText("Ref: " + juce::String(m.referenceConfidence.load(), 1) + "%", juce::dontSendNotification);
    limitLbl.setText("Limit: " + juce::String(m.limiterReduction.load(), 1) + " dB", juce::dontSendNotification);
}
