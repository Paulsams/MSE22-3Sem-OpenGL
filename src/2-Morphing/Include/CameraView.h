#pragma once

#include <QWidget>
#include "Camera.h"

class CameraView : QWidget {
Q_OBJECT
public:
    explicit CameraView(Camera camera, QWidget *parent = nullptr);

    Camera &getCamera() { return camera_; }

    QVector3D getDirectionMoveCamera() { return directionMoveCamera_; }

protected:
    void keyPressEvent(QKeyEvent *keyEvent) override;

    void keyReleaseEvent(QKeyEvent *keyEvent) override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    QVector3D getMoveDirectionFromInput(int keyCode);

    QPointF lastMousePosition_;
    bool isDragged_ = false;
    Camera camera_;
    QVector3D directionMoveCamera_;
};
