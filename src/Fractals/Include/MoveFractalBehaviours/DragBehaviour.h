#pragma once

#include "BaseMoveBehaviour.h"

namespace Fractals {
    class DragBehaviour : public BaseMoveBehaviour {
    public:
        void mousePressEvent(FractalView &, BaseFractalRenderer &renderer, QMouseEvent *mouseEvent) override;

        void mouseMoveEvent(FractalView &, BaseFractalRenderer &renderer, QMouseEvent *mouseEvent) override;

        void mouseReleaseEvent(FractalView &, BaseFractalRenderer &renderer, QMouseEvent *) override;

    private:
        bool isDragged_ = false;
        QVector2D lastMousePosition_{0, 0};
    };
}
