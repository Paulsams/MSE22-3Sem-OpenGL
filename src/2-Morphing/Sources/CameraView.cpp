#include "2-Morphing/Include/CameraView.h"
#include <QKeyEvent>
#include <QApplication>

CameraView::CameraView(Camera camera, QWidget *parent)
        : QWidget(parent), camera_(camera) {
    QApplication::instance()->installEventFilter(this);
    QWidget::setFocus();
    QWidget::setMouseTracking(true);
}

void CameraView::keyPressEvent(QKeyEvent *keyEvent) {
    directionMoveCamera_ += getMoveDirectionFromInput(keyEvent->key());
}

void CameraView::keyReleaseEvent(QKeyEvent *keyEvent) {
    directionMoveCamera_ -= getMoveDirectionFromInput(keyEvent->key());
}

QVector3D CameraView::getMoveDirectionFromInput(int keyCode) {
    switch (keyCode) {
        case Qt::Key_Q:
            return {0, 1.0f, 0.0f};
        case Qt::Key_E:
            return {0, -1.0f, 0.0f};
        case Qt::Key_A:
            return {1.0f, 0.0f, 0.0f};
        case Qt::Key_D:
            return {-1.0f, 0.0f, 0.0f};
        case Qt::Key_S:
            return {0.0f, 0.0f, -1.0f};
        case Qt::Key_W:
            return {0.0f, 0.0f, 1.0f};
    }

    return {};
}

bool CameraView::eventFilter(QObject *obj, QEvent *event) {
    switch (event->type()) {
        case QEvent::MouseButtonPress: {
            auto *mouseEvent = (QMouseEvent *) event;

            if (mouseEvent->button() == Qt::RightButton) {
                lastMousePosition_ = mouseEvent->localPos();
                isDragged_ = true;
            }

            return false;
        }
        case QEvent::MouseMove: {
            const float sensitivity = 0.1f;

            if (!isDragged_)
                return false;

            auto *mouseEvent = (QMouseEvent *) event;

            auto deltaMouse = mouseEvent->localPos() - lastMousePosition_;
            lastMousePosition_ = mouseEvent->localPos();

            deltaMouse *= sensitivity;
            QQuaternion offsetRotate = QQuaternion::fromEulerAngles(
                    deltaMouse.y(),
                    deltaMouse.x(),
                    0.0f);
            camera_.rotate(offsetRotate);

            return false;
        }
        case QEvent::MouseButtonRelease: {
            auto *mouseEvent = (QMouseEvent *) event;

            if (mouseEvent->button() == Qt::RightButton)
                isDragged_ = false;

            return false;
        }
        default:
            // Other event type checks here...
            return false; //the signal will be delivered other filters
    }
}
