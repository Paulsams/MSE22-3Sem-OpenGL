#pragma once

#include "PointLight.h"
#include "Base/Include/SceneLogic/SceneNode.h"

namespace Deferred {
    template <typename T>
    class BaseLightsContainer : public BaseComponent {
    public:
        virtual T& addLight() {
            const int index = static_cast<int>(lights_.size());
            const auto lightNode = SceneNode::create(
                    std::string(T::defaultNameInHierarchy) + " " + std::to_string(index));
            std::unique_ptr<T> lightComponent = std::unique_ptr<T>(new T(texturesBind_));
            T* ptrLightComponent = lightComponent.get();

            lightNode->addComponent(std::move(lightComponent));
            getNode().addChild(lightNode);

            lights_.push_back(ptrLightComponent);

            return *ptrLightComponent;
        }

        const char* getComponentName() override { return "Lights Container"; }

    protected:
        BaseLightsContainer(std::vector<std::pair<const char*, int>>& texturesBind)
            : texturesBind_(texturesBind) {}

        std::vector<T*> lights_;
        std::vector<std::pair<const char*, int>> texturesBind_;
    };
}
