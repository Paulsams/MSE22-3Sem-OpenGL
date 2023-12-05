#include "LoaderModels/LoaderModel.h"

#include <filesystem>

#include "SceneLogic/SceneNode.h"
#include "LoaderModels/LoadModelIterator.h"

#include <iostream>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

std::shared_ptr<SceneNode> LoaderModel::load(const char *filename, QOpenGLFunctions &glFuncs,
                                             const std::shared_ptr<QOpenGLShaderProgram> &program) {
    const std::filesystem::path path(filename);
    const auto model = loadModel(path);
    if (!model)
        return nullptr;

    auto newNode = LoadModelIterator(glFuncs, program, *model).create(path);
    return newNode;
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
