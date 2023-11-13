#pragma once

#include "BaseLight.h"

template <typename T>
class LightsContainer;

struct DirectionalLightUniformData {
    alignas(16) QVector3D direction{};
    alignas(16) QVector3D color{};
    float ambientStrength{};
};

class DirectionalLight final : public BaseLight<DirectionalLight, DirectionalLightUniformData> {
public:
    void setAllData(const QVector3D& color, float ambientStrength);

    [[nodiscard]] QVector3D getColor() const { return uniformData_.color; }
    void setColor(QVector3D value);

    [[nodiscard]] float getAmbientStrength() const { return uniformData_.ambientStrength; }
    void setAmbientStrength(float value);

    const char* getComponentName() override { return "Directional Light"; }

protected:
    void onUpdate(float) override;

    bool drawValueInInspector(FieldsDrawer& drawer) override;

private:
    static constexpr const char* defaultNameInHiearchy = "Directional";
    friend LightsContainer<DirectionalLight>;

    explicit DirectionalLight(LightsContainer<DirectionalLight>& container, int indexInContainer);
};
