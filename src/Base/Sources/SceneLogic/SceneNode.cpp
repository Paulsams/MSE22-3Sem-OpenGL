#include "SceneLogic/SceneNode.h"

#include <utility>

SceneNode::SceneNode(std::string name)
        : name_(std::move(name)) {}

void SceneNode::addChild(const std::shared_ptr<SceneNode> &child) {
    children_.push_back(child);
}

void SceneNode::addComponent(std::unique_ptr<BaseComponent> component) {
    // Не смог пока разобраться с enable_shared_from_this
//    component.attach(this->shared_from_this());
    component->attach(this);
    components_.push_back(std::move(component));
}

SceneNode::~SceneNode() {
    for (const auto &item: components_)
        item->detach();
}
