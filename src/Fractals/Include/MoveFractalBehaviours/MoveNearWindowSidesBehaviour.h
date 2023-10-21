#pragma once

#include "BaseMoveBehaviour.h"

namespace Fractals {
    class MoveNearWindowSidesBehaviour : public BaseMoveBehaviour {
    public:
        explicit MoveNearWindowSidesBehaviour(float speed)
                : speed_(speed) {}

        void mouseMoveEvent(FractalView &view, BaseFractalRenderer &renderer, QMouseEvent *mouseEvent) override;

        void update(FractalView &view, BaseFractalRenderer &renderer, double deltaTime) override;

    private:
        bool isMoved_ = false;
        QVector2D differenceMove_{0, 0};
        const float speed_;
    };
}
