#include "Utils/InputHandler.h"

#include <QKeyEvent>
#include <QApplication>

InputHandler::InputHandler() {
    QApplication::instance()->installEventFilter(this);
}

bool InputHandler::eventFilter(QObject*, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = dynamic_cast<QKeyEvent *>(event);
        emit keyPressed(keyEvent->key());

        return false;
    }

    if (event->type() == QEvent::KeyRelease) {
        auto* keyEvent = dynamic_cast<QKeyEvent *>(event);
        emit keyReleased(keyEvent->key());
        return false;
    }

    return false;
}
