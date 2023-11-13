#pragma once

#include <QStandardItem>

class SceneNode;

class ItemInHierarchy final : public QStandardItem {
public:
    explicit ItemInHierarchy(SceneNode& node);

    SceneNode& getNode() { return node_; }

private:
    SceneNode& node_;
};

