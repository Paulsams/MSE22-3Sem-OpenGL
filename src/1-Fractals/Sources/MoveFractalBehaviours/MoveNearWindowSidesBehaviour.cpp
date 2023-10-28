#include "MoveFractalBehaviours/MoveNearWindowSidesBehaviour.h"
#include "FractalView.h"

namespace Fractals {
    void MoveNearWindowSidesBehaviour::mouseMoveEvent(FractalView &view, BaseFractalRenderer &,
                                                      QMouseEvent *mouseEvent) {
        constexpr float coefficientBoundsMoved = 0.8f;

        const auto retinaScale = view.devicePixelRatio();
        const auto localPosition = QVector2D(mouseEvent->localPos());
        const auto windowSize = QVector2D(view.width(), view.height()) * retinaScale;

        isMoved_ = localPosition.x() > windowSize.x() * coefficientBoundsMoved ||
                   localPosition.x() < windowSize.x() * (1.0f - coefficientBoundsMoved) ||
                   localPosition.y() > windowSize.y() * coefficientBoundsMoved ||
                   localPosition.y() < windowSize.y() * (1.0f - coefficientBoundsMoved);

        if (!isMoved_)
            return;

        differenceMove_ = (localPosition -
                           QVector2D(QPoint(view.width() / 2, view.height() / 2))).normalized();
        differenceMove_.setY(-differenceMove_.y());
    }

    void MoveNearWindowSidesBehaviour::update(FractalView &, BaseFractalRenderer &renderer, double deltaTime) {
        if (!isMoved_)
            return;

        renderer.viewPosition += differenceMove_ * (speed_ * static_cast<float>(deltaTime) / renderer.zoom);
    }
}
