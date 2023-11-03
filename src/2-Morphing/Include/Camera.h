#pragma once

#include <QMatrix4x4>

class Camera {
public:
    explicit Camera(float aspect, float fov = 60.0f, float zNear = 0.1f, float zFar = 100.0f);

    void setAspect(float aspect);

    QMatrix4x4 getProjectionMatrix() { return projection_; }

    QMatrix4x4 getViewMatrix() { return view_; }

    QVector3D getPosition() { return position_; }

    QQuaternion getRotation() { return rotation_; }

    void setPosition(QVector3D position);

    void setRotation(QQuaternion rotation);

    void move(QVector3D offset);

    void rotate(QQuaternion offset);

private:
    void updateProjectionMatrix();

    void updateViewMatrix();

    QMatrix4x4 projection_;
    QMatrix4x4 view_;

    QQuaternion rotation_;
    QVector3D position_;

    float aspect_;
    float fov_;
    float zNear_;
    float zFar_;
};
