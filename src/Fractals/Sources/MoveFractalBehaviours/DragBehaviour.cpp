#include "MoveFractalBehaviours/DragBehaviour.h"
#include "FractalView.h"
#include "FractalRenderers/BaseFractalRenderer.h"

namespace Fractals {
    void DragBehaviour::mousePressEvent(FractalView &, BaseFractalRenderer &, QMouseEvent *mouseEvent) {
        isDragged_ = true;
        lastMousePosition_ = QVector2D(mouseEvent->localPos());
    }

    void DragBehaviour::mouseMoveEvent(FractalView &view, BaseFractalRenderer &renderer, QMouseEvent *mouseEvent) {
        if (!isDragged_)
            return;

        const QVector2D currentPosition = QVector2D(mouseEvent->localPos());
        QVector2D difference = currentPosition - lastMousePosition_;
        difference.setX(-difference.x());

        const auto aspect = renderer.getAspectScreenToWorld(view);
        auto shift = difference / QVector2D(view.width(), view.height()) /
                     renderer.zoom * QVector2D(aspect.width(), aspect.height());
        renderer.viewPosition += shift;

        lastMousePosition_ = currentPosition;
    }

    void DragBehaviour::mouseReleaseEvent(FractalView &, BaseFractalRenderer &, QMouseEvent *) {
        isDragged_ = false;
    }
}
