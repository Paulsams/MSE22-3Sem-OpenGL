#pragma once

#include "tiny_gltf.h"
#include <unordered_map>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class SceneNode;

class LoadModelIterator {
public:
    explicit LoadModelIterator(QOpenGLFunctions &funcs,
                               const std::shared_ptr<QOpenGLShaderProgram> &program,
                               tinygltf::Model &model)
            : funcs_(funcs), program_(program), model_(model) {}

    std::shared_ptr<SceneNode> create();

private:
    void initBufferForModelNodes(tinygltf::Node &node);

    void initBufferForMesh(tinygltf::Mesh &mesh);

    std::shared_ptr<SceneNode> createAndBindRenderersForModelNodes(const tinygltf::Node &node);

    std::shared_ptr<SceneNode> createAndBindRenderersForMesh(tinygltf::Mesh &mesh);

    QOpenGLFunctions &funcs_;
    const std::shared_ptr<QOpenGLShaderProgram> &program_;
    tinygltf::Model &model_;
    std::unordered_map<int, tinygltf::BufferView> bufferViews_;
    // TODO: QOpenGLTexturePrivate::destroy() called without a current context. Texture has not been destroyed.
    std::vector<std::shared_ptr<QOpenGLTexture>> opengGLTextures_;
};
