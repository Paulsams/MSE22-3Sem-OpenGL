#pragma once

#include "Lights/PointLight.h"
#include "Base/Include/SceneLogic/SceneNode.h"

template <typename T>
class BaseLightsContainer : public BaseComponent {
public:
    virtual T& addLight() {
        const int index = static_cast<int>(lights_.size());
        const auto lightNode = SceneNode::create(
                std::string(T::defaultNameInHierarchy) + " " + std::to_string(index));
        std::unique_ptr<T> lightComponent = std::unique_ptr<T>(new T());
        T* ptrLightComponent = lightComponent.get();

        lightNode->addComponent(std::move(lightComponent));
        getNode().addChild(lightNode);

        lights_.push_back(ptrLightComponent);

        return *ptrLightComponent;
    }

    const char* getComponentName() override { return "Lights Container"; }

protected:
    std::vector<T*> lights_;
};
