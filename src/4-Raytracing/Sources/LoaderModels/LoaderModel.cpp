#include "LoaderModels/LoaderModel.h"

#include <filesystem>

#include "LoaderModels/LoadModelIterator.h"

#include <iostream>

std::vector<std::shared_ptr<model>> LoaderModel::load(vec3 position, const char *filename) {
    const std::filesystem::path path(filename);
    const auto model = loadModel(path);
    if (!model)
        assert(0);

    auto models = LoadModelIterator(*model).create(position, path);
    return models;
}

std::unique_ptr<tinygltf::Model> LoaderModel::loadModel(const std::filesystem::path& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    const std::string pathInStr = path.string();
    bool result;
    if (path.extension() == ".gltf") {
        result = loader.LoadASCIIFromFile(&model, &err, &warn, pathInStr);
    } else {
        result = loader.LoadBinaryFromFile(&model, &err, &warn, pathInStr);
    }

    if (!warn.empty())
        std::cout << "WARN: " << warn << std::endl;

    if (!err.empty())
        std::cout << "ERR: " << err << std::endl;

    // TODO: более понятные ошибки. Например не нашёлся файл
    if (!result) {
        std::cout << "Failed to load glTF: " << pathInStr << std::endl;
        return nullptr;
    }

    std::cout << "Loaded glTF: " << pathInStr << std::endl;
    return std::make_unique<tinygltf::Model>(model);
}
