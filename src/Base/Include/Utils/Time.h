#pragma once

#include <QElapsedTimer>

class Time {
public:
    [[nodiscard]] float getDeltaTime() const { return deltaTime_; }
    [[nodiscard]] float getTime() const { return static_cast<float>(lastTimeRender_) / 1000.0; }

    void update();
    void reset();

private:
    float deltaTime_ = 0;
    qint64 lastTimeRender_ = 0;
    QElapsedTimer timer_{};
};
