#include "FrameCounter.h"
#include "DebugWindow.h"

FrameCounter::FrameCounter(DebugWindow &debugWindow, size_t updateStepInMs)
        : fpsCounter_(debugWindow.addReadonlyLabel("FPS", "FPS")), updateStepInMs_(updateStepInMs)
    {
        timer_.start();
    }

void FrameCounter::update() {
    ++accumulativeCount_;

    if (static_cast<quint64>(timer_.elapsed()) >= updateStepInMs_)
    {
        const auto elapsedSeconds = static_cast<float>(timer_.restart()) / static_cast<float>(updateStepInMs_);
        count_ = static_cast<size_t>(std::round(static_cast<float>(accumulativeCount_) / elapsedSeconds));

        fpsCounter_->setText(std::to_string(count_).c_str());
        accumulativeCount_ = 0;
    }
}


