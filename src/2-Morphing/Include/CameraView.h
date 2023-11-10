#pragma once

#include <QWidget>
#include "Camera.h"

class CameraView : public QWidget {
Q_OBJECT
public:
    explicit CameraView(Camera camera, QWidget *parent = nullptr);

    bool isInFastSpeedMode() const { return _fastSpeedModeKeyPress; }

    Camera &getCamera() { return camera_; }

    QVector3D getDirectionMoveCamera() { return directionMoveCamera_; }

private slots:
    void onKeyPressed(int keyCode);

    void onKeyRelease(int keyCode);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    static QVector3D getMoveDirectionFromInput(int keyCode);

    QPointF lastMousePosition_;
    bool isDragged_ = false;
    Camera camera_;
    QVector3D directionMoveCamera_;
    bool _fastSpeedModeKeyPress = false;
};
