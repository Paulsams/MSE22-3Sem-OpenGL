#include "FrameCounter.h"
#include "DebugWindow.h"

FrameCounter::FrameCounter(DebugWindow &debugWindow, size_t updateStepInMs)
        : updateStepInMs_(updateStepInMs), fpsCounter_(debugWindow.addReadonlyLabel("FPS", "FPS"))
    {
        timer_.start();
    }

void FrameCounter::update() {
    ++accumulativeCount_;

    if (static_cast<quint64>(timer_.elapsed()) >= updateStepInMs_)
    {
        const float elapsedSeconds = static_cast<float>(timer_.restart()) / static_cast<float>(updateStepInMs_);
        count_ = static_cast<size_t>(qRound(static_cast<float>(accumulativeCount_) / elapsedSeconds));

        fpsCounter_->setText(std::to_string(count_).c_str());
        accumulativeCount_ = 0;
    }
}


