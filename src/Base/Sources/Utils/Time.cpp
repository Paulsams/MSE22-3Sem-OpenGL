#include "Utils/Time.h"

void Time::update() {
    const qint64 currentTime = timer_.elapsed();
    deltaTime_ = static_cast<float>(currentTime - lastTimeRender_) / 1000.0f;
    lastTimeRender_ = currentTime;
}

void Time::reset() {
    Time::update();
    deltaTime_ = 0.0f;
}
