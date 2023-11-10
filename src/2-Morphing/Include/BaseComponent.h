#pragma once

#include <memory>
#include <utility>
#include <QObject>

class SceneNode;
class Scene;

class BaseComponent : public QObject {
Q_OBJECT;
public:
    SceneNode &getNode() { return *node_; }

protected:
    virtual void onAttach() { }

    virtual void onUpdate(float) { }

    virtual void onDetach() { }

private:
    friend Scene;
    friend SceneNode;

    void attach(SceneNode* node) {
        node_ = node;
        onAttach();
    }

    void update(float deltaTime) {
        onUpdate(deltaTime);
    }

    void detach() {
        node_ = nullptr;
    }

    SceneNode* node_ = nullptr;
};
