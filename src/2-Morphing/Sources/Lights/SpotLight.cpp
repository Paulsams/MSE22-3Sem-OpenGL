#include "Lights/SpotLight.h"
#include "Base/Include/Views/FieldsDrawer.h"

#include <QtMath>

void SpotLight::setAllData(const float distance, const float innerCutOffAngle, const float outerCutOffAngle,
                           const QVector3D& ambient, const QVector3D& diffuse, const QVector3D& specular) {
    uniformData_.distance = distance;
    innerCutOffAngle_ = innerCutOffAngle;
    uniformData_.innerCutOff = std::cos(qDegreesToRadians(innerCutOffAngle));
    outerCutOffAngle_ = outerCutOffAngle;
    uniformData_.outerCutOff = std::cos(qDegreesToRadians(outerCutOffAngle));
    uniformData_.ambient = ambient;
    uniformData_.diffuse = diffuse;
    uniformData_.specular = specular;
    updateLight();
}

void SpotLight::setDistance(const float value) {
    uniformData_.distance = value;
    updateLight();
}

void SpotLight::setAmbient(const QVector3D value) {
    uniformData_.ambient = value;
    updateLight();
}

void SpotLight::setDiffuse(const QVector3D value) {
    uniformData_.diffuse = value;
    updateLight();
}

void SpotLight::setSpecular(const QVector3D value) {
    uniformData_.specular = value;
    updateLight();
}

void SpotLight::setInnerCutOffAngle(const float value) {
    innerCutOffAngle_ = value;
    uniformData_.innerCutOff = std::cos(qDegreesToRadians(value));
    updateLight();
}

void SpotLight::setOuterCutOffAngle(const float value) {
    outerCutOffAngle_ = value;
    uniformData_.outerCutOff = std::cos(qDegreesToRadians(value));
    updateLight();
}

void SpotLight::onUpdate(float) {
    const QVector3D currentPosition = getNode().getTransform().getPosition();
    const QVector3D currentDirection = getNode().getTransform().getRotation() *
                                       QVector3D(0.0f, 1.0f, 0.0f);

    if (uniformData_.direction != currentDirection ||
        uniformData_.position != currentPosition) {
        uniformData_.position = currentPosition;
        uniformData_.direction = currentDirection;

        updateLight();
    }
}

bool SpotLight::drawValueInInspector(FieldsDrawer& drawer) {
    drawer.addSpinBox("Distance", uniformData_.distance, 0.01f, 100.0f, 0.5f,
    [this](const float newValue) { setDistance(newValue); });
    drawer.addColorPicker("Ambient", &uniformData_.ambient,
        [this](const QVector3D newValue) { setAmbient(newValue); });
    drawer.addColorPicker("Diffuse", &uniformData_.diffuse,
        [this](const QVector3D newValue) { setDiffuse(newValue); });
    drawer.addColorPicker("Specular", &uniformData_.specular,
        [this](const QVector3D newValue) { setSpecular(newValue); });

    // TODO: валидироать значение, чтобы оно было не больше/меньше другого
    drawer.addSlider("Inner Cut Off", innerCutOffAngle_, 0.0f, 90.0f,
        [this](const float newValue) { setInnerCutOffAngle(newValue); });
    drawer.addSlider("Outer Cut Off", outerCutOffAngle_, 0.0f, 90.0f,
        [this](const float newValue) { setOuterCutOffAngle(newValue); });

    return true;
}

SpotLight::SpotLight(LightsContainer<SpotLight>&container, const int indexInContainer)
    : BaseLight(container, indexInContainer) {
}
