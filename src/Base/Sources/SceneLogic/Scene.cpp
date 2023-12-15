#include "SceneLogic/Scene.h"
#include "SceneLogic/SceneNode.h"

Scene::Scene()
        : rootNode_(SceneNode::create()) {}

void Scene::iterUpdate(float deltaTime) {
    iterUpdateInternal(*rootNode_, deltaTime);
}

void Scene::iterDraw(const QMatrix4x4& view, const QMatrix4x4& viewProjection) {
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    iterDrawInternal(*rootNode_, modelMatrix, view, viewProjection);
}

void Scene::iterUpdateInternal(SceneNode &node, float deltaTime) {
    node.iterateChildren([deltaTime, this](SceneNode& child) {
        child.iterateComponents([deltaTime](BaseComponent& component) {
            component.update(deltaTime);
        });
        iterUpdateInternal(child, deltaTime);
    });
}

void Scene::iterDrawInternal(
        SceneNode &node,
        const QMatrix4x4 &modelMatrix,
        const QMatrix4x4& view,
        const QMatrix4x4 &viewProjection
) {
    for (const auto& child: node.getChildren()) {
        QMatrix4x4 currentModelMatrix;
        if (node.getTransform().getModelMatrix().isIdentity()) {
            currentModelMatrix = modelMatrix;
        } else {
            currentModelMatrix = modelMatrix * node.getTransform().getModelMatrix();
        }

        auto renderer = child->getRenderer();
        if (renderer)
            renderer->draw(currentModelMatrix, view, viewProjection);

        iterDrawInternal(*child, currentModelMatrix, view, viewProjection);
    }
}
