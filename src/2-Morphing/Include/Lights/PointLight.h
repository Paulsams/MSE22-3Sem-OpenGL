#pragma once

#include "BaseLight.h"

template<typename T>
class LightsContainer;

struct alignas(16) PointLightUniformData {
    alignas(16) QVector3D position{};
    float distance{};

    alignas(16) QVector3D ambient{};
    float constant{};

    alignas(16) QVector3D diffuse{};
    float linear{};

    alignas(16) QVector3D specular{};
    float quadratic{};
};

class PointLight final : public BaseLight<PointLight, PointLightUniformData> {
public:
    using UniformData = PointLightUniformData;

    void setAllData(float distance, QVector3D ambient, QVector3D diffuse, QVector3D specular,
                    float constant, float linear, float quadratic);

    [[nodiscard]] float getDistance() const { return uniformData_.distance; }

    void setDistance(float value);

    [[nodiscard]] QVector3D getAmbient() const { return uniformData_.ambient; }

    void setAmbient(QVector3D value);

    [[nodiscard]] QVector3D getDiffuse() const { return uniformData_.diffuse; }

    void setDiffuse(QVector3D value);

    [[nodiscard]] QVector3D getSpecular() const { return uniformData_.specular; }

    void setSpecular(QVector3D value);

    [[nodiscard]] float getConstant() const { return uniformData_.constant; }

    void setConstant(float value);

    [[nodiscard]] float getLinear() const { return uniformData_.linear; }

    void setLinear(float value);

    [[nodiscard]] float getQuadratic() const { return uniformData_.quadratic; }

    void setQuadratic(float value);

    const char* getComponentName() override { return "Point Light"; }

protected:
    void onUpdate(float) override;
    bool drawValueInInspector(FieldsDrawer& drawer) override;

private:
    static constexpr const char* defaultNameInHiearchy = "Point";
    friend LightsContainer<PointLight>;

    explicit PointLight(LightsContainer<PointLight>&container, int indexInContainer);
};
