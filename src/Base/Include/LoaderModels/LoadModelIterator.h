#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "tiny_gltf.h"

#include <unordered_map>
#include <filesystem>

class SceneNode;

// TODO: сделать так, чтобы он возвращал некий меш, а затем по нему просто дублировать модели, но исопльзуя общие буфферы
class LoadModelIterator {
public:
    explicit LoadModelIterator(QOpenGLFunctions &funcs,
                               const std::shared_ptr<QOpenGLShaderProgram> &program,
                               tinygltf::Model &model)
            : funcs_(funcs), program_(program), model_(model) {}

    std::shared_ptr<SceneNode> create(const std::filesystem::path& path);

private:
    void initBufferForModelNodes(tinygltf::Node &node);

    void initBufferForMesh(tinygltf::Mesh &mesh);

    std::shared_ptr<SceneNode> createAndBindRenderersForModelNodes(const tinygltf::Node &node);

    std::shared_ptr<SceneNode> createAndBindRenderersForMesh(tinygltf::Mesh &mesh);

    QOpenGLFunctions &funcs_;
    const std::shared_ptr<QOpenGLShaderProgram> &program_;
    tinygltf::Model &model_;
    std::unordered_map<int, tinygltf::BufferView> bufferViews_;
    std::vector<std::shared_ptr<QOpenGLTexture>> opengGLTextures_;

    // TODO: нужно не каждый раз её создавать, а шарить между всеми, поэтому нужно делать вместе с системой мешей и общими буферами
    std::shared_ptr<QOpenGLTexture> _defaultTexture;
};
