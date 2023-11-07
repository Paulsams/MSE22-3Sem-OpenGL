#pragma once

#include "Renderer.h"

class SceneNode {
public:
    explicit SceneNode(const std::string& name = "");

    void addChild(const std::shared_ptr<SceneNode>& child);

    Renderer &getRenderer() { return *renderer_; };

    void changeRenderer(std::unique_ptr<Renderer> renderer) { renderer_ = std::move(renderer); };
private:
    const std::string& name_;

    std::unique_ptr<Renderer> renderer_;

    std::vector<std::shared_ptr<SceneNode>> children_;
};
