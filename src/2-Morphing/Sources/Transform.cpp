#include "Transform.h"
#include "QTransform"
#include "QMatrix3x3"

Transform::Transform()
        : position_(QVector3D()),
          rotation_(QQuaternion()),
          scale_(QVector3D(1.0f, 1.0f, 1.0f)),
          modelMatrix_(QMatrix4x4()) {
    modelMatrix_.setToIdentity();
}

void Transform::setPosition(QVector3D position) {
    position_ = position;
    updateModelMatrix();
}

void Transform::setPosition(float x, float y, float z) {
    setPosition(QVector3D(x, y, z));
}

void Transform::setRotation(QQuaternion rotation) {
    rotation_ = rotation;
    updateModelMatrix();
}

void Transform::setScale(QVector3D scale) {
    scale_ = scale;
    updateModelMatrix();
}

void Transform::setScale(float x, float y, float z) {
    setScale(QVector3D(x, y, z));
}

void Transform::translate(QVector3D offset) {
    if (offset.isNull())
        return;

    setPosition(position_ + offset);
}

void Transform::scale(float factor) {
    scale_ *= factor;
    updateModelMatrix();
}

void Transform::setModelMatrix(QMatrix4x4 modelMatrix) {
    modelMatrix_ = modelMatrix;
    position_ = QVector3D(
            modelMatrix.row(0).w(),
            modelMatrix.row(1).w(),
            modelMatrix.row(2).w()
    );
    rotation_ = QQuaternion::fromRotationMatrix(QMatrix3x3(modelMatrix_.data()));
    scale_ = QVector3D(
            modelMatrix.row(0).x(),
            modelMatrix.row(1).y(),
            modelMatrix.row(2).z()
    );
}

void Transform::updateModelMatrix() {
    QMatrix4x4 translation;
    translation.translate(position_);
    QMatrix4x4 rotation;
    rotation.rotate(rotation_);
    QMatrix4x4 scale;
    scale.scale(scale_);

    modelMatrix_ = translation * rotation * scale;
}
