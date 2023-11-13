#include <memory>
#include <QMatrix4x4>

class SceneNode;

class Scene {
public:
    explicit Scene();

    void iterUpdate(float deltaTime);

    void iterDraw(const QMatrix4x4 &viewProjection);

    SceneNode &getRootNode() { return *rootNode_; }

private:
    void iterUpdateInternal(SceneNode &node, float deltaTime);

    void iterDrawInternal(SceneNode &node,
                          const QMatrix4x4 &modelMatrix,
                          const QMatrix4x4 &viewProjection);

    std::shared_ptr<SceneNode> rootNode_;
};
