#include "InputHandler.h"
#include <QKeyEvent>
#include <QApplication>

InputHandler::InputHandler() {
    QApplication::instance()->installEventFilter(this);
}

bool InputHandler::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
        emit keyPressed(keyEvent->key());

        return true;
    }

    if (event->type() == QEvent::KeyRelease) {
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
        emit keyReleased(keyEvent->key());
        return true;
    }

    return false;
}
