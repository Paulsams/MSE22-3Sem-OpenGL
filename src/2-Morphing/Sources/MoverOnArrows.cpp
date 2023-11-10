#include "MoverOnArrows.h"
#include "InputHandler.h"
#include <QApplication>
#include <QKeyEvent>

MoverOnArrows::MoverOnArrows(float speedMove)
        : speedMove_(speedMove), directionMove_(QVector3D()){
    connect(&InputHandler::getInstance(), &InputHandler::keyPressed,
            this, &MoverOnArrows::onKeyPressed);
    connect(&InputHandler::getInstance(), &InputHandler::keyReleased,
            this, &MoverOnArrows::onKeyRelease);
}

void MoverOnArrows::onUpdate(float deltaTime) {
    getNode().getTransform().translate(directionMove_ * (speedMove_ * deltaTime));
}

void MoverOnArrows::onKeyPressed(int keyCode) {
    directionMove_ += getMoveDirectionFromInput(keyCode);
}

void MoverOnArrows::onKeyRelease(int keyCode) {
    directionMove_ -= getMoveDirectionFromInput(keyCode);
}

QVector3D MoverOnArrows::getMoveDirectionFromInput(int keyCode) {
    switch (keyCode) {
        case Qt::Key_O:
            return {0, 1.0f, 0.0f};
        case Qt::Key_U:
            return {0, -1.0f, 0.0f};
        case Qt::Key_J:
            return {1.0f, 0.0f, 0.0f};
        case Qt::Key_L:
            return {-1.0f, 0.0f, 0.0f};
        case Qt::Key_K:
            return {0.0f, 0.0f, -1.0f};
        case Qt::Key_I:
            return {0.0f, 0.0f, 1.0f};
    }

    return {};
}
