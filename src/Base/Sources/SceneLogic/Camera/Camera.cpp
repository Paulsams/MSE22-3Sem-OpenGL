#include "SceneLogic/Camera/Camera.h"

Camera::Camera(float aspect, float fov, float zNear, float zFar)
        : aspect_(aspect), fov_(fov), zNear_(zNear), zFar_(zFar) {
    updateProjectionMatrix();
    updateViewMatrix();
}

void Camera::setAspect(float aspect) {
    aspect_ = aspect;
    updateProjectionMatrix();
}

void Camera::setPosition(const QVector3D position) {
    position_ = position;
    updateViewMatrix();
}

// void Camera::setRotation(const QQuaternion rotation) {
//     rotation_ = rotation;
//     updateViewMatrix();
// }

void Camera::move(const QVector3D offset) {
    if (offset == QVector3D{})
        return;

    const auto rotatedOffset = rotation_ * offset;
    setPosition(position_ + rotatedOffset);
}

void Camera::rotate(const float yaw, const float pitch) {
    yaw_ += yaw;
    pitch_ += pitch;
    updateViewMatrix();
}

void Camera::updateProjectionMatrix() {
    projection_.setToIdentity();
    projection_.perspective(fov_, aspect_, zNear_, zFar_);
}

void Camera::updateViewMatrix() {
    rotation_ = QQuaternion::fromEulerAngles(-pitch_, -yaw_, 0.0f);

    view_.setToIdentity();
    view_.translate(position_);

    QMatrix4x4 rotateMatrix;
    rotateMatrix.setToIdentity();
    rotateMatrix.rotate(yaw_, QVector3D(0, 1, 0));
    view_ = rotateMatrix * view_;
    rotateMatrix.setToIdentity();

    auto tempLastRow = view_.row(3);
    view_.setRow(3, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));

    rotateMatrix.rotate(pitch_, QVector3D(1, 0, 0));
    view_ = rotateMatrix * view_;
    view_.setRow(3, tempLastRow);
}
