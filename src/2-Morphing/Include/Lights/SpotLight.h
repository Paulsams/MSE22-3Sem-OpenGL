#pragma once

#include "BaseLight.h"

struct SpotLightUniformData {
    alignas(16) QVector3D position;
    float distance;

    alignas(16) QVector3D direction;
    float innerCutOff;

    alignas(16) QVector3D ambient;
    float outerCutOff;

    alignas(16) QVector3D diffuse;

    alignas(16) QVector3D specular;
};

class SpotLight final : public BaseLight<SpotLight, SpotLightUniformData> {
public:
    void setAllData(float distance, float innerCutOffAngle, float outerCutOffAngle,
                    const QVector3D&ambient, const QVector3D&diffuse, const QVector3D&specular);

    [[nodiscard]] float getDistance() const { return uniformData_.distance; }
    void setDistance(float value);

    [[nodiscard]] QVector3D getAmbient() const { return uniformData_.ambient; }
    void setAmbient(QVector3D value);

    [[nodiscard]] QVector3D getDiffuse() const { return uniformData_.diffuse; }
    void setDiffuse(QVector3D value);

    [[nodiscard]] QVector3D getSpecular() const { return uniformData_.specular; }
    void setSpecular(QVector3D value);

    [[nodiscard]] float getInnerCutOffAngle() const { return innerCutOffAngle_; }
    void setInnerCutOffAngle(float value);

    [[nodiscard]] float getOuterCutOffAngle() const { return outerCutOffAngle_; }
    void setOuterCutOffAngle(float value);

    const char* getComponentName() override { return "Spot Light"; }

protected:
    void onUpdate(float) override;
    bool drawValueInInspector(FieldsDrawer& drawer) override;

private:
    static constexpr const char* defaultNameInHiearchy = "Spot";
    friend LightsContainer<SpotLight>;

    float outerCutOffAngle_{};
    float innerCutOffAngle_{};

    explicit SpotLight(LightsContainer<SpotLight>&container, int indexInContainer);
};
