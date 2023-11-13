#include <QQuaternion>
#include <QVector3D>
#include <QMatrix4x4>

class Transform {
public:
    Transform();

    QVector3D getPosition() { return position_; }

    QQuaternion getRotation() { return rotation_; }

    QVector3D getScale() { return scale_; }

    const QMatrix4x4 &getModelMatrix() { return modelMatrix_; }

    void setPosition(QVector3D position);

    void setPosition(float x, float y, float z);

    void setRotation(QQuaternion rotation);

    void setScale(QVector3D scale);

    void setScale(float x, float y, float z);

    void translate(QVector3D offset);
    void scale(float factor);

    void setModelMatrix(const QMatrix4x4&modelMatrix);

private:
    void updateModelMatrix();

    QQuaternion rotation_;
    QVector3D position_;
    QVector3D scale_;
    QMatrix4x4 modelMatrix_;
};
