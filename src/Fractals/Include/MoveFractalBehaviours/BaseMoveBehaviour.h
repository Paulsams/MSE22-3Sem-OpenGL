#pragma once

#include <QMouseEvent>

namespace Fractals {
    class FractalView;
    struct BaseFractalRenderer;

    struct BaseMoveBehaviour {
        virtual ~BaseMoveBehaviour() = default;

        virtual void update(FractalView &, BaseFractalRenderer &, double) { }

        virtual void mousePressEvent(FractalView &, BaseFractalRenderer &, QMouseEvent *) { }
        virtual void mouseMoveEvent(FractalView &, BaseFractalRenderer &, QMouseEvent *) { }
        virtual void mouseReleaseEvent(FractalView &, BaseFractalRenderer &, QMouseEvent *) { }
    };
}
