#include "2-Morphing/Include/Camera.h"

Camera::Camera(float aspect, float fov, float zNear, float zFar)
        : aspect_(aspect), fov_(fov), zNear_(zNear), zFar_(zFar) {
    updateProjectionMatrix();
    updateViewMatrix();
}

void Camera::setAspect(float aspect) {
    aspect_ = aspect;
    updateProjectionMatrix();
}

void Camera::setPosition(QVector3D position) {
    position_ = position;
    updateViewMatrix();
}

void Camera::setRotation(QQuaternion rotation) {
    rotation_ = rotation;
    updateViewMatrix();
}

void Camera::move(QVector3D offset) {
    if (offset == QVector3D{})
        return;

    auto rotatedOffset = rotation_ * offset;
    setPosition(position_ - rotatedOffset);
}

void Camera::rotate(QQuaternion offset) {
    rotation_ *= offset;
    updateViewMatrix();
}

void Camera::updateProjectionMatrix() {
    projection_.setToIdentity();
    projection_.perspective(fov_, aspect_, zNear_, zFar_);
}

void Camera::updateViewMatrix() {
    view_.setToIdentity();
    QVector3D targetForward = rotation_ * QVector3D(0.0f, 0.0f, -1.0f);
    QVector3D targetUp = rotation_ * QVector3D(0.0f, 1.0f, 0.0f);
    view_.lookAt(position_, position_ + targetForward, targetUp);
}
