#include "SceneNode.h"

SceneNode::SceneNode(const std::string &name)
        : name_(name) {}

void SceneNode::addChild(const std::shared_ptr<SceneNode>& child) {
    children_.push_back(child);
}
