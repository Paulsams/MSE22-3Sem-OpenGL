#include "SceneLogic/Camera/CameraView.h"
#include "Utils/InputHandler.h"
#include <QKeyEvent>
#include <QApplication>

CameraView::CameraView(Camera camera, QWidget* parent)
    : QWidget(parent), camera_(camera) {
    QApplication::instance()->installEventFilter(this);
    QWidget::setMouseTracking(true);

    connect(&InputHandler::getInstance(), &InputHandler::keyPressed,
            this, &CameraView::onKeyPressed);
    connect(&InputHandler::getInstance(), &InputHandler::keyReleased,
            this, &CameraView::onKeyRelease);
}

void CameraView::onKeyPressed(int keyCode) {
    setMoveDirectionFromInput(keyCode, 1);

    switch (keyCode) {
        case Qt::Key_Shift:
            _fastSpeedModeKeyPress = true;
            break;
    }
}

void CameraView::onKeyRelease(int keyCode) {
    setMoveDirectionFromInput(keyCode, 0);

    switch (keyCode) {
        case Qt::Key_Shift:
            _fastSpeedModeKeyPress = false;
            break;
    }
}

void CameraView::setMoveDirectionFromInput(const int keyCode, const int modifier) {
    switch (keyCode) {
        case Qt::Key_Q:
            directionMoveCamera_.setY(1.0f * modifier);
            break;
        case Qt::Key_E:
            directionMoveCamera_.setY(-1.0f * modifier);
            break;
        case Qt::Key_A:
            directionMoveCamera_.setX(1.0f * modifier);
            break;
        case Qt::Key_D:
            directionMoveCamera_.setX(-1.0f * modifier);
            break;
        case Qt::Key_S:
            directionMoveCamera_.setZ(-1.0f * modifier);
            break;
        case Qt::Key_W:
            directionMoveCamera_.setZ(1.0f * modifier);
            break;
    }
}

bool CameraView::eventFilter(QObject*, QEvent* event) {
    switch (event->type()) {
        case QEvent::MouseButtonPress: {
            auto* mouseEvent = (QMouseEvent *)event;

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

            auto* mouseEvent = (QMouseEvent *)event;

            auto deltaMouse = mouseEvent->localPos() - lastMousePosition_;
            lastMousePosition_ = mouseEvent->localPos();

            deltaMouse *= sensitivity;
            camera_.rotate(deltaMouse.x(), deltaMouse.y());

            return false;
        }
        case QEvent::MouseButtonRelease: {
            auto* mouseEvent = (QMouseEvent *)event;

            if (mouseEvent->button() == Qt::RightButton)
                isDragged_ = false;

            return false;
        }
        default:
            // Other event type checks here...
            return false; //the signal will be delivered other filters
    }
}
