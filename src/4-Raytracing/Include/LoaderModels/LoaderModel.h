#pragma once

#include "tiny_gltf.h"
#include "Hittables/model.h"

#include <filesystem>

class SceneNode;

class LoaderModel {
public:
    static std::vector<std::shared_ptr<model>> load(vec3 position, const char *filename);

private:
    static std::unique_ptr<tinygltf::Model> loadModel(const std::filesystem::path& path);
};
