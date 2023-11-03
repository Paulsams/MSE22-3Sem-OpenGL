#include "Time.h"

void Time::update() {
    const qint64 currentTime = timer_.elapsed();
    deltaTime_ = static_cast<double>(currentTime - lastTimeRender_) / 1000.0;
    lastTimeRender_ = currentTime;
}
