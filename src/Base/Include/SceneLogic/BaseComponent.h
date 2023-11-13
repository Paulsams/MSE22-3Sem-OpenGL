#pragma once

#include <QObject>

class FieldsDrawer;
class InspectorView;
class SceneNode;
class Scene;

class BaseComponent : public QObject {
Q_OBJECT
public:
    [[nodiscard]] SceneNode &getNode() const { return *node_; }

    virtual const char* getComponentName() = 0;

protected:
    virtual void onAttach() { }

    virtual void onUpdate(float) { }

    virtual void onDetach() { }

    virtual bool drawValueInInspector(FieldsDrawer&) { return false; }

private:
    friend Scene;
    friend SceneNode;
    friend InspectorView;

    void attach(SceneNode* node) {
        node_ = node;
        onAttach();
    }

    void update(const float deltaTime) {
        onUpdate(deltaTime);
    }

    void detach() {
        node_ = nullptr;
    }

    SceneNode* node_ = nullptr;
};
