#pragma once

#include "Hittables/model.h"
#include "tiny_gltf.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <unordered_map>
#include <filesystem>

class LoadModelIterator {
public:
    explicit LoadModelIterator(tinygltf::Model &model)
            : model_(model) {}

    std::vector<std::shared_ptr<model>> create(vec3 position, const std::filesystem::path& path);

private:
    void createAndBindRenderersForModelNodes(const tinygltf::Node &node, glm::mat4 modelMatrix);

    void createAndBindRenderersForMesh(tinygltf::Mesh &mesh, const glm::mat4x4& modelMatrix);

    std::vector<std::shared_ptr<model>> models_;
    tinygltf::Model &model_;
};
