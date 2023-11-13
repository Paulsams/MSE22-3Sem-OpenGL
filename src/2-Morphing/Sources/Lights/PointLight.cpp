#include "Lights/PointLight.h"

#include "Base/Include/SceneLogic/SceneNode.h"
#include "Base/Include/Views/FieldsDrawer.h"

void PointLight::setAllData(float distance, QVector3D ambient, QVector3D diffuse, QVector3D specular, float constant,
                            float linear, float quadratic) {
    uniformData_.distance = distance;
    uniformData_.ambient = ambient;
    uniformData_.diffuse = diffuse;
    uniformData_.specular = specular;
    uniformData_.constant = constant;
    uniformData_.linear = linear;
    uniformData_.quadratic = quadratic;
    updateLight();
}

void PointLight::setDistance(const float value) {
    uniformData_.distance = value;
    updateLight();
}

void PointLight::setAmbient(const QVector3D value) {
    uniformData_.ambient = value;
    updateLight();
}

void PointLight::setDiffuse(const QVector3D value) {
    uniformData_.diffuse = value;
    updateLight();
}

void PointLight::setSpecular(const QVector3D value) {
    uniformData_.specular = value;
    updateLight();
}

void PointLight::setConstant(const float value) {
    uniformData_.constant = value;
    updateLight();
}

void PointLight::setLinear(const float value) {
    uniformData_.linear = value;
    updateLight();
}

void PointLight::setQuadratic(const float value) {
    uniformData_.quadratic = value;
    updateLight();
}

void PointLight::onUpdate(float) {
    // TODO: нужна не локал позиция, а глобал
    const QVector3D currentPosition = getNode().getTransform().getPosition();
    if (uniformData_.position != currentPosition) {
        uniformData_.position = currentPosition;
        updateLight();
    }
}

bool PointLight::drawValueInInspector(FieldsDrawer& drawer) {
    drawer.addSpinBox("Distance", uniformData_.distance, 0.01f, 100.0f, 0.5f,
        [this](const float newValue) { setDistance(newValue); });
    drawer.addColorPicker("Ambient", &uniformData_.ambient,
        [this](const QVector3D newValue) { setAmbient(newValue); });
    drawer.addColorPicker("Diffuse", &uniformData_.diffuse,
        [this](const QVector3D newValue) { setDiffuse(newValue); });
    drawer.addColorPicker("Specular", &uniformData_.specular,
        [this](const QVector3D newValue) { setSpecular(newValue); });
    drawer.addSpinBox("Constant", uniformData_.constant, 0.0f, 10.0f, 0.1f,
        [this](const float newValue) { setConstant(newValue); });
    drawer.addSpinBox("Linear", uniformData_.linear, 0.0f, 10.0f, 0.1f,
        [this](const float newValue) { setLinear(newValue); });
    drawer.addSpinBox("Quadratic", uniformData_.quadratic, 0.0f, 10.0f, 0.1f,
        [this](const float newValue) { setQuadratic(newValue); });

    return true;
}

PointLight::PointLight(LightsContainer<PointLight>&container, const int indexInContainer)
    : BaseLight(container, indexInContainer) {
}
