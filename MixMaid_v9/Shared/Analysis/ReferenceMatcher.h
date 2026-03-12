#pragma once
#include <array>
#include "AnalyzerState.h"

namespace mixmaid {

struct ReferenceTarget
{
    std::array<float, 5> distribution { 0.18f, 0.22f, 0.28f, 0.18f, 0.14f };
    bool valid = false;
};

class ReferenceMatcher
{
public:
    void reset() noexcept;
    void setLearning(bool shouldLearn) noexcept { learning = shouldLearn; }
    bool isLearning() const noexcept { return learning; }
    void ingest(const AnalyzerSnapshot& snapshot) noexcept;
    ReferenceTarget getTargetBlend(const std::array<float, 5>& fallback, float blend) const noexcept;
    float getConfidence() const noexcept;

private:
    bool learning = false;
    int frames = 0;
    std::array<double, 5> accum { 0, 0, 0, 0, 0 };
};

}
