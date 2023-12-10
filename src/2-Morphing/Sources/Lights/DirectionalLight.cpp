#include "Lights/DirectionalLight.h"

#include "Base/Include/Views/FieldsDrawer.h"
#include "Base/Include/SceneLogic/SceneNode.h"
#include "3-SSAO/Include/Lights/DirectionalLight.h"


void DirectionalLight::setAllData(const QVector3D& color, const float ambientStrength) {
    uniformData_.color = color;
    uniformData_.ambientStrength = ambientStrength;
    updateLight();
}

void DirectionalLight::setColor(const QVector3D value) {
    uniformData_.color = value;
    updateLight();
}

void DirectionalLight::setAmbientStrength(const float value) {
    uniformData_.ambientStrength = value;
    updateLight();
}

void DirectionalLight::onUpdate(float) {
    const QVector3D currentDirection = getNode().getTransform().getRotation() *
                                       QVector3D(0.0f, 1.0f, 0.0f);

    if (uniformData_.direction != currentDirection) {
        uniformData_.direction = currentDirection;

        updateLight();
    }
}

bool DirectionalLight::drawValueInInspector(FieldsDrawer& drawer) {
    drawer.addColorPicker("Color", &uniformData_.color,
        [this](const QVector3D newValue) { setColor(newValue); });
    drawer.addSliderIn01Range("Ambient Strength", uniformData_.ambientStrength,
        [this](const float newValue) { setAmbientStrength(newValue); });

    return true;
}

DirectionalLight::DirectionalLight(LightsContainer<DirectionalLight>&container, const int indexInContainer)
    : BaseLight(container, indexInContainer) {
}
