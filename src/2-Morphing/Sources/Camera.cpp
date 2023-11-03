#include "2-Morphing/Include/Camera.h"

Camera::Camera(float aspect, float fov, float zNear, float zFar)
        : aspect_(aspect), fov_(fov), zNear_(zNear), zFar_(zFar) {
    updateProjectionMatrix();
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

    QMatrix4x4 offsetMatrix;
    offsetMatrix.translate(offset);

    QMatrix4x4 rotationMatrix;
    rotationMatrix.rotate(rotation_);
    rotationMatrix = rotationMatrix.transposed();

    QMatrix4x4 positionMatrix;
    positionMatrix.translate(position_);
    auto matrix = positionMatrix - offsetMatrix * rotationMatrix;

    setPosition(matrix.column(3).toVector3D());
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
    QMatrix4x4 translateMatrix;
    translateMatrix.translate(position_);

    QMatrix4x4 rotationMatrix(rotation_.toRotationMatrix());

    view_ = rotationMatrix * translateMatrix;
}
