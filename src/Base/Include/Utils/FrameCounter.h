#pragma once

#include <QLabel>
#include <QElapsedTimer>

class FieldsDrawer;

class FrameCounter {
public:
    explicit FrameCounter(FieldsDrawer& debugWindow, size_t updateStepInMs);

    void update();
    [[nodiscard]] size_t getCount() const { return count_; }

private:
    const size_t updateStepInMs_;

    QLabel* fpsCounter_;
    QElapsedTimer timer_;
    size_t accumulativeCount_ = 0;

    size_t count_ = 0;
};
