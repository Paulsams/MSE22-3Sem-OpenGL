#pragma once

#include <QElapsedTimer>

class Time {
public:
    [[nodiscard]] double getDeltaTime() const { return deltaTime_; }
    [[nodiscard]] double getTime() const { return static_cast<double>(lastTimeRender_) / 1000.0; }

    void update();

private:
    double deltaTime_ = 0;
    qint64 lastTimeRender_ = 0;
    QElapsedTimer timer_{};
};
