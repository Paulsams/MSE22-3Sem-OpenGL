#pragma once

#include "Renderer.h"
#include "Transform.h"
#include "BaseComponent.h"

class SceneNode {
public:
    ~SceneNode();

    void addChild(const std::shared_ptr<SceneNode> &child);

    const std::vector<std::shared_ptr<SceneNode>> &getChildren() { return children_; }

    void iterateChildren(const std::function<void(SceneNode &)> &func) {
        for (auto &item: children_)
            func(*item);
    }

    Transform &getTransform() { return transform_; };

    Renderer *getRenderer() { return renderer_.get(); };

    const std::vector<std::unique_ptr<BaseComponent>> &getComponents() { return components_; }

    void iterateComponents(const std::function<void(BaseComponent &)> &func) {
        for (auto &item: components_)
            func(*item);
    }

    void addComponent(std::unique_ptr<BaseComponent> component);

    void changeRenderer(std::unique_ptr<Renderer> renderer) { renderer_ = std::move(renderer); };

    const std::string& getName() { return name_; }
    void setName(const std::string& name) { name_ = name; }

    [[nodiscard]] static std::shared_ptr<SceneNode> create(const std::string &name = "") {
        return std::shared_ptr<SceneNode>(new SceneNode(name));
    }

private:
    explicit SceneNode(std::string name = "");

    std::string name_;

    Transform transform_;
    std::unique_ptr<Renderer> renderer_;

    std::vector<std::unique_ptr<BaseComponent>> components_;
    std::vector<std::shared_ptr<SceneNode>> children_;
};
